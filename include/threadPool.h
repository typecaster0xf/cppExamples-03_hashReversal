#pragma once

#include <string>
#include <pthread.h>
#include <queue>

/*This thread pool is specially designed for
running a function that takes an integer and if
it succeeds, sets a string value; and running
that function many times until it succeeds.*/
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
		
		pthread_mutex_t           queueMutex;
		std::queue<ThreadCommand> commandQueue;
		
		pthread_mutex_t returnMutex;
		bool            hasReturnData;
		std::string     returnString;
	};
	
protected:
	
	const unsigned int numberOfThreads;
	ThreadData         *threads;
};
