#include <iostream>
#include "reverseDigest.h"
#include "zedwood/sha512.h"

using namespace std;

int main(const int argc, const char* argv[])
{
	cout << reverseDigest(sha512("rdd"), 6) << endl;
	return 0;
}
