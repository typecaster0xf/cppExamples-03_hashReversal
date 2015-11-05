#include <iostream>
#include <string>
#include "zedwood/sha512.h"

using namespace std;

int main()
{
	string input  = "grape";
	string output = sha512(input);
	
	cout << "sha512('"<< input << "'):" << output1 << endl;
	return 0;
}
