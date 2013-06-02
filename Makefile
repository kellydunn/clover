CC=clang
LDFLAGS=$(shell pkg-config --libs jack fftw3 gstreamer-0.10 gstreamer-plugins-base-0.10)
CFLAGS=$(shell pkg-config --cflags jack fftw3 gstreamer-0.10 gstreamer-plugins-base-0.10)

all:
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/jack-client.cpp -o bin/jack-client.o
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover/gstreamer-client.cpp -o bin/gstreamer-client.o
	$(CC) $(LDFLAGS) $(CFLAGS) src/clover.cpp bin/*.o -o bin/clover
clean:
	rm -rf bin/*
install:
	sudo cp bin/clover /usr/bin/clover
