/*
 * Michael Neises
 * 1 nov 2019
 * eyes
 */

#include "Eyes.h"
#include <stdio.h>

Eyes::Eyes()
{
    minIni* ini = new minIni(INI_FILE_PATH);

    LinuxCamera::GetInstance()->Initialize(0);
    LinuxCamera::GetInstance()->SetCameraSettings(CameraSettings());    // set default
    LinuxCamera::GetInstance()->LoadINISettings(ini);                   // load from ini

    streamer = new mjpg_streamer(Camera::WIDTH, Camera::HEIGHT);

    red_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    red_finder->LoadINISettings(ini, "RED");
    httpd::red_finder = red_finder;

    orange_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    orange_finder->LoadINISettings(ini, "ORANGE");
    httpd::orange_finder = orange_finder;

    yellow_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    yellow_finder->LoadINISettings(ini, "YELLOW");
    httpd::yellow_finder = yellow_finder;

    green_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    green_finder->LoadINISettings(ini, "GREEN");
    httpd::green_finder = green_finder;

    blue_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    blue_finder->LoadINISettings(ini, "BLUE");
    httpd::blue_finder = blue_finder;

    purple_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    purple_finder->LoadINISettings(ini, "PURPLE");
    httpd::purple_finder = purple_finder;

    /* UNUSED COLORS

       pink_finder = new ColorFinder(305, 20, 9, 0, 0.3, 50.0);
       pink_finder->LoadINISettings(ini, "PINK");
       httpd::pink_finder = pink_finder;

       brown_finder = new ColorFinder(10, 20, 9, 0, 0.3, 50.0);
       brown_finder->LoadINISettings(ini, "BROWN");
       httpd::brown_finder = brown_finder;

       white_finder = new ColorFinder(205, 10, 0, 70, 0.3, 50.0);
       white_finder->LoadINISettings(ini, "WHITE");
       httpd::white_finder = white_finder;

    // black needs to be at Exposure = 75

    ColorFinder* black_finder = new ColorFinder(215, 10, 30, 30, 0.3, 50.0);
    black_finder->LoadINISettings(ini, "BLACK");
    httpd::black_finder = black_finder;

*/

    httpd::ini = ini;
}

Eyes::~Eyes()
{
    delete( red_finder );
    delete( orange_finder );
    delete( yellow_finder );
    delete( green_finder );
    delete( blue_finder );
    delete( purple_finder );
    delete( streamer );
}
bool Eyes::tryHit( Color col )
{
    int detected_color = look().color;
    if( detected_color == col )
    {
        return( true );
    }
    return( false );
}

ScanData Eyes::look()
{
    Point2D red_pos, orange_pos, yellow_pos, green_pos, blue_pos, purple_pos;

    LinuxCamera::GetInstance()->CaptureFrame();

    Image* rgb_output = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);
    memcpy(rgb_output->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);

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
    streamer->send_image(rgb_output);

    int detected_color = 0;

    detected_color |= (red_pos.X == -1)? 0 : RED;
    detected_color |= (orange_pos.X == -1)? 0 : ORANGE;
    detected_color |= (yellow_pos.X == -1)? 0 : YELLOW;
    detected_color |= (green_pos.X == -1)? 0 : GREEN;
    detected_color |= (blue_pos.X == -1)? 0 : BLUE;
    detected_color |= (purple_pos.X == -1)? 0 : PURPLE;

    ScanData ret = {};
    if( (detected_color & RED) != 0 )
    {
        ret.color = RED;
        ret.location = red_pos;
    }
    else if( (detected_color & ORANGE) != 0 )
    {
        ret.color = ORANGE;
        ret.location = orange_pos;
    }
    else if( (detected_color & YELLOW) != 0 )
    {
        ret.color = YELLOW;
        ret.location = yellow_pos;
    }
    else if( (detected_color & GREEN) != 0 )
    {
        ret.color = GREEN;
        ret.location = green_pos;
    }
    else if( (detected_color & BLUE) != 0 )
    {
        ret.color = BLUE;
        ret.location = blue_pos;
    }
    else if( (detected_color & PURPLE) != 0 )
    {
        ret.color = PURPLE;
        ret.location = purple_pos;
    }
    else
    {
        ret.color = UNKNOWN;
        ret.location = red_pos;
    }
    return( ret );
}

void Eyes::learnRed(int hue, bool isLearning)
{
    minIni* ini = new minIni(INI_FILE_PATH);
    if( isLearning )
    {
        red_finder = new ColorFinder(hue, 10, 45, 0, 30, 80);
    }
    else
    {
        red_finder = new ColorFinder(hue, 20, 45, 0, 30, 100);
    }
    httpd::red_finder = red_finder;
    delete( ini );
}

