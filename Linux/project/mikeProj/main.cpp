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

int main(void)
{
	Player* DarCon = new Player();

	LinuxCM730 linux_cm730("/dev/ttyUSB0");
	CM730 cm730(&linux_cm730);

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

	ColorFinder* red_finder = new ColorFinder(340, 15, 45, 0, 0.3, 50.0);
	red_finder->LoadINISettings(ini, "RED");
	httpd::red_finder = red_finder;

	ColorFinder* orange_finder = new ColorFinder(5, 15, 45, 0, 0.3, 50.0);
	orange_finder->LoadINISettings(ini, "ORANGE");
	httpd::orange_finder = orange_finder;

	ColorFinder* yellow_finder = new ColorFinder(60, 15, 45, 0, 0.3, 50.0);
	yellow_finder->LoadINISettings(ini, "YELLOW");
	httpd::yellow_finder = yellow_finder;

	ColorFinder* green_finder = new ColorFinder(165, 15, 45, 0, 0.3, 50.0);
	green_finder->LoadINISettings(ini, "BLUE");
	httpd::green_finder = green_finder;

	ColorFinder* blue_finder = new ColorFinder(215, 5, 30, 30, 0.3, 50.0);
	blue_finder->LoadINISettings(ini, "BLUE");
	httpd::blue_finder = blue_finder;

	ColorFinder* purple_finder = new ColorFinder(280, 15, 20, 20, 0.3, 50.0);
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
			return(0);
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

	// MIKE NOTE
	// START OF PROGRAM (END OF INITS)

	//LinuxActionScript::PlayMP3("../../../Data/mp3/Demonstration ready mode.mp3");
	Action::GetInstance()->Start(15);

	Color lastDetected = UNKNOWN;

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


		if(StatusCheck::m_is_started == 0)
			continue;

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
		/*
		   if(Action::GetInstance()->IsRunning() == 0)
		   VisionMode::Play(detected_color);
		 */
		if((detected_color & RED) != 0 && lastDetected != RED)
		{
			printf( "got red\n" );
			lastDetected = RED;
			LinuxActionScript::PlayMP3("../../../Data/mp3/Wow.mp3");
			//StatusCheck::m_is_started = 0;
			DarCon->changeGemColor( &cm730, RED );
		}
		else if((detected_color & YELLOW) != 0 && lastDetected != YELLOW)
		{
			printf( "got yellow\n" );
			lastDetected = YELLOW;
			LinuxActionScript::PlayMP3("../../../Data/mp3/Wow.mp3");
			//StatusCheck::m_is_started = 0;
			DarCon->changeGemColor( &cm730, YELLOW );
		}
		else if((detected_color & BLUE) != 0 && lastDetected != BLUE)
		{
			printf( "got blue\n" );
			lastDetected = BLUE;
			LinuxActionScript::PlayMP3("../../../Data/mp3/Wow.mp3");
			//StatusCheck::m_is_started = 0;
			DarCon->changeGemColor( &cm730, BLUE );
		}

	}
}
