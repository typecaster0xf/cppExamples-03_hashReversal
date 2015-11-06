#pragma once

#include <pthread.h>
#include <vector>

class ThreadPool
{
public:
	
	ThreadPool(const unsigned int numberOfThreads);
	~ThreadPool();
	
	template<R>
	R process(bool(*function)(&R, const unsigned long));
	
protected:
	
	struct ThreadData
	{
		pthread_t thread;
		bool      threadIsAlive;
	};
	
	std::vector<ThreadData> threads;
};
