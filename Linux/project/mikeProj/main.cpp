/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
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

/*
void change_current_dir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
    {
        if(chdir(dirname(exepath)))
            fprintf(stderr, "chdir error!! \n");
    }
}

void sighandler(int sig)
{
    exit(0);
}
*/

int main(void)
{
    // init the player
    Player* DarCon = new Player();

    DarCon->greet();
    DarCon->changeGemColor( BLUE );
    DarCon->statusCheck();
    DarCon->learnColors();

    while(1)
    {
        DarCon->scan();
    }



    /*
    while(1)
    {
        if(StatusCheck::m_cur_mode == NO_MOOD)
        {
            //tracker.Process(blue_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame));
        }

        if(StatusCheck::m_is_started == 0)
        {
            continue;
        }

        if(Action::GetInstance()->IsRunning() == 0)


        if((detected_color & RED) != 0 && lastDetected != RED)
        {
            lastDetected = RED;
            DarCon->changeGemColor( &cm730, RED );
            festival_say_text("red");
        }
        else if((detected_color & ORANGE) != 0 && lastDetected != ORANGE)
        {
            lastDetected = ORANGE;
            DarCon->changeGemColor( &cm730, ORANGE );
            festival_say_text("orange");
        }
        else if((detected_color & YELLOW) != 0 && lastDetected != YELLOW)
        {
            lastDetected = YELLOW;
            DarCon->changeGemColor( &cm730, YELLOW );
            festival_say_text("yellow");
        }
        else if((detected_color & GREEN) != 0 && lastDetected != GREEN)
        {
            lastDetected = GREEN;
            DarCon->changeGemColor( &cm730, GREEN );
            festival_say_text("green");
        }
        else if((detected_color & BLUE) != 0 && lastDetected != BLUE)
        {
            lastDetected = BLUE;
            DarCon->changeGemColor( &cm730, BLUE );
            festival_say_text("blue");
        }
        else if((detected_color & PURPLE) != 0 && lastDetected != PURPLE)
        {
            lastDetected = PURPLE;
            DarCon->changeGemColor( &cm730, PURPLE );
            festival_say_text("purple");
        }

    }
    */
}



