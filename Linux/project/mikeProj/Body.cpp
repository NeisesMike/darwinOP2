/*
 * Michael Neises
 * 1 nov 2019
 * player
 */

#include "Body.h"
#include <time.h>

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

Body::Body() : linux_cm730(LinuxCM730("/dev/ttyUSB0")) , cm730(CM730(&linux_cm730)), eyes(Eyes()), tracker(BallTracker())
{
    m_debug = true;

    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGQUIT, &sighandler);
    signal(SIGINT, &sighandler);

    change_current_dir();

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

    //grab
	MotionStatus::m_CurrentJoints.SetEnableBodyWithoutHead(false);
	MotionManager::GetInstance()->SetEnable(true);
	/////////////////////////////////////////////////////////////////////

	Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
    Action::GetInstance()->m_Joint.SetEnableBody(true, true);


	Head::GetInstance()->m_Joint.SetPGain(JointData::ID_HEAD_PAN, 8);
	Head::GetInstance()->m_Joint.SetPGain(JointData::ID_HEAD_TILT, 8);
    //endgrab

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

void lookForOne(Eyes& eyes, int percent, int centerCol, int centerRow)
{
    time_t startTimer;
    time_t nowTimer;
    time(&startTimer);
    time(&nowTimer);
    int startRow = centerRow - (Camera::HEIGHT/(100/percent)/2);
    int startCol = centerCol - (Camera::WIDTH/(100/percent)/2);
    while( difftime(nowTimer, startTimer) < 1 )
    {
        eyes.maculaLook( startRow, startCol, percent );
        time(&nowTimer);
    }
    return;
}

ScanData recenterGaze(Eyes& eyes, int percent, int centerCol, int centerRow)
{
    int startRow = centerRow - (Camera::HEIGHT/(100/percent)/2);
    int startCol = centerCol - (Camera::WIDTH/(100/percent)/2);
    return( eyes.maculaLook(startRow, startCol, percent) );
}

void refreshEyes(Eyes& eyes)
{
    eyes.maculaLook(0,0,10);
    eyes.maculaLook(0,0,10);
    eyes.maculaLook(0,0,10);
    eyes.maculaLook(0,0,10);
    eyes.maculaLook(0,0,10);
    return;
}

ScanData* Body::scan()
{
    // ===============================================
    // from left to right
        // take a picture
        // scan it for cards
        // add potential cards to a list
    // ===============================================

    int percent = 20;
    int numHeadPositions = 3;

    // *2 because a macula can get a hit on the same position twice
    // happens when matches on head position overlap
    int numPossibleMatches = (100/percent)*2*numHeadPositions;

    ScanData stop = {};
    Point2D stopPoint = Point2D(-1000,-1000);
    stop.location = stopPoint;
    
    // ===============================================
    // grab all potential matches in the working area
    // ===============================================

    ScanData* retList = (ScanData*)malloc(sizeof(ScanData)*numPossibleMatches);
    retList[0] = stop;

    // Camera H angle is 58.0 degrees
    // so we try to partition 174 degrees of viewing here
    moveHead( -58, 0 );
    refreshEyes(eyes);
    eyes.partitionScan( percent, -58, 0, retList ); 
    moveHead( 0, -20 );
    refreshEyes(eyes);
    eyes.partitionScan( percent, 0, -20, retList ); 
    moveHead( 58, 0 );
    refreshEyes(eyes);
    eyes.partitionScan( percent, 58, 0, retList ); 

    // ===============================================
    // for each potential card,
        // center the gaze at that card
        // add its new center to a list
    // ===============================================

    ScanData* onceFilteredList = (ScanData*)malloc(sizeof(ScanData)*(100/percent)*4*numHeadPositions);
    int iter = 0;
    for( int i=0; i<numPossibleMatches; i++ )
    {
        ScanData temp = retList[i];
        if( temp.location.X == -1000 )
        {
            break;
        }
        if( temp.color == RED )
        {
            // center the gaze
            ScanData pantilt = centerGaze( temp );

            // grow the macula
            ScanData result = eyes.growMacula( temp, percent );
            result.pan = pantilt.pan;
            result.tilt = pantilt.tilt;

            onceFilteredList[iter] = result;
            iter++;
        }
    }
    onceFilteredList[iter] = stop;

    // ===============================================
    // for each newly centered card
        // if it is too close to another card
        // remove it from the list
    // ===============================================

    ScanData* twiceFilteredList = (ScanData*)malloc(sizeof(ScanData)*(100/percent)*4*numHeadPositions);
    twiceFilteredList[0] = stop;
    iter = 0;
    for( int i=0; i<numPossibleMatches; i++ )
    {
        ScanData temp = onceFilteredList[i];
        if( temp.location.X == -1000 )
        {
            break;
        }

        // no choice but to iterate over all matches again :sigh:
        bool doWeAlreadyHaveThisCard = false;
        for( int j=0; j<numPossibleMatches; j++ )
        {
            ScanData innerTemp = twiceFilteredList[j];
            if( innerTemp.location.X == -1000 )
            {
                break;
            }

            // if the tilts and pans are no more than a few degrees apart, skip
            // if the x and y coords are within... 20% horiz and vert, skip
            // assume no two cards are this close together
            int panTolerance = 5;
            int tiltTolerance = 5;
            int xTolerance = Camera::WIDTH*0.1;
            int yTolerance = Camera::HEIGHT*0.1;

            bool isPanMatched = (innerTemp.pan-panTolerance < temp.pan) && temp.pan < (innerTemp.pan + panTolerance);
            bool isTiltMatched = (innerTemp.tilt-tiltTolerance < temp.tilt) && temp.tilt < (innerTemp.tilt+tiltTolerance);
            bool isXMatched = (innerTemp.location.X-xTolerance < temp.location.X) && temp.location.X < (innerTemp.location.X+xTolerance);
            bool isYMatched = (innerTemp.location.Y-yTolerance < temp.location.Y) && temp.location.Y < (innerTemp.location.Y+yTolerance);

            if( isPanMatched && isTiltMatched && isXMatched && isYMatched )
            {
                // skip this one!
                doWeAlreadyHaveThisCard = true;
                break;
            }

            // otherwise, keep considering it!
            doWeAlreadyHaveThisCard = false;
        }
        if( !doWeAlreadyHaveThisCard )
        {
            twiceFilteredList[iter] = temp;
            iter++;
        }
    }
    twiceFilteredList[iter] = stop;

    delete( retList );
    delete( onceFilteredList );
    return( twiceFilteredList );
}

