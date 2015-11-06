#include <cassert>
#include <unistd.h>
#include "threadPool.h"

using namespace std;

void* threadMain(void* dataStructPtr);

void lockMutex  (pthread_mutex_t &mutex);
void unlockMutex(pthread_mutex_t &mutex);

//===============================================

ThreadPool::ThreadPool(const unsigned int numberOfThreads):
numberOfThreads(numberOfThreads)
{
	threads = new ThreadData[numberOfThreads];
	
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		const int mutexInitStatus =
				pthread_mutex_init(&threads[j].queueMutex, NULL);
		if(mutexInitStatus > 0)
			throw mutexInitStatus;
		
		const int threadCreateStatus =
				pthread_create(&threads[j].thread, NULL,
						&threadMain, &threads[j]);
		if(threadCreateStatus > 0)
			throw threadCreateStatus;
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
		const int joinStatus = pthread_join(threads[j].thread, NULL);
		if(joinStatus > 0)
			throw joinStatus;
		
		const int mutexStatus = 
				pthread_mutex_destroy(&threads[j].queueMutex);
		if(mutexStatus > 0)
			throw mutexStatus;
	}
	
	delete [] threads;
}

string ThreadPool::findResult(
		bool(*function)(string&, const unsigned long),
		unsigned long blockSize)
{
	//TODO
	return "string";
}

//===============================================

void* threadMain(void* dataStructPtr)
{
	ThreadPool::ThreadData *data =
			reinterpret_cast<ThreadPool::ThreadData*>(dataStructPtr);
	
	bool commandQueueWasEmpty;
	ThreadPool::ThreadData::ThreadCommand command;
	
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
			//TODO case run function
			default:
				assert(false);
			}
	}
}

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
