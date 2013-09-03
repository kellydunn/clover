#ifndef _CLOVER_H
#define _CLOVER_H

#include <stdio.h>

GstBus * bus;
GstElement *sink_bin;
jack_port_t * input_port_l;
jack_port_t * input_port_r;

class Clover {};

#endif
