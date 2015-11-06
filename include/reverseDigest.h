#pragma once

#include <string>

/*Uses a ThreadPool to brute-force the reversal
of the provided digest.*/
std::string reverseDigest(const std::string sha256Checksum,
		const unsigned int threads);
