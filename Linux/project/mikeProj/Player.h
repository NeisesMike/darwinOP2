/*
* Michael Neises
* 1 november 2019
* PLAYER game header
*/

#ifndef PLAYER_H
#define PLAYER_H

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
#include "Body.h"
#include "Eyes.h"
#include "/usr/include/festival/festival.h"

#ifdef MX28_1024
#define MOTION_FILE_PATH    "../../../Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "../../../Data/motion_4096.bin"
#endif

#define INI_FILE_PATH       "config.ini"
#define SCRIPT_FILE_PATH    "script.asc"

#define U2D_DEV_NAME0       "/dev/ttyUSB0"
#define U2D_DEV_NAME1       "/dev/ttyUSB1"

using namespace Robot;

class Player
{
    public:

        Player();
        ~Player();

        // simple mode for rapid prototyping
        void debug();
        bool m_debug;

        // learn one color
        int learnRed();
        int learnOrange();
        int learnYellow();
        int learnGreen();
        int learnBlue();
        int learnPurple();
        int learnBack();

        // learn, in sequence, the colors of the game.
        void learnColors();

        // learn the approx range of the size of a card
        void learnCardSize();

        Color getCardAtPosition( int pos );

        // given memory of the card last upturned,
        // make a choice
        // NOTE: while this makes Darwin more efficient
        // it means he cannot play drop in games
        int chooseCard();

        // verbally and/or somatically
        void indicateChoice( int pos );

        // update memory based on our pick
        // determine if we made a match
        bool analyzeResults(int ourCardPos);

        void updateMood(bool lastMatched, bool lastCardWasNew);

        // return true if the card upturned was new 
        bool waitForPartner();

        void say(const EST_String str);

        void makeBodyLanguage();

        void changeGemColor( Color col );
        void changeEyeColor( Color col );

        // say hello
        void greet();

        void interpretColor( int col );

        // do a look
        void observe();

        // scan the board
        void scan();

        // report on cards found
        void cardReport();
        
        // check button status
        void statusCheck();

    private:
        //LinuxCM730 linux_cm730;
        //CM730 cm730;
        Body body;
        //Eyes eyes;
        
        ScanData* kinestheticMemory;

        Color theirCard;
        int theirCardPos;
        Color boardMemory[8];
        bool boardMatchedMemory[8];
        //Mood dictionary
        // 0 pleasure
        // 1 valence
        // 2 arousal
        // 3 stance
        int mood[4];


        Color lastDetected;
};




#endif
