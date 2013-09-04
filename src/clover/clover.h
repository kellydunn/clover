#ifndef _CLOVER_H
#define _CLOVER_H

#include <stdio.h>
#include <jack/jack.h>
#include "visualizer.h"
#include "jack-client.h"

class Clover {
  public:

    Clover(int * argc, char ***argv);
    void set_visualizer_source(char *);
    void play();
    Visualizer * get_visualizer();
    JackClient * get_jack_client();

  private:
    Visualizer * visualizer;
    JackClient * jack_client;    
};

#endif
