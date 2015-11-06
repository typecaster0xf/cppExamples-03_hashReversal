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
	
	/*Returns the string set by "function" when
	it eventually returns true.*/
	std::string findResult(bool (*function)(
					std::string&, const unsigned long),
			unsigned long blockSize = 300);
	
	/*Used internally.  Only declaired in the
	public section so that it is visible to the
	worker threads, (which for some reason can't
	seem to access friend functions).*/
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
		
		/*The worker threads receive instructions
		through this queue.*/
		pthread_mutex_t           queueMutex;
		std::queue<ThreadCommand> commandQueue;
		
		/*Data is returned using these
		variables.*/
		pthread_mutex_t returnMutex;
		bool            hasReturnData;
		std::string     returnString;
	};
	
protected:
	
	const unsigned int numberOfThreads;
	ThreadData         *threads;
};