void Eyes::learnOrange(int hue, bool isLearning)
{
    minIni* ini = new minIni(INI_FILE_PATH);
    if( isLearning )
    {
        orange_finder = new ColorFinder(hue, 15, 35, 0, 20, 80);
    }
    else
    {
        orange_finder = new ColorFinder(hue, 5, 35, 0, 30, 100);
    }
    httpd::orange_finder = orange_finder;
    delete( ini );
}

void Eyes::learnYellow(int hue, bool isLearning)
{
    minIni* ini = new minIni(INI_FILE_PATH);
    if( isLearning )
    {
        yellow_finder = new ColorFinder(hue, 15, 65, 50, 30, 80);
    }
    else
    {
        yellow_finder = new ColorFinder(hue, 5, 65, 50, 30, 100);
    }
    httpd::yellow_finder = yellow_finder;
    delete( ini );
}

void Eyes::learnGreen(int hue, bool isLearning)
{
    minIni* ini = new minIni(INI_FILE_PATH);
    if( isLearning )
    {
        green_finder = new ColorFinder(hue, 10, 20, 0, 30, 80);
    }
    else
    {
        green_finder = new ColorFinder(hue, 20, 20, 0, 30, 100);
    }
    httpd::green_finder = green_finder;
    delete( ini );
}

void Eyes::learnBlue(int hue, bool isLearning)
{
    minIni* ini = new minIni(INI_FILE_PATH);
    if( isLearning )
    {
        blue_finder = new ColorFinder(hue, 10, 30, 30, 30, 80);
    }
    else
    {
        blue_finder = new ColorFinder(hue, 20, 30, 30, 30, 100);
    }
    httpd::blue_finder = blue_finder;
    delete( ini );
}

void Eyes::learnPurple(int hue, bool isLearning)
{
    minIni* ini = new minIni(INI_FILE_PATH);
    if( isLearning )
    {
        purple_finder = new ColorFinder(hue, 40, 20, 20, 30, 80);
    }
    else
    {
        purple_finder = new ColorFinder(hue, 40, 20, 20, 30, 100);
    }
    httpd::purple_finder = purple_finder;
    delete( ini );
}

int coord2index( int row, int column )
{
    return( column + row*320 );
}

// for rgb frames
// assume the row, col pair is in the domain
int coord2maculaindex( int row, int column, int macStartRow, int macStartColumn, double percent )
{
    int rowSize = Camera::WIDTH / (100.0/percent);
    return( (column-macStartColumn) + (row-macStartRow)*rowSize );
}


// look at the center
ScanData Eyes::maculaLook(double percent)
{
    // calc the right starting coordinates
    int macH = Camera::HEIGHT/(100.0/percent);
    int macW = Camera::WIDTH/(100.0/percent);

    int startRow = (Camera::HEIGHT - macH)/2 + 1;
    int startCol = (Camera::WIDTH - macW)/2 + 1;

    // the first pixel of the percent% macula 
    return( maculaLook(startRow, startCol, percent) );
}

ScanData Eyes::maculaLook(int macStartRow, int macStartColumn, double percent)
{
    return( maculaLook(int macStartRow, int macStartColumn, double percent, true) );
}

