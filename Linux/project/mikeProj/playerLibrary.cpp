
int getMid(int bot, int top)
{
    if( bot < top )
    {
        return( (bot+top)/2 );
    }
    return( (bot+top-360)/2 );
}

void Player::learnRed()
{
    festival_say_text("Please show me RED.");
    Point2D myPoint = Point2D(180.0,227.0);
    body.moveHead(myPoint);
    sleep(5);

    // these are meant to be upper and lower bounds for what the hue might be
    int startBottom = 330;
    int startTop = 30;

    // this will be the chosen value for the indicated REDor
    // also a couple helpers
    int hue = 0;
    int bottomHue = 0;
    int topHue = 0;

    // scan from ntc up
    for( int i=startBottom; i<startBottom+360; i++ )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnRed(iter, true);
        if( body.eyes.tryHit(RED) )
        {
            bottomHue = iter;
            break;
        }
    }

    // scan from ntc down
    for( int i=startTop; i>startTop-360; i-- )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnRed(iter, true);
        if( body.eyes.tryHit(RED) )
        {
            topHue = iter;
            break;
        }
    }

    // split the difference
    hue = getMid(bottomHue, topHue);
    printf( "Red is %d\n", hue );

    body.eyes.learnRed(hue, false);

    return;
}

void Player::learnOrange()
{
    festival_say_text("Please show me Orange.");
    Point2D myPoint = Point2D(180.0,227.0);
    body.moveHead(myPoint);
    sleep(5);

    // these are meant to be upper and lower bounds for what the hue might be
    int startBottom = 345;
    int startTop = 35;

    // this will be the chosen value for the indicated ORANGEor
    // also a couple helpers
    int hue = 0;
    int bottomHue = 0;
    int topHue = 0;

    // scan from ntc up
    for( int i=startBottom; i<startBottom+360; i++ )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnOrange(iter, true);
        if( body.eyes.tryHit(ORANGE) )
        {
            bottomHue = iter;
            break;
        }
    }

    // scan from ntc down
    for( int i=startTop; i>startTop-360; i-- )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnOrange(iter, true);
        if( body.eyes.tryHit(ORANGE) )
        {
            topHue = iter;
            break;
        }
    }

    // split the difference
    hue = getMid(bottomHue, topHue);
    printf( "Orange is %d\n", hue );

    body.eyes.learnOrange(hue, false);

    return;
}

void Player::learnYellow()
{
    festival_say_text("Please show me Yellow.");
    Point2D myPoint = Point2D(180.0,227.0);
    body.moveHead(myPoint);
    sleep(5);

    // these are meant to be upper and lower bounds for what the hue might be
    int startBottom = 5;
    int startTop = 85;

    // this will be the chosen value for the indicated YELLOWor
    // also a couple helpers
    int hue = 0;
    int bottomHue = 0;
    int topHue = 0;

    // scan from ntc up
    for( int i=startBottom; i<startBottom+360; i++ )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnYellow(iter, true);
        if( body.eyes.tryHit(YELLOW) )
        {
            bottomHue = iter;
            break;
        }
    }

    // scan from ntc down
    for( int i=startTop; i>startTop-360; i-- )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnYellow(iter, true);
        if( body.eyes.tryHit(YELLOW) )
        {
            topHue = iter;
            break;
        }
    }

    // split the difference
    hue = getMid(bottomHue, topHue);
    printf( "Yellow is %d\n", hue );

    body.eyes.learnYellow(hue, false);

    return;
}

void Player::learnGreen()
{
    festival_say_text("Please show me Green.");
    Point2D myPoint = Point2D(180.0,227.0);
    body.moveHead(myPoint);
    sleep(5);

    // these are meant to be upper and lower bounds for what the hue might be
    int startBottom = 121;
    int startTop = 181;

    // this will be the chosen value for the indicated GREENor
    // also a couple helpers
    int hue = 0;
    int bottomHue = 0;
    int topHue = 0;

    // scan from ntc up
    for( int i=startBottom; i<startBottom+360; i++ )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnGreen(iter, true);
        if( body.eyes.tryHit(GREEN) )
        {
            bottomHue = iter;
            break;
        }
    }

    // scan from ntc down
    for( int i=startTop; i>startTop-360; i-- )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnGreen(iter, true);
        if( body.eyes.tryHit(GREEN) )
        {
            topHue = iter;
            break;
        }
    }

    // split the difference
    hue = getMid(bottomHue, topHue);
    printf( "Green is %d\n", hue );

    body.eyes.learnGreen(hue, false);

    return;
}

void Player::learnBlue()
{
    festival_say_text("Please show me Blue.");
    Point2D myPoint = Point2D(180.0,227.0);
    body.moveHead(myPoint);
    sleep(5);

    // these are meant to be upper and lower bounds for what the hue might be
    int startBottom = 194;
    int startTop = 254;

    // this will be the chosen value for the indicated BLUEor
    // also a couple helpers
    int hue = 0;
    int bottomHue = 0;
    int topHue = 0;

    // scan from ntc up
    for( int i=startBottom; i<startBottom+360; i++ )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnBlue(iter, true);
        if( body.eyes.tryHit(BLUE) )
        {
            bottomHue = iter;
            break;
        }
    }

    // scan from ntc down
    for( int i=startTop; i>startTop-360; i-- )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnBlue(iter, true);
        if( body.eyes.tryHit(BLUE) )
        {
            topHue = iter;
            break;
        }
    }

    // split the difference
    hue = getMid(bottomHue, topHue);
    printf( "Blue is %d\n", hue );

    body.eyes.learnBlue(hue, false);

    return;
}

void Player::learnPurple()
{
    festival_say_text("Please show me Purple.");
    Point2D myPoint = Point2D(180.0,227.0);
    body.moveHead(myPoint);
    sleep(5);

    // these are meant to be upper and lower bounds for what the hue might be
    int startBottom = 220;
    int startTop = 280;

    // this will be the chosen value for the indicated PURPLEor
    // also a couple helpers
    int hue = 0;
    int bottomHue = 0;
    int topHue = 0;

    // scan from ntc up
    for( int i=startBottom; i<startBottom+360; i++ )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnPurple(iter, true);
        if( body.eyes.tryHit(PURPLE) )
        {
            bottomHue = iter;
            break;
        }
    }

    // scan from ntc down
    for( int i=startTop; i>startTop-360; i-- )
    {
        int iter = i;
        while( iter > 360 )
        {
            iter -= 360;
        }
        while( iter < 0 )
        {
            iter += 360;
        }
        
        body.eyes.learnPurple(iter, true);
        if( body.eyes.tryHit(PURPLE) )
        {
            topHue = iter;
            break;
        }
    }

    // split the difference
    hue = getMid(bottomHue, topHue);
    printf( "Purple is %d\n", hue );

    body.eyes.learnPurple(hue, false);

    return;
}
