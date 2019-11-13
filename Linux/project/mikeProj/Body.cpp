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

Body::Body() : linux_cm730(LinuxCM730("/dev/ttyUSB0")) , cm730(CM730(&linux_cm730)), eyes(Eyes()), tracker(BallTracker())
{
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

void Body::statusCheck()
{
    StatusCheck::Check(cm730);
    return;
}

void Body::moveHead(Point2D pos)
{
//	MotionManager::GetInstance()->SetEnable(true);

	Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
    tracker.Process(pos);
    return;
}
// ================== BEGIN HEAD ====================
/*

#include <stdio.h>
#include "MX28.h"
#include "Kinematics.h"
#include "MotionStatus.h"
#include "Head.h"


Head* Head::m_UniqueInstance = new Head();

Head::Head()
{
	m_Pan_p_gain = 0.1;
	m_Pan_d_gain = 0.22;

    m_Tilt_p_gain = 0.1;
	m_Tilt_d_gain = 0.22;

	m_LeftLimit = 70;
	m_RightLimit = -70;
	m_TopLimit = Kinematics::EYE_TILT_OFFSET_ANGLE;
	m_BottomLimit = Kinematics::EYE_TILT_OFFSET_ANGLE - 65;

	m_Pan_Home = 0.0;
	m_Tilt_Home = Kinematics::EYE_TILT_OFFSET_ANGLE - 30.0;

	m_Joint.SetEnableHeadOnly(true);
}

Head::~Head()
{
}

void Head::CheckLimit()
{
	if(m_PanAngle > m_LeftLimit)
		m_PanAngle = m_LeftLimit;
	else if(m_PanAngle < m_RightLimit)
		m_PanAngle = m_RightLimit;

	if(m_TiltAngle > m_TopLimit)
		m_TiltAngle = m_TopLimit;
	else if(m_TiltAngle < m_BottomLimit)
		m_TiltAngle = m_BottomLimit;	
}

void Head::Initialize()
{
	m_PanAngle = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	m_TiltAngle = -MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_TILT);
	CheckLimit();

	InitTracking();
	MoveToHome();
}

void Head::LoadINISettings(minIni* ini)
{
    LoadINISettings(ini, HEAD_SECTION);
}

void Head::LoadINISettings(minIni* ini, const std::string &section)
{
    double value = INVALID_VALUE;

    if((value = ini->getd(section, "pan_p_gain", INVALID_VALUE)) != INVALID_VALUE)  m_Pan_p_gain = value;
    if((value = ini->getd(section, "pan_d_gain", INVALID_VALUE)) != INVALID_VALUE)  m_Pan_d_gain = value;
    if((value = ini->getd(section, "tilt_p_gain", INVALID_VALUE)) != INVALID_VALUE) m_Tilt_p_gain = value;
    if((value = ini->getd(section, "tilt_d_gain", INVALID_VALUE)) != INVALID_VALUE) m_Tilt_d_gain = value;
    if((value = ini->getd(section, "left_limit", INVALID_VALUE)) != INVALID_VALUE)  m_LeftLimit = value;
    if((value = ini->getd(section, "right_limit", INVALID_VALUE)) != INVALID_VALUE) m_RightLimit = value;
    if((value = ini->getd(section, "top_limit", INVALID_VALUE)) != INVALID_VALUE)   m_TopLimit = value;
    if((value = ini->getd(section, "bottom_limit", INVALID_VALUE)) != INVALID_VALUE)m_BottomLimit = value;
    if((value = ini->getd(section, "pan_home", INVALID_VALUE)) != INVALID_VALUE)    m_Pan_Home = value;
    if((value = ini->getd(section, "tilt_home", INVALID_VALUE)) != INVALID_VALUE)   m_Tilt_Home = value;
}

void Head::SaveINISettings(minIni* ini)
{
    SaveINISettings(ini, HEAD_SECTION);
}

void Head::SaveINISettings(minIni* ini, const std::string &section)
{
    ini->put(section,   "pan_p_gain",   m_Pan_p_gain);
    ini->put(section,   "pan_d_gain",   m_Pan_d_gain);
    ini->put(section,   "tilt_p_gain",  m_Tilt_p_gain);
    ini->put(section,   "tilt_d_gain",  m_Tilt_d_gain);
    ini->put(section,   "left_limit",   m_LeftLimit);
    ini->put(section,   "right_limit",  m_RightLimit);
    ini->put(section,   "top_limit",    m_TopLimit);
    ini->put(section,   "bottom_limit", m_BottomLimit);
    ini->put(section,   "pan_home",     m_Pan_Home);
    ini->put(section,   "tilt_home",    m_Tilt_Home);
}

void Head::MoveToHome()
{
	MoveByAngle(m_Pan_Home, m_Tilt_Home);
}

void Head::MoveByAngle(double pan, double tilt)
{
	m_PanAngle = pan;
	m_TiltAngle = tilt;

	CheckLimit();
}

void Head::MoveByAngleOffset(double pan, double tilt)
{	
	MoveByAngle(m_PanAngle + pan, m_TiltAngle + tilt);
}

void Head::InitTracking()
{
	m_Pan_err = 0;
	m_Pan_err_diff = 0;
	m_Tilt_err = 0;
	m_Tilt_err_diff = 0;
}

void Head::MoveTracking(Point2D err)
{	
	m_Pan_err_diff = err.X - m_Pan_err;
	m_Pan_err = err.X;

	m_Tilt_err_diff = err.Y - m_Tilt_err;
	m_Tilt_err = err.Y;

	MoveTracking();
}

void Head::MoveTracking()
{
	double pOffset, dOffset;

	pOffset = m_Pan_err * m_Pan_p_gain;
	pOffset *= pOffset;
	if(m_Pan_err < 0)
		pOffset = -pOffset;
	dOffset = m_Pan_err_diff * m_Pan_d_gain;
	dOffset *= dOffset;
	if(m_Pan_err_diff < 0)
		dOffset = -dOffset;
	m_PanAngle += (pOffset + dOffset);

	pOffset = m_Tilt_err * m_Tilt_p_gain;
	pOffset *= pOffset;
	if(m_Tilt_err < 0)
		pOffset = -pOffset;
	dOffset = m_Tilt_err_diff * m_Tilt_d_gain;
	dOffset *= dOffset;
	if(m_Tilt_err_diff < 0)
		dOffset = -dOffset;
	m_TiltAngle += (pOffset + dOffset);

	CheckLimit();
}

void Head::Process()
{
	if(m_Joint.GetEnable(JointData::ID_HEAD_PAN) == true)
		m_Joint.SetAngle(JointData::ID_HEAD_PAN, m_PanAngle);

	if(m_Joint.GetEnable(JointData::ID_HEAD_TILT) == true)
		m_Joint.SetAngle(JointData::ID_HEAD_TILT, m_TiltAngle);
}
*/

