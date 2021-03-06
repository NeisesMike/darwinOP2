/*
 * Michael Neises
 * 12 november 2019
 * BODY header
 */

#ifndef BODY_H
#define BODY_H

#include <stdlib.h>
#include "LinuxDARwIn.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <unistd.h>

#include "StatusCheck.h"
#include "knowledge.h"
#include "Eyes.h"
#include <math.h>

#ifdef MX28_1024
#define MOTION_FILE_PATH    "../../../Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "../../../Data/motion_4096.bin"
#endif

#define SCRIPT_FILE_PATH    "script.asc"

#define U2D_DEV_NAME0       "/dev/ttyUSB0"
#define U2D_DEV_NAME1       "/dev/ttyUSB1"

using namespace Robot;

class Body
{
    public:

        bool m_debug;

        void makeBodyLanguage();

        void changeGemColor( Color col );
        void changeEyeColor( Color col );

        // do a look
        ScanData* scan();

        void statusCheck();

        /*
           * from neutral
           * pan left and right. face forward at 0 degrees
           * tilt up and down. face forward at 0 degrees
           * left is negative
           * down is negative
           */
        void moveHead(int pan, int tilt);
        int readHeadPan();
        int readHeadTilt();

        /*
           * from neutral
           * pitch swings forward. straight out at 0 degrees
           * roll swings out. straight out at 0 degrees
           * both grow positively down
           * such that neutral is 90, 90
           */
        void moveShoulder(bool isRight, int pitchAngle, int rollAngle);
        int readShoulderPitch(bool isRight);
        int readShoulderRoll(bool isRight);

        // do some trig
        void moveShoulderByGaze( int pan, int tilt );

        ScanData centerGaze( ScanData card );

        // if final, judge redundancy only by pan tilt
        void scanFilter( ScanData* input, ScanData* output, int percent, bool isFinal );
        void reorient( ScanData* input, ScanData* output, int percent );
        // numPossible is the number of possible card matches, redundant or otherwise
        void reorientFilter( ScanData* input, ScanData* output, int percent, int numPossible, bool isFinal );

        Eyes eyes;
        Body();

    private:
        LinuxCM730 linux_cm730;
        CM730 cm730;
        BallTracker tracker;
};

#endif

