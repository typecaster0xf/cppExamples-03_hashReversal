#pragma once

#include <pthread.h>
#include <queue>

using std::queue;

class ThreadPool
{
public:
	
	ThreadPool(const unsigned int numberOfThreads);
	~ThreadPool();
	
	//template<typename R>
	//R process(bool(*function)(&R, const unsigned long));
	
	struct ThreadData
	{
		pthread_t       thread;
		pthread_mutex_t queueMutex;
		
		enum ThreadCommandType
		{
			TERMINATE
		};
		
		struct ThreadCommand
		{
			ThreadCommandType commandType;
		};
		
		queue<ThreadCommand> commandQueue;
	};
	
protected:
	
	const unsigned int numberOfThreads;
	ThreadData         *threads;
};
