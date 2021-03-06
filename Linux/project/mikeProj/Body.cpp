/*
 * Michael Neises
 * 1 nov 2019
 * player
 */

#include "Body.h"
#include "bodyLibrary.cpp"
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

void Body::changeEyeColor( Color col )
{
    switch( col )
    {
        case RED:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_EYE_L, cm730.MakeColor(255, 0, 0), 0);
            break;
        case GREEN:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_EYE_L, cm730.MakeColor(0, 255, 0), 0);
            break;
        case BLUE:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_EYE_L, cm730.MakeColor(0, 0, 255), 0);
            break;
        case ORANGE:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_EYE_L, cm730.MakeColor(225, 64, 0), 0);
            break;
        case YELLOW:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_EYE_L, cm730.MakeColor(128, 225, 0), 0);
            break;
        case PURPLE:
            cm730.WriteWord(cm730.ID_CM, cm730.P_LED_EYE_L, cm730.MakeColor(225, 0, 225), 0);
            break;
    }
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
        eyes.maculaLook( startRow, startCol, percent, false );
        time(&nowTimer);
    }
    return;
}

void refreshEyes(Eyes& eyes)
{
    eyes.maculaLook(0,0,10,false);
    eyes.maculaLook(0,0,10,false);
    eyes.maculaLook(0,0,10,false);
    eyes.maculaLook(0,0,10,false);
    eyes.maculaLook(0,0,10,false);
    return;
}

ScanData* Body::scan()
{
    int percent = 25;
    int numHeadPositions = 3;

    // *8 because a macula can get a hit on the same position 8 times
    // happens when a card is in the intersection of 4 maculae
    // on two different head positions whose gaze overlaps
    int numPossibleMatches = (100/percent)*8*numHeadPositions;

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
    moveHead( -58, -20 );
    refreshEyes(eyes);
    eyes.partitionScan( percent, -58, -20, retList ); 
    moveHead( 0, -40 );
    refreshEyes(eyes);
    eyes.partitionScan( percent, 0, -40, retList ); 
    moveHead( 58, -20 );
    refreshEyes(eyes);
    eyes.partitionScan( percent, 58, -20, retList ); 

    // ===============================================
    // for each potential card,
        // center the macula at that card
        // add its new center to a list
    // ===============================================
    
    ScanData* onceFilteredList = (ScanData*)malloc(sizeof(ScanData)*numPossibleMatches);
    int iter = 0;
    for( int i=0; i<numPossibleMatches; i++ )
    {
        ScanData temp = retList[i];
        if( temp.location.X == -1000 )
        {
            break;
        }
        if( temp.color != UNKNOWN )
        {
            if( temp.location.X == -1 )
            {
                continue;
            }

            // orient the gaze
            moveHead( temp.pan, temp.tilt );
            refreshEyes(eyes);

            // grow the macula
            ScanData result = eyes.growMacula( temp, percent );
            result.pan = temp.pan;
            result.tilt = temp.tilt;

            onceFilteredList[iter] = result;
            iter++;
        }
    }
    onceFilteredList[iter] = stop;

    ScanData* twiceFilteredList = (ScanData*)malloc(sizeof(ScanData)*numPossibleMatches);
    scanFilter( onceFilteredList, twiceFilteredList, percent, false );

    ScanData* thriceFilteredList = (ScanData*)malloc(sizeof(ScanData)*numPossibleMatches);
    reorientFilter( twiceFilteredList, thriceFilteredList, percent, numPossibleMatches, false );

    ScanData* fourFilteredList = (ScanData*)malloc(sizeof(ScanData)*numPossibleMatches);
    reorientFilter( thriceFilteredList, fourFilteredList, percent, numPossibleMatches, false );

    ScanData* fiveFilteredList = (ScanData*)malloc(sizeof(ScanData)*numPossibleMatches);
    reorientFilter( fourFilteredList, fiveFilteredList, percent, numPossibleMatches, true );

    delete( retList );
    delete( onceFilteredList );
    delete( twiceFilteredList );
    delete( thriceFilteredList );
    delete( fourFilteredList );
    return( fiveFilteredList );
}

void Body::statusCheck()
{
    StatusCheck::Check(cm730);
    return;
}

