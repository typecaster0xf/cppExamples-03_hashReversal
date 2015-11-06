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
					std::string&, const unsigned long),
			unsigned long blockSize = 300);
	
	struct ThreadData
	{
		pthread_t thread;
		
		enum ThreadCommandType
		{
			TERMINATE,
			RUN_FUNCTION_SEARCH
		};
		
		struct ThreadCommand
		{
			ThreadCommandType commandType;
			bool (*function)(std::string&, const unsigned long);
			unsigned long startValue;
			unsigned long blockSize;
		};
		
		pthread_mutex_t      queueMutex;
		queue<ThreadCommand> commandQueue;
		
		pthread_mutex_t returnMutex;
		bool            hasReturnData;
		std::string     returnString;
	};
	
protected:
	
	const unsigned int numberOfThreads;
	ThreadData         *threads;
};
