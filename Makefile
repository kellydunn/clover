CC=clang

# TODO pkg-config isn't on all systems, make some way of checking for this stuff

LDFLAGS=$(shell pkg-config --libs jack fftw3 gstreamer-1.0 gstreamer-plugins-base-1.0 gstreamer-plugins-bad-1.0 gtk+-3.0) -lstdc++ -DGST_DISABLE_DEPRECATED
CFLAGS=$(shell pkg-config --cflags jack fftw3 gstreamer-1.0 gstreamer-plugins-base-1.0 gstreamer-plugins-bad-1.0 gtk+-3.0) -lstdc++ -DGST_DISABLE_DEPRECATED
TEST_CFLAGS=$(CFLAGS) -lcheck
TARGET_OBJS=bin/build/clover/*.o
TEST_TARGET_OBJS=bin/build/clover/*.o 

# TODO This is a rather naiive way to approach building the project
#      There should be someway to collect all buildable files and link them accordingly

all:
	mkdir -p bin/build/clover
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/audio-sampler.cpp -o bin/build/clover/audio-sampler.o
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/spectrum-analyzer.cpp -o bin/build/clover/spectrum-analyzer.o
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/visualizer.cpp -o bin/build/clover/visualizer.o
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/clover.cpp -o bin/build/clover/clover.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(TARGET_OBJS) src/main.cpp  -o bin/clover

test:
	mkdir -p bin/tests
	$(CC) tests/clover_test.cpp $(LDFLAGS) $(TEST_CFLAGS) $(TEST_TARGET_OBJS) -o bin/tests/clover_test
	./bin/tests/clover_test
clean:
	rm -rf bin/*

install:
	sudo cp bin/clover /usr/bin/clover
