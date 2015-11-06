#include <unistd.h>
#include "threadPool.h"

using namespace std;

void* threadMain(void* dataStructPtr);

//===============================================

ThreadPool::ThreadPool(const unsigned int numberOfThreads)
{
	threads.resize(numberOfThreads);
	
	for(unsigned int j = 0; j < threads.size(); j++)
	{
		threads[j].threadIsAlive = true;
		//TODO spawn threads.
	}
}

//===============================================

void* threadMain(void* dataStructPtr)
{
	ControlStruct *data =
			reinterpret_cast<ThreadData*>(dataStructPtr);
	
	while(data->threadIsAlive)
		sleep(40);
	
	return NULL;
}
