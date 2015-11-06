#pragma once

#include <pthread.h>

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
		pthread_mutex_t dataAccess;
		
		bool threadIsAlive;
	};
	
protected:
	
	const unsigned int numberOfThreads;
	ThreadData         *threads;
};
