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
	it eventually returns true.
	
	"function" is expected to return true when it
	has succeeded and the find operation can
	stop.  The first parameter will be the actual
	value found by the function, which will be
	returned by findResult().  The second
	parameter will be set to the value of
	"functionArg".  The last parameter will start
	at 0 and increment by one for each call made
	to it by a thread, (note that this means that
	there is no guarantee of ordering).
	
	"blockSize" is the number of calls to
	"function" to assign at a time to a thread.*/
	std::string findResult(bool (*function)(
					std::string&, const std::string,
					const unsigned long),
			const std::string functionArg,
			unsigned long blockSize = 300);
	
	/*Used internally.  Only declared in the
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
