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
   int backside;
};

struct ScanData
{
    int tilt;
    int pan;
    Point2D location;
    Color color;
    Point2D maculaOrigin;
    int numPixels;

    struct ScanData& operator=(const ScanData& rhs)
    {
        tilt = rhs.tilt;
        pan = rhs.pan;
        location.X = rhs.location.X;
        location.Y = rhs.location.Y;
        color = rhs.color;
        maculaOrigin.X = rhs.maculaOrigin.X;
        maculaOrigin.Y = rhs.maculaOrigin.Y;
        numPixels = rhs.numPixels;
        return *this;
    }
};

class Eyes
{
    public:

        Eyes();
        ~Eyes();
        bool m_debug;

        // see if can identify
        bool tryHit( Color col );
        // more rigorous
        bool tryHitSize( Color col );

        void setMinCardSize(int size);
        void setMaxCardSize(int size);

        // learn, in sequence, the colors of the game.
        void learnColors();

        // give a hue integer, reset the related color_finder
        void learnRed(int hue, bool isLearning);
        void learnGreen(int hue, bool isLearning);
        void learnBlue(int hue, bool isLearning);
        void learnBack(int hue, bool isLearning);

        // give a min/max, reset the global card size range
        void learnCardSize(int hue, int min, int max);

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

        // grow macula until encompass whole card
        // return center point
        ScanData growMacula( ScanData card, int percent );

    private:
        ColorFinder* red_finder;
        ColorFinder* green_finder; 
        ColorFinder* blue_finder;
        ColorFinder* backside_finder;
        mjpg_streamer* streamer;

        double m_minCardSize;
        double m_maxCardSize;

        ColorFinder* orange_finder;
        ColorFinder* yellow_finder;
        ColorFinder* purple_finder;
};

#endif
