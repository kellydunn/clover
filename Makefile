SHELL=/bin/sh
all: clang -I/usr/lib/clang/2.8/include -I/usr/include/  -ljack -lpthread -lrt   -lfftw3 -lm  -pthread -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/gstreamer-0.10 -I/usr/include/libxml2  -pthread -L/usr/lib/i386-linux-gnu -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lrt -lglib-2.0   src/warlock.cpp -o bin/warlock
