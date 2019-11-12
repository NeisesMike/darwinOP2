/*
 * Michael Neises
 * 1 nov 2019
 * player
 */

#include "Body.h"

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

Body::Body() : linux_cm730(LinuxCM730("/dev/ttyUSB0")) , cm730(CM730(&linux_cm730))
{
    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGQUIT, &sighandler);
    signal(SIGINT, &sighandler);

    change_current_dir();

    minIni* ini = new minIni(INI_FILE_PATH);

    //////////////////// Framework Initialize ////////////////////////////
    if(MotionManager::GetInstance()->Initialize(&cm730) == false)
    {
        linux_cm730.SetPortName(U2D_DEV_NAME1);
        if(MotionManager::GetInstance()->Initialize(&cm730) == false)
        {
            printf("Fail to initialize Motion Manager!\n");
            exit(0);
        }
    }

    MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());

    LinuxMotionTimer *motion_timer = new LinuxMotionTimer(MotionManager::GetInstance());
    motion_timer->Start();
    /////////////////////////////////////////////////////////////////////

    MotionManager::GetInstance()->LoadINISettings(ini);

    int firm_ver = 0;
    if(cm730.ReadByte(JointData::ID_HEAD_PAN, MX28::P_VERSION, &firm_ver, 0)  != CM730::SUCCESS)
    {
        fprintf(stderr, "Can't read firmware version from Dynamixel ID %d!! \n\n", JointData::ID_HEAD_PAN);
        exit(0);
    }

    if(0 < firm_ver && firm_ver < 27)
    {
#ifdef MX28_1024
        Action::GetInstance()->LoadFile(MOTION_FILE_PATH);
#else
        fprintf(stderr, "MX-28's firmware is not support 4096 resolution!! \n");
        fprintf(stderr, "Upgrade MX-28's firmware to version 27(0x1B) or higher.\n\n");
        exit(0);
#endif
    }
    else if(27 <= firm_ver)
    {
#ifdef MX28_1024
        fprintf(stderr, "MX-28's firmware is not support 1024 resolution!! \n");
        fprintf(stderr, "Remove '#define MX28_1024' from 'MX28.h' file and rebuild.\n\n");
        exit(0);
#else
        Action::GetInstance()->LoadFile((char*)MOTION_FILE_PATH);
#endif
    }
    else
        exit(0);

    Action::GetInstance()->m_Joint.SetEnableBody(true, true);
    MotionManager::GetInstance()->SetEnable(true);

    cm730.WriteByte(CM730::P_LED_PANNEL, 0x01|0x02|0x04, NULL);
}

// TODO
void Body::makeBodyLanguage()
{
    return;
}

void Body::changeGemColor( Color col )
{
    switch( col )
    {
        case RED:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_HEAD_L, cm730.MakeColor(255, 0, 0), 0);
            break;
        case GREEN:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_HEAD_L, cm730.MakeColor(0, 255, 0), 0);
            break;
        case BLUE:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_HEAD_L, cm730.MakeColor(0, 0, 255), 0);
            break;
        case ORANGE:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_HEAD_L, cm730.MakeColor(225, 64, 0), 0);
            break;
        case YELLOW:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_HEAD_L, cm730.MakeColor(128, 225, 0), 0);
            break;
        case PURPLE:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_HEAD_L, cm730.MakeColor(225, 0, 225), 0);
            break;
    }
    return;
}

void Body::statusCheck()
{
    StatusCheck::Check(cm730);
    return;
}
