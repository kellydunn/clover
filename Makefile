all:
	clang -I/usr/lib/clang/2.8/include -I/usr/include/ src/warlock.cpp -o bin/warlock -lsndfile -laubio -ljack