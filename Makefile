CC=clang
LDFLAGS=$(shell pkg-config --libs jack fftw3 gstreamer-0.10 gstreamer-plugins-base-0.10)
CFLAGS=$(shell pkg-config --cflags jack fftw3 gstreamer-0.10 gstreamer-plugins-base-0.10)

# TODO choose a better build system than this cruft-tacular garbage
all:
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover-jack.cpp -o bin/clover-jack.o
	$(CC) -c $(LDFLAGS) $(CFLAGS) src/clover-gst.cpp -o bin/clover-gst.o
	$(CC) $(LDFLAGS) $(CFLAGS) src/clover.cpp bin/*.o -o bin/clover
clean:
	rm -rf bin/*
install:
	sudo cp bin/clover /usr/bin/clover
