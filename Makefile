CC=clang

# TODO pkg-config isn't on all systems, make some way of checking for this stuff

LDFLAGS=$(shell pkg-config --libs jack fftw3 gstreamer-0.10 gstreamer-plugins-base-0.10 gtk+-3.0)
CFLAGS=$(shell pkg-config --cflags jack fftw3 gstreamer-0.10 gstreamer-plugins-base-0.10 gtk+-3.0)
TEST_CFLAGS=$(CFLAGS) -lcheck
TARGET_OBJS=bin/build/clover/*.o
TEST_TARGET_OBJS=bin/build/clover/*.o bin/build/*.o

# TODO This is a rather naiive way to approach building the project
#      There should be someway to collect all buildable files and link them accordingly

all:
	mkdir -p bin/build/clover
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/jack-client.cpp -o bin/build/clover/jack-client.o
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/visualizer.cpp -o bin/build/clover/visualizer.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(TARGET_OBJS) src/clover.cpp  -o bin/clover

test:
	mkdir -p bin/test

clean:
	rm -rf bin/*

install:
	sudo cp bin/clover /usr/bin/clover
