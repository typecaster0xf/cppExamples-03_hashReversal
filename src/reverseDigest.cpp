#include <cassert>
#include "reverseDigest.h"
#include "threadPool.h"
#include "zedwood/sha512.h"

using namespace std;

bool checkADigest(string& testedString,
		const string digest,
		const unsigned long iterationNumber);

string convertToTestString(const unsigned long iterationNumber);

//===============================================

string reverseDigest(const string sha512Checksum,
		const unsigned int threads)
{
	assert(sha512Checksum.size() == 128);
	
	ThreadPool threadPool(threads);
	
	return threadPool.findResult(&checkADigest,
			sha512Checksum);
}

//===============================================

bool checkADigest(string& testedString,
		const string digest,
		const unsigned long iterationNumber)
{
	return true;
	//TODO
}

string convertToTestString(const unsigned long iterationNumber)
{
	return "return";//TODO
}

//===============================================

#ifdef UNITTEST
#include <iostream>

int main()
{
	const string input = "R4";
	
	assert(reverseDigest(sha512(input), 4) == input);
	
	cout << "reverseDigest(string) unit test passed." << endl;
	return 0;
}
#endif
