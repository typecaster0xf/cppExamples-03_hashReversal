CC=g++

.SECONDEXPANSION:
OBJECT=$(CC) $< -c -I include -o $@
OPTIMIZED=$(CC) $< -c -I include -O2 -march=native -D NDEBUG -o $@
UNIT_TEST=$(CC) $< -c -I include -D UNITTEST -o $@
LINK=$(CC) $^ -lpthread -o $@

#################################################

unittestSHA512.exe: \
		bin/zedwood-unittestMain.o \
		bin/zedwood-sha512.o
	$(LINK)

unittestThreadPool.exe: \
		bin/threadPool__ut.o
	$(LINK)

#################################################

bin/threadPool.o: \
		src/threadPool.cpp \
		include/threadPool.h
	$(OBJECT)
bin/threadPool__opt.o: \
		src/threadPool.cpp \
		include/threadPool.h
	$(OPTIMIZED)
bin/threadPool__ut.o: \
		src/threadPool.cpp \
		include/threadPool.h
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
