/*
 * Michael Neises
 * 1 nov 2019
 * eyes
 */

#include "Eyes.h"
#include <stdio.h>

Eyes::Eyes()
{
    m_debug = true;
    m_minCardSize = 2;
    m_maxCardSize = 100;

    minIni* ini = new minIni(INI_FILE_PATH);

    LinuxCamera::GetInstance()->Initialize(0);
    LinuxCamera::GetInstance()->SetCameraSettings(CameraSettings());    // set default
    LinuxCamera::GetInstance()->LoadINISettings(ini);                   // load from ini

    streamer = new mjpg_streamer(Camera::WIDTH, Camera::HEIGHT);

    red_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    red_finder->LoadINISettings(ini, "RED");
    httpd::red_finder = red_finder;

    green_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    green_finder->LoadINISettings(ini, "GREEN");
    httpd::green_finder = green_finder;

    blue_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
    blue_finder->LoadINISettings(ini, "BLUE");
    httpd::blue_finder = blue_finder;

    back_finder = new ColorFinder(0, 0, 20, 40, 24, 50.0);
    back_finder->LoadINISettings(ini, "BACK");
    httpd::back_finder = back_finder;

    /* UNUSED COLORS

       orange_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
       orange_finder->LoadINISettings(ini, "ORANGE");
       httpd::orange_finder = orange_finder;

       yellow_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
       yellow_finder->LoadINISettings(ini, "YELLOW");
       httpd::yellow_finder = yellow_finder;

       purple_finder = new ColorFinder(0, 0, 45, 0, 24, 50.0);
       purple_finder->LoadINISettings(ini, "PURPLE");
       httpd::purple_finder = purple_finder;

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
    delete( green_finder );
    delete( blue_finder );
    delete( streamer );
}

void Eyes::setMinCardSize( int size )
{
    m_minCardSize = size;
}

void Eyes::setMaxCardSize( int size )
{
    m_maxCardSize = size;
}

bool Eyes::tryHit( Color col )
{
    if( look().color == col )
    {
        return( true );
    }
    return( false );
}

bool Eyes::tryHitSize( Color col )
{
    int detected_color0 = look().color;
    int detected_color1 = look().color;
    int detected_color2 = look().color;

    if( detected_color0 == col )
    {
        if( detected_color0 == detected_color1 )
        {
            if( detected_color0 == detected_color2 )
            {
                return( true );
            }
        }
    }
    return( false );
}

ScanData Eyes::look()
{
    Point2D red_pos, green_pos, blue_pos, back_pos;

    LinuxCamera::GetInstance()->CaptureFrame();

    Image* rgb_output = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);
    memcpy(rgb_output->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);

    red_pos = red_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
    green_pos = green_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
    blue_pos = blue_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
    back_pos = back_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);

    if( m_debug )
    {
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
            if(back_finder->m_result->m_ImageData[i] == 1)
            {
                r = 255;
                g = 255;
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
    }

    int detected_color = 0;

    detected_color |= (red_pos.X == -1)? 0 : RED;
    detected_color |= (green_pos.X == -1)? 0 : GREEN;
    detected_color |= (blue_pos.X == -1)? 0 : BLUE;
    detected_color |= (back_pos.X == -1)? 0 : BACK;

    ScanData ret = {};
    if( (detected_color & RED) != 0 )
    {
        ret.color = RED;
        ret.location = red_pos;
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
    else if( (detected_color & BACK) != 0 )
    {
        ret.color = BACK;
        ret.location = back_pos;
    }
    else
    {
        ret.color = UNKNOWN;
        ret.location = red_pos;
    }
    delete(rgb_output);
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
        red_finder = new ColorFinder(hue, 20, 45, 0, m_minCardSize, m_maxCardSize);
    }
    httpd::red_finder = red_finder;
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
        green_finder = new ColorFinder(hue, 20, 20, 0, m_minCardSize, m_maxCardSize);
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
        blue_finder = new ColorFinder(hue, 20, 30, 30, m_minCardSize, m_maxCardSize);
    }
    httpd::blue_finder = blue_finder;
    delete( ini );
}

void Eyes::learnBack(int hue, bool isLearning)
{
    minIni* ini = new minIni(INI_FILE_PATH);
    if( isLearning )
    {
        back_finder = new ColorFinder(hue, 10, 20, 40, 30, 80);
    }
    else
    {
        back_finder = new ColorFinder(hue, 20, 20, 40, m_minCardSize, m_maxCardSize);
    }
    httpd::back_finder = back_finder;
    delete( ini );
}

void Eyes::learnCardSize( int hue, int min, int max )
{
    minIni* ini = new minIni(INI_FILE_PATH);
    green_finder = new ColorFinder(hue, 40, 20, 20, min, max);
    httpd::green_finder = green_finder;
    delete( ini );
    return;
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
    return( maculaLook(macStartRow, macStartColumn, percent, true) );
}

ScanData Eyes::maculaLook(int macStartRow, int macStartColumn, double percent, bool needTakePicture)
{
    Point2D red_pos, green_pos, blue_pos, back_pos;
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
    green_pos = green_finder->GetPosition(hsv_output);
    blue_pos = blue_finder->GetPosition(hsv_output);
    back_pos = back_finder->GetPosition(hsv_output);

    if( m_debug )
    {
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
                if(back_finder->m_result->m_ImageData[smallIndex] == 1)
                {
                    r = 255;
                    g = 255;
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
    }

    int detected_color = 0;

    detected_color |= (red_pos.X == -1)? 0 : RED;
    detected_color |= (green_pos.X == -1)? 0 : GREEN;
    detected_color |= (blue_pos.X == -1)? 0 : BLUE;

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
    else if( (detected_color & BACK) != 0 )
    {
        ret.color = BACK;
        ret.location = back_pos;
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

    int iter = 0;

    // start after a stopPoint
    ScanData item = retList[iter];
    while( item.location.X != -1000 )
    {
        iter++;
        item = retList[iter];
    }

    for( int row=0; row<Camera::HEIGHT-macH+1; row+=macH )
    {
        for( int col=0; col<Camera::WIDTH-macW+1; col+= macW )
        {
            // can probably get a speedup out of not taking a picture every time
            // is bugged now, tho
            // always scans over the same picture :shrug:
            ScanData temp = maculaLook( row, col, percent, true );
            if( temp.color != UNKNOWN )
            {
                temp.tilt = tilt;
                temp.pan = pan;
                retList[iter] = temp;
                iter++;
            }
        }
    }

    Point2D stopPoint( -1000, -1000 );
    ScanData stop = {};
    stop.location = stopPoint;
    stop.color = UNKNOWN;

    retList[iter] = stop;

    return;
}

