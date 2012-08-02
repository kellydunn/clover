all:
	clang -I/usr/lib/clang/2.8/include -I/usr/include/ -I/usr/lib/ src/warlock.cpp -o bin/warlock -ljack -lgstreamer-0.10 -lfftw3