#include <cassert>
#include <iostream>
#include <string>
#include "zedwood/sha512.h"

using namespace std;

int main()
{
	/*
	string input  = "grape";
	string output = sha512(input);
	
	cout << "sha512('"<< input << "'):" << output << endl;
	return 0;
	*/
	
	assert(sha512("grape") == "9375d1abdb644a01955bccad12e2f5c2bd8a3e226187e548d99c559a99461453b980123746753d07c169c22a5d9cc75cb158f0e8d8c0e713559775b5e1391fc4");
	
	assert(sha512("Tornado") == sha512("Tornado"));
	assert(sha512("Tornado") != sha512("tornado"));
	
	cout << "sha512(string) unit test passed." << endl;
	return 0;
}
