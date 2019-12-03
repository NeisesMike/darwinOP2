/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: mike
 */

#include "Player.h"
#include <unistd.h>

#ifdef MX28_1024
#define MOTION_FILE_PATH    "../../../Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "../../../Data/motion_4096.bin"
#endif

#define INI_FILE_PATH       "config.ini"
#define SCRIPT_FILE_PATH    "script.asc"

#define U2D_DEV_NAME0       "/dev/ttyUSB0"
#define U2D_DEV_NAME1       "/dev/ttyUSB1"

int main(void)
{
    // init the player
    Player* DarCon = new Player();
    DarCon->debug();

    /*
    DarCon->changeGemColor( YELLOW );
    DarCon->changeEyeColor( YELLOW );
    DarCon->shoulderTests();
    */

    DarCon->greet();
    DarCon->changeGemColor( BLUE );
    DarCon->changeEyeColor( BLUE );
    DarCon->statusCheck();

    //DarCon->learnCardSize();

    DarCon->learnColors();
    DarCon->say( "Please set out some cards now." );
    DarCon->say( "I will wait 10 seconds." );
    sleep(10);

    /*
    DarCon->scan();
    DarCon->cardReport();
    */

    DarCon->scan();
    DarCon->nod();

    int unmatchedPairs = 3;

    while( unmatchedPairs > 0 )
    {
        DarCon->say( "your turn" );
        // wait for partner's turn
        sleep(3);
        int partnerCardIndex = DarCon->waitForPartner();
        ScanData partnerCard = DarCon->getCardAtMem( partnerCardIndex );

        DarCon->say( "my turn" );
        // take turn
        int choice = DarCon->chooseCard( partnerCardIndex );
        DarCon->indicateChoice( choice );
        ScanData ourCard = DarCon->getCardAtMem( choice );
        sleep(4);

        // see how we did
        if( partnerCard.color == ourCard.color )
        {
            // it's a match!
            DarCon->updateMem( partnerCardIndex, true );
            DarCon->updateMem( choice, true );
            
            DarCon->nod();
            unmatchedPairs--;
        }
        else
        {
            // gotta see what we turned over
            Color choiceColor = DarCon->getCardAtPosition( choice );
            DarCon->updateMem( choice, choiceColor );
            if( choiceColor == partnerCard.color )
            {
                DarCon->say( "lucky!" );
                DarCon->updateMem( partnerCardIndex, true );
                DarCon->updateMem( choice, true );
                unmatchedPairs--;
            }
            else
            {
                DarCon->say( "no match" );
            }
        }
    }

    DarCon->say( "Good game" );
    DarCon->nod();

    delete( DarCon );
    return(0);
}