void Body::moveHead(int pan, int tilt)
{
    int goodPan = pan*-1;
    int goodTilt = tilt+20;
    if( tilt < -40 )
    {
        printf( "bad tilt!\n" );
        goodTilt = -40;
    }
    else if( tilt > 40 )
    {
        printf( "bad tilt!\n" );
        goodTilt = 40;
    }

    if( pan < -90 )
    {
        printf( "bad pan!\n" );
        goodPan = 90;
    }
    else if( pan > 90 )
    {
        printf( "bad pan!\n" );
        goodPan = -90;
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

int angleToPitch( bool isRight, int angle )
{
    if( isRight )
    {
        return( (-1*angle+270)*4100/360 );
    }
    return( (angle+90)*4100/360 );
}

int angleToRoll( bool isRight, int angle )
{
    if( isRight )
    {
        return( (-1*angle+220)*4100/360 );
    }
    return( (angle+140)*4100/360 );
}

void Body::moveShoulder(bool isRight, int pitch, int roll)
{
    //make elbows rigid
    cm730.WriteWord(JointData::ID_L_ELBOW, MX28::P_GOAL_POSITION_L, 3109, 0);
    cm730.WriteWord(JointData::ID_R_ELBOW, MX28::P_GOAL_POSITION_L, 996, 0);

    int goodPitch = pitch;
    int goodRoll = roll;
    if( roll < -85 )
    {
        printf( "bad Roll!\n" );
        goodRoll = -85;
    }
    else if( roll > 85 )
    {
        printf( "bad roll!\n" );
        goodRoll = 85;
    }

    if( pitch < -90 )
    {
        printf( "bad pitch!\n" );
        goodPitch = -90;
    }
    else if( pitch > 90 )
    {
        printf( "bad pitch!\n" );
        goodPitch = 90;
    }

    int pitchVal = angleToPitch( isRight, goodPitch );
    int rollVal = angleToRoll( isRight, goodRoll );

	MotionManager::GetInstance()->SetEnable(false);
    if( isRight )
    {
        cm730.WriteWord(JointData::ID_R_SHOULDER_PITCH, MX28::P_GOAL_POSITION_L, pitchVal, 0);
        cm730.WriteWord(JointData::ID_R_SHOULDER_ROLL, MX28::P_GOAL_POSITION_L, rollVal, 0);
    }
    else
    {
        cm730.WriteWord(JointData::ID_L_SHOULDER_PITCH, MX28::P_GOAL_POSITION_L, pitchVal, 0);
        cm730.WriteWord(JointData::ID_L_SHOULDER_ROLL, MX28::P_GOAL_POSITION_L, rollVal, 0);
    }

    int LPmoving = 1;
    int LRmoving = 1;
    int RPmoving = 1;
    int RRmoving = 1;
    while( LPmoving || LRmoving || RPmoving || RRmoving )
    {
        cm730.ReadWord(JointData::ID_L_SHOULDER_PITCH, MX28::P_MOVING, &LPmoving, 0);
        cm730.ReadWord(JointData::ID_L_SHOULDER_ROLL, MX28::P_MOVING, &LRmoving, 0);
        cm730.ReadWord(JointData::ID_R_SHOULDER_PITCH, MX28::P_MOVING, &RPmoving, 0);
        cm730.ReadWord(JointData::ID_R_SHOULDER_ROLL, MX28::P_MOVING, &RRmoving, 0);
    }
    return;
}

int Body::readShoulderPitch(bool isRight)
{
    int shoulderPitch, error;
    if( isRight )
    {
        cm730.ReadWord(JointData::ID_R_SHOULDER_PITCH, MX28::P_PRESENT_POSITION_L, &shoulderPitch, &error);
    }
    else
    {
        cm730.ReadWord(JointData::ID_L_SHOULDER_PITCH, MX28::P_PRESENT_POSITION_L, &shoulderPitch, &error);
    }
    return( shoulderPitch );
}

int Body::readShoulderRoll(bool isRight)
{
    int shoulderRoll, error;
    if( isRight )
    {
        cm730.ReadWord(JointData::ID_R_SHOULDER_ROLL, MX28::P_PRESENT_POSITION_L, &shoulderRoll, &error);
    }
    else
    {
        cm730.ReadWord(JointData::ID_L_SHOULDER_ROLL, MX28::P_PRESENT_POSITION_L, &shoulderRoll, &error);
    }
    cm730.ReadWord(JointData::ID_L_ELBOW, MX28::P_PRESENT_POSITION_L, &shoulderRoll, &error);
    printf( "Left elbow is %d\n", shoulderRoll );
    cm730.ReadWord(JointData::ID_R_ELBOW, MX28::P_PRESENT_POSITION_L, &shoulderRoll, &error);
    printf( "Right elbow is %d\n", shoulderRoll );
    return( shoulderRoll );
}

void Body::moveShoulderByGaze( int pan, int tilt )
{
    // this doesn't work

    double overIt = 0;
    if( pan > 0 )
    {
        overIt = 90-pan;
    }
    else
    {
        overIt = 90+pan;
    }

    double adjustRoll = tilt;
    double adjustPitch = tilt*sin(-1*pan);
    double temp =  (90-overIt)*cos(pan);

    if( pan > 0 )
    {
        moveShoulder( true, adjustPitch, overIt-adjustRoll );
    }
    else
    {
        moveShoulder( false, adjustPitch, overIt-adjustRoll );
    }
    return;
}

ScanData Body::centerGaze( ScanData card )
{
    // get "angle per pixel" ratio
    double horizRatio = Camera::VIEW_H_ANGLE / (double)Camera::WIDTH;
    double vertRatio = Camera::VIEW_V_ANGLE / (double)Camera::HEIGHT;

    Point2D center = Point2D(Camera::WIDTH/2, Camera::HEIGHT/2);
    Point2D absLoc = card.maculaOrigin + card.location;
    Point2D displacement = center - absLoc;

    int finalPan = card.pan - (displacement.X * horizRatio);
    int finalTilt = card.tilt + (displacement.Y * vertRatio);

    moveHead(finalPan, finalTilt);
    refreshEyes(eyes);

    ScanData ret;
    ret.pan = finalPan;
    ret.tilt = finalTilt;

    return( ret );
}

