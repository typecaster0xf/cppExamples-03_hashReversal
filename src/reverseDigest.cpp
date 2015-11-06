#include <cassert>
#include "reverseDigest.h"
#include "threadPool.h"
#include "zedwood/sha512.h"

using namespace std;

bool checkADigest(string& testedString,
		const string digest,
		const unsigned long iterationNumber);

string convertToTestString(unsigned long iterationNumber);

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
	const string test = convertToTestString(iterationNumber);
	
	if(sha512(test) == digest)
	{
		testedString = test;
		return true;
	}else
		return false;
}

string convertToTestString(unsigned long iterationNumber)
{
	string output;
	output.reserve(10);
	
	while(iterationNumber > 0)
	{
		output += static_cast<char>(iterationNumber & 0x7f);
		iterationNumber >>= 7;
	}
	
	return output;
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
