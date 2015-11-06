#include <cstdlib>
#include <iostream>
#include <string>
#include "reverseDigest.h"
#include "zedwood/sha512.h"

using namespace std;

int main(const int argc, const char* argv[])
{
	if(argc < 2)
	{
		cout << "This program takes an argument string, hashes "
				<< "it, then reverses that hash to get the "
				<< "original string back.  Due to the brute-force "
				<< "algorithm used, it is best to only provided "
				<< "short strings, otherwise this program will run "
				<< "for quite some time.\n\n"
				<< "Usage: " << argv[0] << " <string_to_hash> "
				<< "<number_of_threads>" << endl;
		return 0;
	}
	
	const string stringToHash          = argv[1];
	const unsigned int numberOfThreads = atoi(argv[2]);
	
	//--
	
	cout << "Running SHA-512 digest reversal..." << endl;
	cout << reverseDigest(sha512(stringToHash), numberOfThreads) << endl;
	return 0;
}
