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

## deps

- jack
- fftw3
- gstreamer-0.10
- gstreamer-plugins-base-0.10

## roadmap

- specify your own video effects through some sort of configuration file