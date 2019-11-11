/*
 * StatusCheck.cpp
 *
 *  Created on: 2011. 1. 21.
 *      Author: zerom
 */

#include <stdio.h>
#include <unistd.h>

#include "StatusCheck.h"
#include "Head.h"
#include "Action.h"
#include "Walking.h"
#include "MotionStatus.h"
#include "MotionManager.h"
#include "LinuxActionScript.h"

using namespace Robot;

int StatusCheck::m_cur_mode     = NO_MOOD;
int StatusCheck::m_old_btn      = 0;
int StatusCheck::m_is_started   = 0;

void StatusCheck::Check(CM730 &cm730)
{
    if(m_old_btn == MotionStatus::BUTTON)
        return;

    m_old_btn = MotionStatus::BUTTON;

    if(m_old_btn & BTN_MODE)
    {
        fprintf(stderr, "Mode button pressed.. \n");

        if(m_is_started == 1)
        {
            m_is_started    = 0;
            m_cur_mode      = NO_MOOD;
            LinuxActionScript::m_stop = 1;

            Walking::GetInstance()->Stop();
            Action::GetInstance()->m_Joint.SetEnableBody(true, true);

            while(Action::GetInstance()->Start(15) == false) usleep(8000);
            while(Action::GetInstance()->IsRunning() == true) usleep(8000);
        }
        else
        {
            m_cur_mode++;
            if(m_cur_mode >= MAX_MODE) m_cur_mode = NO_MOOD;
        }

        MotionManager::GetInstance()->SetEnable(false);
        usleep(10000);

        // MIKE NOTE
        // there are 7 possible modes via the leds
        // check the other file, but I think you just OR the numbers together
        // like: 0x01 | 0x02
        // or: 0x01 | 0x02 | 0x04
        if(m_cur_mode == NO_MOOD)
        {
            cm730.WriteByte(CM730::P_LED_PANNEL, 0x01, NULL);
        }
        else if(m_cur_mode == YES_MOOD)
        {
            cm730.WriteByte(CM730::P_LED_PANNEL, 0x02, NULL);
        }
    }

    if(m_old_btn & BTN_START)
    {
        if(m_is_started == 0)
        {
            fprintf(stderr, "Start button pressed.. & started is false.. \n");

            MotionManager::GetInstance()->Reinitialize();
            MotionManager::GetInstance()->SetEnable(true);
            m_is_started = 1;
            LinuxActionScript::PlayMP3("../../../Data/mp3/Start soccer demonstration.mp3");

            Action::GetInstance()->m_Joint.SetEnableBody(true, true);

            Action::GetInstance()->Start(1);
            while(Action::GetInstance()->IsRunning() == true) usleep(8000);

            Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);

            MotionManager::GetInstance()->ResetGyroCalibration();
            while(1)
            {
                if(MotionManager::GetInstance()->GetCalibrationStatus() == 1)
                {
                    LinuxActionScript::PlayMP3("../../../Data/mp3/Sensor calibration complete.mp3");
                    break;
                }
                else if(MotionManager::GetInstance()->GetCalibrationStatus() == -1)
                {
                    LinuxActionScript::PlayMP3Wait("../../../Data/mp3/Sensor calibration fail.mp3");
                    MotionManager::GetInstance()->ResetGyroCalibration();
                }
                usleep(8000);
            }
            if(m_cur_mode == NO_MOOD)
            {
                // disable mood
            }
            else if(m_cur_mode == YES_MOOD)
            {
                // enable mood
            }
        }
        else
        {
            fprintf(stderr, "Start button pressed.. & started is true.. \n");
        }
    }
}
