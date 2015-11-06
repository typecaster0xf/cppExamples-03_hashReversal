#pragma once

#include <string>
#include <pthread.h>
#include <queue>

using std::queue;

class ThreadPool
{
public:
	
	ThreadPool(const unsigned int numberOfThreads);
	~ThreadPool();
	
	std::string findResult(bool (*function)(
			std::string&, const unsigned long));
	
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
