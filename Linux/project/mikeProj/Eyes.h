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

struct ScanData
{
    int tilt;
    int pan;
    Point2D location;
    Color color;
    Point2D maculaOrigin;
};

class Eyes
{
    public:

        Eyes();
        ~Eyes();
        bool m_debug;

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
        ScanData look();

        // like look but only includes the central 10% of the image
        ScanData maculaLook(double percent);
        // always take a new picture
        ScanData maculaLook(int row, int column, double percent);
        // only includes the designated percent% of each dimension
        ScanData maculaLook(int row, int column, double percent, bool needTakePicture);

        // partition and analyze the image
        // return a list of midpoints
        void partitionScan(double percent, int tilt, int pan, ScanData* retList);

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
