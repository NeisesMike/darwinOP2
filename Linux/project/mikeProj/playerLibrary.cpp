int getMid(int bot, int top)
{
    if( bot < top )
    {
        return( (bot+top)/2 );
    }
    return( (bot+top-360)/2 );
}

int Player::learnRed()
{
    say("Please show me RED.");
    body.moveHead( 0, -40 );
    sleep(3);

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

    return(hue);
}

int Player::learnGreen()
{
    say("Please show me Green.");
    body.moveHead( 0, -40 );
    sleep(3);

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

    return(hue);
}

int Player::learnBlue()
{
    say("Please show me Blue.");
    body.moveHead( 0, -40 );
    sleep(3);

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

    return(hue);
}

int Player::learnBack()
{
    say("Please show me the backside color.");
    body.moveHead( 0, -40 );
    sleep(3);

    // these are meant to be upper and lower bounds for what the hue might be
    int startBottom = 25;
    int startTop = 55;

    // this will be the chosen value for the indicated REDor
    // also a couple helpers
    int hue = 0;
    int bottomHue = 0;
    int topHue = 60;

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
        
        body.eyes.learnBack(iter, true);
        if( body.eyes.tryHit(BACKSIDE) )
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
        
        body.eyes.learnBack(iter, true);
        if( body.eyes.tryHit(BACKSIDE) )
        {
            topHue = iter;
            break;
        }
    }

    // split the difference
    hue = getMid(bottomHue, topHue);
    printf( "Backside is %d\n", hue );

    body.eyes.learnBack(hue, false);

    return(hue);
}
