CC=g++

.SECONDEXPANSION:
OBJECT=$(CC) $< -c -std=c++11 -I include -o $@

OPTIMIZED=$(CC) $< -c -std=c++11 -I include -O2 \
-march=native -D NDEBUG -o $@

UNIT_TEST=$(CC) $< -c -std=c++11 -I include -D UNITTEST -o $@

LINK=$(CC) $^ -lpthread -o $@

#################################################

unittestReverseDigest.exe: \
		bin/reverseDigest__ut.o \
		bin/threadPool.o \
		bin/zedwood-sha512.o
	$(LINK)

unittestSHA512.exe: \
		bin/zedwood-unittestMain.o \
		bin/zedwood-sha512.o
	$(LINK)

unittestThreadPool.exe: \
		bin/threadPool__ut.o
	$(LINK)

#################################################

bin/reverseDigest.o: \
		src/reverseDigest.cpp \
		include/reverseDigest.h \
		include/threadPool.h \
		include/zedwood/sha512.h | bin
	$(OBJECT)
bin/reverseDigest__opt.o: \
		src/reverseDigest.cpp \
		include/reverseDigest.h \
		include/threadPool.h \
		include/zedwood/sha512.h | bin
	$(OPTIMIZED)
bin/reverseDigest__ut.o: \
		src/reverseDigest.cpp \
		include/reverseDigest.h \
		include/threadPool.h \
		include/zedwood/sha512.h | bin
	$(UNIT_TEST)

bin/threadPool.o: \
		src/threadPool.cpp \
		include/threadPool.h | bin
	$(OBJECT)
bin/threadPool__opt.o: \
		src/threadPool.cpp \
		include/threadPool.h | bin
	$(OPTIMIZED)
bin/threadPool__ut.o: \
		src/threadPool.cpp \
		include/threadPool.h | bin
	$(UNIT_TEST)

bin/zedwood-sha512.o: \
		zedwood/sha512.cpp \
		include/zedwood/sha512.h | bin
	$(OBJECT)
bin/zedwood-sha512__opt.o: \
		zedwood/sha512.cpp \
		include/zedwood/sha512.h | bin
	$(OPTIMIZED)

bin/zedwood-unittestMain.o: \
		zedwood/unittestMain.cpp \
		include/zedwood/sha512.h | bin
	$(OBJECT)

#################################################

bin:
	mkdir bin

.PHONY: clean
clean:
	rm -rf bin
	rm *.exe
