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

#include "mjpg_streamer.h"

#include "StatusCheck.h"

using namespace Robot;

enum Color
{
    UNKNOWN = 1,
    RED = 2,
    ORANGE = 4,
    YELLOW = 8,
    GREEN = 16,
    BLUE = 32,
    PURPLE = 64
};

class Player
{
    public:

        // learn, in sequence, the colors of the game.
        void learnColors();

        // associate each card with a location ( a 2Dpos a la gaze )
        // allow for some error plox
        void calibrateVision();

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

        void makeUtterance();

        void makeBodyLanguage();

        void changeGemColor( CM730* body, Color col );

        Player();

    private:
        double kinestheticMemory[8];
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
};




#endif