void Body::statusCheck()
{
    StatusCheck::Check(cm730);
    return;
}

void Body::moveHead(int pan, int tilt)
{
    int goodPan = pan;
    int goodTilt = tilt;
    if( tilt < -20 )
    {
        printf( "bad tilt!\n" );
        goodTilt = -20;
    }
    else if( tilt > 60 )
    {
        printf( "bad tilt!\n" );
        goodTilt = 60;
    }

    if( pan < -90 )
    {
        printf( "bad pan!\n" );
        goodPan = -90;
    }
    else if( pan > 90 )
    {
        printf( "bad pan!\n" );
        goodPan = 90;
    }

	MotionManager::GetInstance()->SetEnable(false);
    cm730.WriteWord(JointData::ID_HEAD_PAN, MX28::P_GOAL_POSITION_L, MX28::Angle2Value(goodPan), 0);
    cm730.WriteWord(JointData::ID_HEAD_TILT, MX28::P_GOAL_POSITION_L, MX28::Angle2Value(goodTilt), 0);

    int moving = 1;
    while( moving )
    {
        cm730.ReadWord(JointData::ID_HEAD_TILT, MX28::P_MOVING, &moving, 0);
    }
    return;
}

int Body::readHeadPan()
{
    int headPan, error;
    cm730.ReadWord(JointData::ID_HEAD_PAN, MX28::P_PRESENT_POSITION_L, &headPan, &error);
    return( headPan );
}

int Body::readHeadTilt()
{
    int headTilt, error;
    cm730.ReadWord(JointData::ID_HEAD_TILT, MX28::P_PRESENT_POSITION_L, &headTilt, &error);
    return( headTilt );
}

ScanData Body::centerGaze( ScanData card )
{
    // get angle per pixel ratio
    double horizRatio = Camera::VIEW_H_ANGLE / (double)Camera::WIDTH;
    double vertRatio = Camera::VIEW_V_ANGLE / (double)Camera::HEIGHT;

    Point2D center = Point2D(Camera::WIDTH/2, Camera::HEIGHT/2);
    Point2D absLoc = card.maculaOrigin + card.location;
    Point2D displacement = center - absLoc;

    int finalPan = card.pan + (displacement.X * horizRatio);
    int finalTilt = card.tilt + (displacement.Y * vertRatio);

    moveHead(finalPan, finalTilt);

    ScanData ret;
    ret.pan = finalPan;
    ret.tilt = finalTilt;

    return( ret );
}
