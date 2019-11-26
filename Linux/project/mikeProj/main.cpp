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

//    DarCon->greet();
    DarCon->changeGemColor( BLUE );
    DarCon->statusCheck();

    //DarCon->debug();
//    DarCon->learnCardSize();
    DarCon->learnColors();

    /*
    DarCon->say( "Please set out some cards now." );
    DarCon->say( "I will wait five seconds." );
    sleep(5);

    DarCon->scan();
    DarCon->cardReport();
    printf( "done!\n" );
    */

    while(1)
    {
        DarCon->observe(); 
        //DarCon->scan();
    }

    delete( DarCon );
    return(0);
}

