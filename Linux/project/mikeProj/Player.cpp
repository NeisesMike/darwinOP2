/*
 * Michael Neises
 * 1 nov 2019
 * player
 */
#include "Player.h"

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

Player::Player() : linux_cm730(LinuxCM730("/dev/ttyUSB0")) , cm730(CM730(&linux_cm730))
{
    for(int i=0; i<8; i++)
    {
        boardMemory[i] = UNKNOWN;
    }

    /*
    linux_cm730 = Robot.LINUXCM730("/dev/ttyUSB0");
    cm730 = ROBOT::CM730(&linux_cm730);
    */

    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGQUIT, &sighandler);
    signal(SIGINT, &sighandler);

    change_current_dir();

    minIni* ini = new minIni(INI_FILE_PATH);
    Image* rgb_output = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);

    LinuxCamera::GetInstance()->Initialize(0);
    LinuxCamera::GetInstance()->SetCameraSettings(CameraSettings());    // set default
    LinuxCamera::GetInstance()->LoadINISettings(ini);                   // load from ini

    mjpg_streamer* streamer = new mjpg_streamer(Camera::WIDTH, Camera::HEIGHT);

    // MIKE NOTE
    // Color Definitions - construction paper in daylight bulb light

    ColorFinder* red_finder = new ColorFinder(355, 10, 45, 0, 24, 50.0);
    red_finder->LoadINISettings(ini, "RED");
    httpd::red_finder = red_finder;

    ColorFinder* orange_finder = new ColorFinder(15, 10, 45, 0, 24, 50.0);
    orange_finder->LoadINISettings(ini, "ORANGE");
    httpd::orange_finder = orange_finder;

    ColorFinder* yellow_finder = new ColorFinder(45, 15, 45, 0, 24, 50.0);
    yellow_finder->LoadINISettings(ini, "YELLOW");
    httpd::yellow_finder = yellow_finder;

    ColorFinder* green_finder = new ColorFinder(117, 15, 25, 0, 24, 50.0);
    green_finder->LoadINISettings(ini, "BLUE");
    httpd::green_finder = green_finder;

    ColorFinder* blue_finder = new ColorFinder(220, 15, 30, 30, 24, 50.0);
    blue_finder->LoadINISettings(ini, "BLUE");
    httpd::blue_finder = blue_finder;

    ColorFinder* purple_finder = new ColorFinder(280, 15, 20, 20, 24, 50.0);
    purple_finder->LoadINISettings(ini, "PURPLE");
    httpd::purple_finder = purple_finder;

    /* UNUSED COLORS

       ColorFinder* pink_finder = new ColorFinder(305, 20, 9, 0, 0.3, 50.0);
       pink_finder->LoadINISettings(ini, "PINK");
       httpd::pink_finder = pink_finder;

       ColorFinder* brown_finder = new ColorFinder(10, 20, 9, 0, 0.3, 50.0);
       brown_finder->LoadINISettings(ini, "BROWN");
       httpd::brown_finder = brown_finder;

       ColorFinder* white_finder = new ColorFinder(205, 10, 0, 70, 0.3, 50.0);
       white_finder->LoadINISettings(ini, "WHITE");
       httpd::white_finder = white_finder;

    // black needs to be at Exposure = 75
    ColorFinder* black_finder = new ColorFinder(215, 10, 30, 30, 0.3, 50.0);
    black_finder->LoadINISettings(ini, "BLACK");
    httpd::black_finder = black_finder;
    */

    BallTracker tracker = BallTracker();

    httpd::ini = ini;

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

    Action::GetInstance()->Start(1);
    sleep(3);
    Action::GetInstance()->Start(15);

}
/*
struct ColorMem
{
   int red;
   int orange;
   int yellow;
   int green;
   int blue;
   int purple;
};
*/

void Player::learnColors()
{
    return;
}

// TODO
void Player::calibrateVision()
{
    for( int i=0; i<8; i++ )
    {
        kinestheticMemory[i] = 0;
    }
    return; 
}

// TODO
Color Player::getCardAtPosition( int pos )
{
    // watch out for hands ( red ? )
    return( UNKNOWN );
}

int Player::chooseCard()
{
    // case: know a match
    int i;
    for( i=0; i<8; i++ )
    {
        if( boardMemory[i] == theirCard && i != theirCardPos )
        {
            return( i );
        }
    }

    // case: don't know a match
    while( true )
    {
        int choice = rand() % 8;
        if( boardMemory[choice] == UNKNOWN )
        {
            return( choice );
        }
    }
}

// TODO
void Player::indicateChoice( int pos )
{
    // verbally indicate pos + 1 for sanity

    // turn gaze towards the appropriate card
    double thisLocation = kinestheticMemory[pos];

    // maybe point towards the appropriate card?

    return;
}
bool Player::analyzeResults(int ourCardPos)
{
    // identify the color of the card we chose
    Color ourCard = getCardAtPosition( ourCardPos );

    // update memory
    boardMemory[ourCardPos] = ourCard;

    // compare it to the one our partner picked
    if( ourCard == theirCard && ourCard != UNKNOWN )
    {
        // update memory
        boardMatchedMemory[ourCardPos] = true;
        boardMatchedMemory[theirCardPos] = true;
        return( true );
    }
    // TODO
    // "rats!"
    return( false );
}

// TODO
void Player::updateMood(bool lastMatched, bool lastCardWasNew)
{
    if(lastMatched)
    {
        mood[0]++;
    }
    else
    {
        mood[0]--;
    }

    if(lastCardWasNew)
    {
        mood[2]++;
    }
    else
    {
        mood[2]--;
    }
}

bool Player::waitForPartner()
{
    // look at all cards in sequence that aren't already matched
    Color thisCard = UNKNOWN;
    int thisCardPos = 0;
    while( thisCard == UNKNOWN )
    {
        for( int i=0; i<8; i++ )
        {
            // skip it if we already know its been matched
            if( boardMatchedMemory[i] )
            {
                continue;
            }

            // otherwise, get the color
            // update memory
            theirCard = getCardAtPosition(i);
            theirCardPos = i;
        }
    }

    // check too see if we knew this card already
    if( boardMemory[thisCardPos] != thisCard )
    {
        if( boardMemory[thisCardPos] == UNKNOWN )
        {
            boardMemory[thisCardPos] = thisCard;
            return( true );
        }
        // TODO
        // "I must be confused. I thought another card was there!"
    }
    return( false );
}

// TODO
void Player::makeUtterance()
{
    return;
}

// TODO
void Player::makeBodyLanguage()
{
    return;
}

void Player::changeGemColor(CM730* body, Color col )
{
    switch( col )
    {
        case RED:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(255, 0, 0), 0);
            break;
        case GREEN:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(0, 255, 0), 0);
            break;
        case BLUE:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(0, 0, 255), 0);
            break;
        case ORANGE:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(225, 128, 0), 0);
            break;
        case YELLOW:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(128, 225, 0), 0);
            break;
        case PURPLE:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(225, 0, 225), 0);
            break;
    }
    return;
}

