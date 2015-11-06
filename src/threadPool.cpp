#include <cassert>
#include <unistd.h>
#include "threadPool.h"

using namespace std;

void lockMutex  (pthread_mutex_t &mutex);
void unlockMutex(pthread_mutex_t &mutex);

void fillRunQueues(ThreadPool::ThreadData* threads,
		const unsigned int numberOfThreads,
		unsigned long &currentBlockNumber,
		bool(*function)(string&, const unsigned long),
		const unsigned long blockSize);

ThreadPool::ThreadData::ThreadCommand makeRunCommand(
		bool(*function)(string&, const unsigned long),
		unsigned long blockSize, unsigned long blockNumber);

bool getSolution(ThreadPool::ThreadData* threads,
		const unsigned int numberOfThreads,
		string& solution);

//======

void* threadMain(void* dataStructPtr);

bool runFunctionSearch(bool (*function)(
				std::string&, const unsigned long),
		const unsigned long start,
		const unsigned long blockSize,
		string& result);

//===============================================

ThreadPool::ThreadPool(const unsigned int numberOfThreads):
numberOfThreads(numberOfThreads)
{
	threads = new ThreadData[numberOfThreads];
	
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		{
			const int mutexInitStatus =
					pthread_mutex_init(&threads[j].queueMutex, NULL);
			if(mutexInitStatus > 0)
				throw mutexInitStatus;
		}
		
		{
			const int mutexInitStatus =
					pthread_mutex_init(&threads[j].returnMutex, NULL);
			if(mutexInitStatus > 0)
				throw mutexInitStatus;
		}
		
		threads[j].hasReturnData = false;
		
		{
			const int threadCreateStatus =
					pthread_create(&threads[j].thread, NULL,
							&threadMain, &threads[j]);
			if(threadCreateStatus > 0)
				throw threadCreateStatus;
		}
	}
}

ThreadPool::~ThreadPool()
{
	ThreadPool::ThreadData::ThreadCommand terminateCommand
	{
		ThreadPool::ThreadData::ThreadCommandType::TERMINATE,
		NULL,
		0,
		0
	};
	
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		lockMutex(threads[j].queueMutex);
		threads[j].commandQueue.push(terminateCommand);
		unlockMutex(threads[j].queueMutex);
	}
	
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		{
			const int joinStatus = pthread_join(
					threads[j].thread, NULL);
			if(joinStatus > 0)
				throw joinStatus;
		}
		
		{
			const int mutexStatus = 
					pthread_mutex_destroy(&threads[j].queueMutex);
			if(mutexStatus > 0)
				throw mutexStatus;
		}
		
		{
			const int mutexStatus = 
					pthread_mutex_destroy(&threads[j].returnMutex);
			if(mutexStatus > 0)
				throw mutexStatus;
		}
	}
	
	delete [] threads;
}

string ThreadPool::findResult(
		bool(*function)(string&, const unsigned long),
		unsigned long blockSize)
{
	unsigned long currentBlockNumber = 0;
	
	string solutionString;
	
	do
	{
		fillRunQueues(threads, numberOfThreads,
				currentBlockNumber, function, blockSize);
		
		usleep(100);
	}while(!getSolution(threads, numberOfThreads, solutionString));
	
	return solutionString;
}

//===============================================

void lockMutex(pthread_mutex_t &mutex)
{
	const int mutexStatus = pthread_mutex_lock(&mutex);
	
	if(mutexStatus > 0)
		throw mutexStatus;
	
	return;
}

void unlockMutex(pthread_mutex_t &mutex)
{
	const int mutexStatus = pthread_mutex_unlock(&mutex);
	
	if(mutexStatus > 0)
		throw mutexStatus;
	
	return;
}

void fillRunQueues(ThreadPool::ThreadData* threads,
		const unsigned int numberOfThreads,
		unsigned long &currentBlockNumber,
		bool(*function)(string&, const unsigned long),
		const unsigned long blockSize)
{
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		lockMutex(threads[j].queueMutex);
		
		for(unsigned int k = threads[j].commandQueue.size();
				k < 3; k++)
			threads[j].commandQueue.push(makeRunCommand(
					function, blockSize, currentBlockNumber++));
		
		unlockMutex(threads[j].queueMutex);
	}
	
	return;
}

ThreadPool::ThreadData::ThreadCommand makeRunCommand(
		bool(*function)(string&, const unsigned long),
		unsigned long blockSize, unsigned long blockNumber)
{
	return ThreadPool::ThreadData::ThreadCommand
	{
		ThreadPool::ThreadData::ThreadCommandType
				::RUN_FUNCTION_SEARCH,
		function,
		blockSize * blockNumber,
		blockSize
	};
}

bool getSolution(ThreadPool::ThreadData* threads,
		const unsigned int numberOfThreads,
		string& solution)
{
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		lockMutex(threads[j].returnMutex);
		
		if(threads[j].hasReturnData)
		{
			solution = threads[j].returnString;
			threads[j].hasReturnData = false;
			unlockMutex(threads[j].returnMutex);
			return true;
		}
		
		unlockMutex(threads[j].returnMutex);
	}
	
	return false;
}

//---------------------------

void* threadMain(void* dataStructPtr)
{
	ThreadPool::ThreadData *data =
			reinterpret_cast<ThreadPool::ThreadData*>(dataStructPtr);
	
	bool commandQueueWasEmpty;
	ThreadPool::ThreadData::ThreadCommand command;
	
	string resultString;
	
	while(true)
	{
		/*Get the next command sent to this
		thread.*/
		lockMutex(data->queueMutex);
		commandQueueWasEmpty = data->commandQueue.empty();
		if(!commandQueueWasEmpty)
		{
			command = data->commandQueue.front();
			data->commandQueue.pop();
		}
		unlockMutex(data->queueMutex);
		
		/*Process the command if there was one.*/
		if(commandQueueWasEmpty)
			usleep(40);
		else
			switch(command.commandType)
			{
			case ThreadPool::ThreadData::
					ThreadCommandType::TERMINATE:
				return NULL;
			case ThreadPool::ThreadData::
					ThreadCommandType::RUN_FUNCTION_SEARCH:
				if(runFunctionSearch(command.function,
						command.startValue,
						command.blockSize,
						resultString))
				{
					lockMutex(data->returnMutex);
					
					assert(data->hasReturnData == false);
					data->hasReturnData = true;
					data->returnString  = resultString;
					
					unlockMutex(data->returnMutex);
				}
				break;
			default:
				assert(false);
			}
	}
}

bool runFunctionSearch(bool (*function)(
				std::string&, const unsigned long),
		const unsigned long start,
		const unsigned long blockSize,
		string& result)
{
	for(unsigned long j = start, count = 0;
			count < blockSize; j++, count++)
		if(function(result, j))
			return true;
	
	return false;
}

//===============================================

#ifdef UNITTEST
#include <iostream>

using namespace std;

bool isNumber3000(string& returnString, const unsigned long number)
{
	if(number == 3000)
	{
		returnString = "Number is 3000";
		return true;
	}else
		return false;
}

int main()
{
	ThreadPool *threadPool = new ThreadPool(3);
	
	assert(threadPool->findResult(&isNumber3000)
			== "Number is 3000");
	
	delete threadPool;
	
	cout << "class ThreadPool unit test passed." << endl;
	return 0;
}
#endif
