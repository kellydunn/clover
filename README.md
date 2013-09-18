```
      ___
     /\_ \
  ___\//\ \     ___   __  __     __   _ __
 /'___\\ \ \   / __`\/\ \/\ \  /'__`\/\`'__\
/\ \__/ \_\ \_/\ \L\ \ \ \_/ |/\  __/\ \ \/
\ \____\/\____\ \____/\ \___/ \ \____\\ \_\
 \/____/\/____/\/___/  \/__/   \/____/ \/_/

trippy~
```

## what

A realtime video manipulation tool using Gstreamer, fueled by FFT analysis of JACK audio data.

## installation

```
make
sudo make install   # assumes /usr/bin is in your $PATH
```

## usage
```
# start a jack server
clover /path/to/your/video
```
Then attach any audio playing on your computer to a jack server and watch your video go crazy.

## tests

This project currently uses libcheck to provide test coverage.  In order to run the tests, you may issue the following commands:

```
make
make test
```

## deps

- jack
- fftw3
- gstreamer-1.0
- gstreamer-plugins-base-1.0
- gstreamer-plugins-bad-1.0
- gst-plugins-good
- gst-ffmpeg

If you're using video accelerated apis (vaapi), you might need to download the appropriate drivers for your video card as well as any additional codecs.  On my Arch System I needed to install the following:

- gstreamer1.0-vaapi
- libva-intel-driver

## roadmap

- specify your own video effects through some sort of configuration file