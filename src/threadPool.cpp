#include <unistd.h>
#include "threadPool.h"

using namespace std;

void* threadMain(void* dataStructPtr);

void lockMutex  (pthread_mutex_t &mutex);
void unlockMutex(pthread_mutex_t &mutex);

bool isThreadAlive(ThreadPool::ThreadData* threadData);

//===============================================

ThreadPool::ThreadPool(const unsigned int numberOfThreads):
numberOfThreads(numberOfThreads)
{
	threads = new ThreadData[numberOfThreads];
	
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		threads[j].threadIsAlive = true;
		
		const int mutexInitStatus =
				pthread_mutex_init(&threads[j].dataAccess, NULL);
		if(mutexInitStatus > 0)
			throw mutexInitStatus;
		
		const int threadCreateStatus =
				pthread_create(&threads[j].thread, NULL,
						&threadMain, &threads[j]);
		if(threadCreateStatus > 0)
			throw createStatus;
	}
}

ThreadPool::~ThreadPool()
{
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		lockMutex(threads[j].dataAccess);
		threads[j].threadIsAlive = false;
		unlockMutex(threads[j].dataAccess);
	}
	
	for(unsigned int j = 0; j < numberOfThreads; j++)
	{
		const int joinStatus = pthread_join(threads[j].thread, NULL);
		if(joinStatus > 0)
			throw joinStatus;
	}
	
	delete [] threads;
}

//===============================================

void* threadMain(void* dataStructPtr)
{
	ControlStruct *data =
			reinterpret_cast<ThreadData*>(dataStructPtr);
	
	while(isThreadAlive(data))
		sleep(40);
	
	return NULL;
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

bool isThreadAlive(ThreadPool::ThreadData* threadData)
{
	lockMutex(threadData->dataAccess);
	bool isAlive = threadData->threadIsAlive;
	unlockMutex(threadData->dataAccess);
	
	return isAlive;
}

//===============================================

#ifdef UNITTEST
int main()
{
	ThreadPool threadPool(3);
	
	return 0;
}
#endif
