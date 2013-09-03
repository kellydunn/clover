#ifndef _CLOVER_H
#define _CLOVER_H

#include <stdio.h>
#include "clover/visualizer.h"

GstBus * bus;
GstElement *sink_bin;
jack_port_t * input_port_l;
jack_port_t * input_port_r;

class Clover {
  public:

    Clover();
    void set_visualizer_source(char *);
    void play();

  private:
    Visualizer * visualizer;
    JackClient * jack_client;    
};

#endif
