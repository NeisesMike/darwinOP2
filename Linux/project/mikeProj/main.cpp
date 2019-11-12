/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include "/usr/include/festival/festival.h"
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

    // init the voice
    int heap_size = 310000;  // default scheme heap size
    int load_init_files = 1; // we want the festival init files loaded

    festival_initialize(load_init_files,heap_size);

    // if we don't wait for the spooler, an error will be thrown
    // festival will crash, and anything we want to say
    // will be lost if it's queued after the current output
    festival_wait_for_spooler();

    // Say simple file
    //festival_say_file("/etc/motd");

    //festival_eval_command("(voice_ked_diphone)");
    // Say some text;
    festival_say_text("hello world");

    sleep(15);

    // MIKE NOTE
    // START OF PROGRAM (END OF INITS)

    //LinuxActionScript::PlayMP3("../../../Data/mp3/Demonstration ready mode.mp3");
    //Action::GetInstance()->Start(15);

    Color lastDetected = UNKNOWN;

    /*
    while(1)
    {
        StatusCheck::Check(cm730);

        Point2D red_pos, orange_pos, yellow_pos, green_pos, blue_pos, purple_pos;

        LinuxCamera::GetInstance()->CaptureFrame();

        // MIKE NOTE
        // CONDITIONAL ANALYSES


        memcpy(rgb_output->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);

        if(StatusCheck::m_cur_mode == NO_MOOD || StatusCheck::m_cur_mode == YES_MOOD)
        {
            red_pos = red_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            orange_pos = orange_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            yellow_pos = yellow_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            green_pos = green_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            blue_pos = blue_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            purple_pos = purple_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);

            unsigned char r, g, b;
            for(int i = 0; i < rgb_output->m_NumberOfPixels; i++)
            {
                r = 0; g = 0; b = 0;
                if(red_finder->m_result->m_ImageData[i] == 1)
                {
                    r = 255;
                    g = 0;
                    b = 0;
                }
                if(orange_finder->m_result->m_ImageData[i] == 1)
                {
                    r = 255;
                    g = 128;
                    b = 0;
                }
                if(yellow_finder->m_result->m_ImageData[i] == 1)
                {
                    r = 255;
                    g = 255;
                    b = 0;
                }
                if(green_finder->m_result->m_ImageData[i] == 1)
                {
                    r = 0;
                    g = 255;
                    b = 0;
                }
                if(blue_finder->m_result->m_ImageData[i] == 1)
                {
                    r = 0;
                    g = 0;
                    b = 255;
                }
                if(purple_finder->m_result->m_ImageData[i] == 1)
                {
                    r = 255;
                    g = 0;
                    b = 255;
                }

                if(r > 0 || g > 0 || b > 0)
                {
                    rgb_output->m_ImageData[i * rgb_output->m_PixelSize + 0] = r;
                    rgb_output->m_ImageData[i * rgb_output->m_PixelSize + 1] = g;
                    rgb_output->m_ImageData[i * rgb_output->m_PixelSize + 2] = b;
                }
            }
        }
        streamer->send_image(rgb_output);

        if(StatusCheck::m_cur_mode == NO_MOOD)
        {
            //tracker.Process(blue_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame));
        }

//        if(StatusCheck::m_is_started == 0)
//        {
//            continue;
//        }

        // MIKE NOTE
        // CONDITIONAL ACTIONS

        int detected_color = 0;
        switch(StatusCheck::m_cur_mode)
        {
            case NO_MOOD:
                // subtly alert to mood status
                // TODO

                detected_color |= (red_pos.X == -1)? 0 : RED;
                detected_color |= (orange_pos.X == -1)? 0 : ORANGE;
                detected_color |= (yellow_pos.X == -1)? 0 : YELLOW;
                detected_color |= (green_pos.X == -1)? 0 : GREEN;
                detected_color |= (blue_pos.X == -1)? 0 : BLUE;
                detected_color |= (purple_pos.X == -1)? 0 : PURPLE;

                break;
            case YES_MOOD:
                break;
            default:
                break;
        }

           //if(Action::GetInstance()->IsRunning() == 0)
           //VisionMode::Play(detected_color);

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



