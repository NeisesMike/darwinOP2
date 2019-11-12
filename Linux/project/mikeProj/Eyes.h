/*
* Michael Neises
* 12 november 2019
* eyes header
*/

#ifndef EYES_H
#define EYES_H

#include <stdlib.h>
#include "LinuxDARwIn.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <unistd.h>

#include "mjpg_streamer.h"
#include "knowledge.h"

#define INI_FILE_PATH       "config.ini"

using namespace Robot;

struct ColorMem
{
   int red;
   int orange;
   int yellow;
   int green;
   int blue;
   int purple;
};

class Eyes
{
    public:

        // learn, in sequence, the colors of the game.
        void learnColors();

        // do a look
        // return the detected color
        int look();

        Eyes();

    private:
        ColorFinder* red_finder;
        ColorFinder* orange_finder;
        ColorFinder* yellow_finder;
        ColorFinder* green_finder; 
        ColorFinder* blue_finder;
        ColorFinder* purple_finder;
        BallTracker* tracker;
        mjpg_streamer* streamer;
};

#endif