ScanData Eyes::maculaLook(int macStartRow, int macStartColumn, double percent, bool needTakePicture)
{
    Point2D red_pos, orange_pos, yellow_pos, green_pos, blue_pos, purple_pos;
    Image* rgb_output = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);
    Image* hsv_output = new Image(Camera::WIDTH/(100.0/percent), Camera::HEIGHT/(100.0/percent), Image::HSV_PIXEL_SIZE);
    int macH = Camera::HEIGHT/(100.0/percent);
    int macW = Camera::WIDTH/(100.0/percent);

    if( needTakePicture )
    {
        LinuxCamera::GetInstance()->CaptureFrame(); 
    }
    memcpy(rgb_output->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);

    // selectively a chunk of the original imag
    Image* hsvFrame = LinuxCamera::GetInstance()->fbuffer->m_HSVFrame;
    for( int row=macStartRow; row<macStartRow+macH; row++ )
    {
        for( int column=macStartColumn; column<macStartColumn+macW; column++ )
        {
            int smallIndex = coord2maculaindex( row, column, macStartRow, macStartColumn, percent );
            int bigIndex = coord2index( row, column );
            for( int i=0; i<Image::HSV_PIXEL_SIZE; i++ )
            {
                hsv_output->m_ImageData[(smallIndex*Image::HSV_PIXEL_SIZE)+i] = hsvFrame->m_ImageData[(bigIndex*Image::HSV_PIXEL_SIZE)+i];
            }
        }
    }

    red_pos = red_finder->GetPosition(hsv_output);
    orange_pos = orange_finder->GetPosition(hsv_output);
    yellow_pos = yellow_finder->GetPosition(hsv_output);
    green_pos = green_finder->GetPosition(hsv_output);
    blue_pos = blue_finder->GetPosition(hsv_output);
    purple_pos = purple_finder->GetPosition(hsv_output);

    unsigned char r, g, b;
    for( int row=macStartRow; row<macStartRow+macH; row++ )
    {
        for( int column=macStartColumn; column<macStartColumn+macW; column++ )
        {
            r = 0; g = 0; b = 0;
            int smallIndex = coord2maculaindex( row, column, macStartRow, macStartColumn, percent );
            int bigIndex = coord2index( row, column );

            if(red_finder->m_result->m_ImageData[smallIndex] == 1)
            {
                r = 255;
                g = 0;
                b = 0;
            }
            if(orange_finder->m_result->m_ImageData[smallIndex] == 1)
            {
                r = 255;
                g = 128;
                b = 0;
            }
            if(yellow_finder->m_result->m_ImageData[smallIndex] == 1)
            {
                r = 255;
                g = 255;
                b = 0;
            }
            if(green_finder->m_result->m_ImageData[smallIndex] == 1)
            {
                r = 0;
                g = 255;
                b = 0;
            }
            if(blue_finder->m_result->m_ImageData[smallIndex] == 1)
            {
                r = 0;
                g = 0;
                b = 255;
            }
            if(purple_finder->m_result->m_ImageData[smallIndex] == 1)
            {
                r = 255;
                g = 0;
                b = 255;
            }

            if(r > 0 || g > 0 || b > 0)
            {
                rgb_output->m_ImageData[bigIndex*rgb_output->m_PixelSize + 0] = r;
                rgb_output->m_ImageData[bigIndex*rgb_output->m_PixelSize + 1] = g;
                rgb_output->m_ImageData[bigIndex*rgb_output->m_PixelSize + 2] = b;
            }
        }
    }

    streamer->send_image(rgb_output);

    int detected_color = 0;

    detected_color |= (red_pos.X == -1)? 0 : RED;
    detected_color |= (orange_pos.X == -1)? 0 : ORANGE;
    detected_color |= (yellow_pos.X == -1)? 0 : YELLOW;
    detected_color |= (green_pos.X == -1)? 0 : GREEN;
    detected_color |= (blue_pos.X == -1)? 0 : BLUE;
    detected_color |= (purple_pos.X == -1)? 0 : PURPLE;

    delete( rgb_output );
    rgb_output = NULL;
    delete( hsv_output );
    hsv_output = NULL;

    ScanData ret = {};
    Point2D macOrigin = Point2D(macStartColumn, macStartRow);
    ret.maculaOrigin = macOrigin;
    if( (detected_color & RED) != 0 )
    {
        ret.color = RED;
        ret.location = red_pos;
    }
    else if( (detected_color & ORANGE) != 0 )
    {
        ret.color = ORANGE;
        ret.location = orange_pos;
    }
    else if( (detected_color & YELLOW) != 0 )
    {
        ret.color = YELLOW;
        ret.location = yellow_pos;
    }
    else if( (detected_color & GREEN) != 0 )
    {
        ret.color = GREEN;
        ret.location = green_pos;
    }
    else if( (detected_color & BLUE) != 0 )
    {
        ret.color = BLUE;
        ret.location = blue_pos;
    }
    else if( (detected_color & PURPLE) != 0 )
    {
        ret.color = PURPLE;
        ret.location = purple_pos;
    }
    else
    {
        ret.color = UNKNOWN;
        ret.location = red_pos;
    }
    return( ret );
}

void Eyes::partitionScan( double percent, int pan, int tilt, ScanData* retList )
{
    int macH = Camera::HEIGHT/(100.0/percent);
    int macW = Camera::WIDTH/(100.0/percent);
    LinuxCamera::GetInstance()->CaptureFrame(); 

    Point2D stopPoint( -1000, -1000 );
    ScanData stop = {};
    stop.location = stopPoint;
    stop.color = UNKNOWN;

    int iter = 0;

    for( int row=0; row<Camera::HEIGHT-macH+1; row+=macH )
    {
        for( int col=0; col<Camera::WIDTH-macW+1; col+= macW )
        {
            time_t startTimer;
            time_t nowTimer;
            time(&startTimer);
            time(&nowTimer);
            //while( difftime(nowTimer, startTimer) < 0.001 )
            //{
                ScanData temp = maculaLook( row, col, percent, false );
                temp.tilt = tilt;
                temp.pan = pan;
                retList[iter] = temp;
                time(&nowTimer);
            //}
            iter++;
            //maculaLook( row, col, percent );
        }
    }
    retList[iter] = stop;
    return;
}

