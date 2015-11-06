#!/bin/sh

echo "Running unit tests..."
echo ""
make unittestSHA512.exe
./unittestSHA512.exe
make unittestThreadPool.exe
./unittestThreadPool.exe
make unittestReverseDigest.exe
./unittestReverseDigest.exe

echo ""

make hashReversal.exe
./hashReversal.exe wed 2
