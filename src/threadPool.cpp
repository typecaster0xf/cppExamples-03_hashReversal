#include <cassert>
#include <unistd.h>
#include "threadPool.h"

using namespace std;

void lockMutex  (pthread_mutex_t &mutex);
void unlockMutex(pthread_mutex_t &mutex);

/*Ensures that each thread's queue has 3 blocks
to run.*/
void fillRunQueues(ThreadPool::ThreadData* threads,
		const unsigned int numberOfThreads,
		unsigned long &currentBlockNumber,
		bool(*function)(string&, const string,
				const unsigned long),
		const string& consistendParam,
		const unsigned long blockSize);

/*Used by fillRunQueues() to generate a node to
put on the command queue.*/
ThreadPool::ThreadData::ThreadCommand makeRunCommand(
		bool(*function)(string&, const string,
				const unsigned long),
		const string& consistentParam,
		unsigned long blockSize, unsigned long blockNumber);

/*Used by findResult() to check if a solution has
been found.  If it has, this function will return
true and the "solution" string will be set.*/
bool getSolution(ThreadPool::ThreadData* threads,
		const unsigned int numberOfThreads,
		string& solution);

//===Worker Thread Functions===

void* threadMain(void* dataStructPtr);

/*Runs the block of function calls.  Returns true
if a function call succeeded and will set the
"result" parameter.*/
bool runFunctionSearch(bool (*function)(std::string&,
				const string, const unsigned long),
		const string *consistentParam,
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
	ThreadData::ThreadCommand terminateCommand
	{
		ThreadData::ThreadCommandType::TERMINATE,
	};
	
	/*Send each thread the terminate command.*/
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
}//ThreadPool::~ThreadPool()

string ThreadPool::findResult(
		bool(*function)(string&, const string,
				const unsigned long),
		const string functionArg,
		unsigned long blockSize)
{
	unsigned long currentBlockNumber = 0;
	
	string solutionString;
	
	do
	{
		fillRunQueues(threads, numberOfThreads,
				currentBlockNumber, function,
				functionArg, blockSize);
		
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
		bool(*function)(string&, const string,
				const unsigned long),
		const string& consistentParam,
		const unsigned long blockSize)
{
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		lockMutex(threads[j].queueMutex);
		
		/*This only adds to the queue if it had
		less than three nodes pushed to begin
		with.*/
		for(unsigned int k = threads[j].commandQueue.size();
				k < 3; k++)
			threads[j].commandQueue.push(makeRunCommand(
					function, consistentParam,
					blockSize, currentBlockNumber++));
		
		unlockMutex(threads[j].queueMutex);
	}
	
	return;
}

ThreadPool::ThreadData::ThreadCommand makeRunCommand(
		bool(*function)(string&, const string,
				const unsigned long),
		const string& consistentParam,
		unsigned long blockSize, unsigned long blockNumber)
{
	return ThreadPool::ThreadData::ThreadCommand
	{
		ThreadPool::ThreadData::ThreadCommandType
				::RUN_FUNCTION_SEARCH,
		
		function,
		
		&consistentParam,
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
						command.consistentParam,
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
}//threadMain

bool runFunctionSearch(bool (*function)(
				string&, const string, const unsigned long),
		const string *consistentParam,
		const unsigned long start,
		const unsigned long blockSize,
		string& result)
{
	for(unsigned long j = start, count = 0;
			count < blockSize; j++, count++)
		if(function(result, *consistentParam, j))
			return true;
	
	return false;
}

//===============================================

#ifdef UNITTEST
#include <iostream>

/*Example function to be passed to
findResult().*/
bool isNumber3000(string& returnString, const string,
		const unsigned long number)
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
	
	assert(threadPool->findResult(&isNumber3000, "")
			== "Number is 3000");
	
	delete threadPool;
	
	cout << "class ThreadPool unit test passed." << endl;
	return 0;
}
#endif
