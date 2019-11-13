/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#include "Camera.h"
#include "mjpg_streamer.h"
#include "LinuxDARwIn.h"

#define INI_FILE_PATH       "config.ini"

#define U2D_DEV_NAME        "/dev/ttyUSB0"

void change_current_dir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        chdir(dirname(exepath));
}

int main(void)
{
    printf( "\n===== Head tracking Tutorial for DARwIn =====\n\n");

    change_current_dir();
    minIni* ini = new minIni(INI_FILE_PATH);

	//////////////////// Framework Initialize ////////////////////////////
	LinuxCM730 linux_cm730(U2D_DEV_NAME);
	CM730 cm730(&linux_cm730);
	if(MotionManager::GetInstance()->Initialize(&cm730) == false)
	{
		printf("Fail to initialize Motion Manager!\n");
			return 0;
	}
    MotionManager::GetInstance()->LoadINISettings(ini);
	MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());	
    LinuxMotionTimer *motion_timer = new LinuxMotionTimer(MotionManager::GetInstance());
    motion_timer->Start();

	MotionStatus::m_CurrentJoints.SetEnableBodyWithoutHead(false);
	MotionStatus::m_CurrentJoints.SetEnableHeadOnly(true);
	MotionManager::GetInstance()->SetEnable(true);


    sleep(1);
	MotionManager::GetInstance()->SetEnable(false);
    cm730.WriteWord(JointData::ID_HEAD_PAN, MX28::P_GOAL_POSITION_L, MX28::Angle2Value(0), 0);
    cm730.WriteWord(JointData::ID_HEAD_TILT, MX28::P_GOAL_POSITION_L, MX28::Angle2Value(-20), 0);
    sleep(1);
	MotionManager::GetInstance()->SetEnable(true);

    while(1)
    {
        int headTilt, headPan, error;
        cm730.ReadWord(JointData::ID_HEAD_PAN, MX28::P_PRESENT_POSITION_L, &headPan, &error);
        cm730.ReadWord(JointData::ID_HEAD_TILT, MX28::P_PRESENT_POSITION_L, &headTilt, &error);

        printf( "Head Pan is %f\n", MX28::Value2Angle(headPan) );
        printf( "Head Tilt is %f\n\n", MX28::Value2Angle(headTilt) );
        sleep( 1 );
    }

    return 0;
}

