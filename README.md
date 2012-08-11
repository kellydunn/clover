# clover

## what

A realtime video manipulation tool using Gstreamer, fueled by FFT analysis of JACK audio data.

## installation

```
./config
make
sudo make install   # assumes /usr/bin is in your $PATH
```

## usage
```
clover /path/to/your/video
```
Then attach any audio playing on your computer to a jack server and watch your video go crazy.

## deps

- jack
- fftw3
- gstreamer-0.10
- gstreamer-plugins-base-0.10

## TODOs

- autoconf
- Learn how to code in C :P
