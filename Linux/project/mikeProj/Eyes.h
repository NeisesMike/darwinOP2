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

        Eyes();
        ~Eyes();

        bool tryHit( Color col );

        // learn, in sequence, the colors of the game.
        void learnColors();

        // give a hue integer, reset the related color_finder
        void learnRed(int hue, bool isLearning);
        void learnOrange(int hue, bool isLearning);
        void learnYellow(int hue, bool isLearning);
        void learnGreen(int hue, bool isLearning);
        void learnBlue(int hue, bool isLearning);
        void learnPurple(int hue, bool isLearning);

        // do a look
        // return the detected color
        int look();

        // like look but only includes the central 10% of the image
        int maculaLook(double percent);
        // only includes the designated 10% of the image
        int maculaLook(int row, int column, double percent);

    private:
        ColorFinder* red_finder;
        ColorFinder* orange_finder;
        ColorFinder* yellow_finder;
        ColorFinder* green_finder; 
        ColorFinder* blue_finder;
        ColorFinder* purple_finder;
        mjpg_streamer* streamer;
};

#endif
