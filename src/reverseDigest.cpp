#include <cassert>
#include "reverseDigest.h"
#include "threadPool.h"
#include "zedwood/sha512.h"

using namespace std;

string reverseDigest(const string sha512Checksum,
		const unsigned int threads)
{
	assert(sha512Checksum.size() == 128);
	
	ThreadPool threadPool(threads);
	
	//return threadPool.findResult(//TODO
	
	return "return";
}

//===============================================

#ifdef UNITTEST
#include <iostream>

int main()
{
	const string input = "R4";
	
	assert(reverseDigest(sha512(input)) == input);
	
	cout << "reverseDigest(string) unit test passed." << endl;
	return 0;
}
#endif
