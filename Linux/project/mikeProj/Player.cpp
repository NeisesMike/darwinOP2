/*
 * Michael Neises
 * 1 nov 2019
 * player
 */

#include "Player.h"
#include "playerLibrary.cpp"
#include <pthread.h>

Player::Player() : body(Body())// , linux_cm730(LinuxCM730("/dev/ttyUSB0")), cm730(CM730(&linux_cm730)) 
{
    for(int i=0; i<8; i++)
    {
        boardMemory[i] = UNKNOWN;
        boardMatchedMemory[i] = false;
    }

    lastDetected = UNKNOWN;


    // init the voice
    int heap_size = 21000000;  // default scheme heap size
    int load_init_files = 1; // we want the festival init files loaded

    festival_initialize(load_init_files,heap_size);

    // if we don't wait for the spooler, an error will be thrown
    // festival will crash, and anything we want to say
    // will be lost if it's queued after the current output
    festival_wait_for_spooler();

}

Player::~Player()
{
    delete( kinestheticMemory );
}

void Player::debug()
{
    body.eyes.learnRed( 357, false );
    body.eyes.learnBack( 59, false );
    m_debug = true;
    body.m_debug = true;
    body.eyes.m_debug = true;
    return;
}

void Player::learnColors()
{
    body.moveHead(0, -10);
    say("Please help me learn my colors.");

    printf( "\nred\n" );
    learnRed();
    printf( "\ngreen\n" );
    learnGreen();
    printf( "\nblue\n" );
    learnBlue();
    printf( "\nback\n" );
    learnBack();

    body.moveHead(0, 0);
    say("Okay, I'm ready to identify those colors.");
    body.moveHead(0, -30);
    return;
}

void Player::learnCardSize()
{
    //say("I need you to help me learn how big a card is.");

    // to ensure high quality color matching
    int greenHue = learnGreen();

    say("Now, please show me a green card close to me.");
    body.moveHead( 0, -40 );
    sleep(3);

    // learn the max size by growing the match range down from 100
    for( double min=9; min>0; min-=.1 )
    {
        body.eyes.learnCardSize(greenHue, min, 9);
        if( body.eyes.tryHitSize(GREEN) )
        {
            body.eyes.setMaxCardSize(min);
            printf( "max size is %f\n", min );
            break;
        }
    }

    say("Now, please show me a green card away from me.");
    body.moveHead( 0, -35 );
    sleep(3);

    // learn the min size by growing the match range up from 0
    for( double max=0; max<100; max+=.1 )
    {
        body.eyes.learnCardSize(greenHue, 0, max);
        if( body.eyes.tryHitSize(GREEN) )
        {
            body.eyes.setMinCardSize(max);
            printf( "min size is %f\n", max );
            break;
        }
    }
    return;
}

Color Player::getCardAtPosition( int index )
{
    ScanData temp = kinestheticMemory[ index ];

    //check it out
    body.centerGaze( temp );
    while( true )
    {
        ScanData result = body.eyes.maculaLook(30, true);
        if( result.color != UNKNOWN && result.color != BACKSIDE )
        {
            return( result.color );
        }
    }
}

int Player::chooseCard(int partnerCardIndex)
{
    // case: know a match
    for( int i=0; i<100; i++ )
    {
        ScanData thisCard = kinestheticMemory[i];
        if( thisCard.location.X == -1000 )
        {
            printf( "breaking\n" );
            break;
        }
        
        if( i == partnerCardIndex )
        {
            printf( "skipping\n" );
            continue;
        }

        if( thisCard.color == kinestheticMemory[partnerCardIndex].color )
        {
            say( "Ah!" );
            say( "I know." );
            return( i );
        }
    }

    // case: don't know a match
    while( true )
    {
        for( int i=0; i<100; i++ )
        {
            ScanData thisCard = kinestheticMemory[i];
            if( thisCard.location.X == -1000 )
            {
                printf( "breaking\n" );
                break;
            }
            
            if( i == partnerCardIndex )
            {
                printf( "skipping\n" );
                continue;
            }

            if( thisCard.color == UNKNOWN )
            {
                say( "Humm" );
                return( i );
            }
        }
    }
}

// TODO
void Player::indicateChoice( int choice )
{
    ScanData thisCard = kinestheticMemory[choice];

    // turn gaze towards the appropriate card
    body.centerGaze( thisCard );

    // announce when ready
    say( "This card." );

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

int Player::waitForPartner()
{
    // look at all cards in sequence that aren't already matched
    bool isWaiting = true;
    while( isWaiting )
    {
        for( int i=0; i<1000; i++ )
        {
            ScanData temp = kinestheticMemory[i];
            if( temp.location.X == -1000 )
            {
                printf( "reached the end\n" );
                sleep(2);
                break;
            }

            if( temp.isMatched == true )
            {
                printf( "skipping\n");
                continue;
            }

            //check it out
            body.centerGaze( temp );
            usleep(500000);
            ScanData result = body.eyes.maculaLook(30, true);
            if( result.color != UNKNOWN && result.color != BACKSIDE )
            {
                updateMem( i, result.color );
                return( i );
            }
        }
    }
}

void Player::say(const EST_String str)
{
    festival_say_text( str );
    return;
}


void Player::changeGemColor( Color col )
{
    body.changeGemColor( col );
}

void Player::changeEyeColor( Color col )
{
    body.changeEyeColor( col );
}

void Player::interpretColor( int col )
{
    if((col & RED) != 0 && lastDetected != RED)
    {
        lastDetected = RED;
        changeGemColor( RED );
        festival_say_text("red");
    }
    else if((col & GREEN) != 0 &lastDetected != GREEN)
    {
        lastDetected = GREEN;
        changeGemColor( GREEN );
        festival_say_text("green");
    }
    else if((col & BLUE) != 0 &lastDetected != BLUE)
    {
        lastDetected = BLUE;
        changeGemColor( BLUE );
        festival_say_text("blue");
    }
    else if((col & BACKSIDE) != 0 &lastDetected != BACKSIDE)
    {
        lastDetected = BACKSIDE;
        changeGemColor( BACKSIDE );
        festival_say_text("backside");
    }
    return;
}

void Player::observe()
{
    interpretColor(body.eyes.look().color);
    return;
}

void Player::scan()
{
    changeEyeColor( PURPLE );
    kinestheticMemory = body.scan();
    for( int i=0; i<1000; i++ )
    {
        if( kinestheticMemory[i].location.X == -1000 )
        {
            break;
        }
        updateMem( i, false );
        updateMem( i, UNKNOWN );
    }
    return;
}

void Player::statusCheck()
{
    body.statusCheck();
    return;
}

void Player::greet()
{
    changeEyeColor( BLUE );
    body.moveHead(0, 0);
    say("Hello.");
    say("My name is Darwin.");
    return;
}

void Player::cardReport()
{
    changeEyeColor( BLUE );
    int iter = 0;
    // surely there won't be 1000 cards...
    for( int i=0; i<1000; i++ )
    {
        ScanData temp = kinestheticMemory[i];
        if( temp.location.X == -1000 )
        {
            break;
        }
        if( temp.color == UNKNOWN )
        {
            continue;
        }

        //body.centerGaze( temp );
        body.moveHead( temp.pan, temp.tilt );
        body.moveShoulderByGaze( temp.pan, temp.tilt );

        if(temp.color & RED)
        {
            changeGemColor( RED );
            festival_say_text("Here is a red card.");
        }
        else if(temp.color & GREEN)
        {
            changeGemColor( GREEN );
            festival_say_text("Here is a green card.");
        }
        else if(temp.color & BLUE)
        {
            changeGemColor( BLUE );
            festival_say_text("Here is a blue card.");
        }
        else if(temp.color & BACKSIDE)
        {
            changeGemColor( BACKSIDE );
            festival_say_text("Here is an overturned card.");
        }

    }
    body.moveShoulder( true, 90, 60 );
    body.moveShoulder( true, 90, 80 );
    body.moveShoulder( false, 90, 60 );
    body.moveShoulder( false, 90, 80 );
    return;
}

void Player::shoulderTests()
{
    body.moveShoulder( true, 90, 80 );
    body.moveShoulder( false, 90, 80 );

    // try pointing at 6 cards fanned around you
    body.moveShoulderByGaze( -75, -22 );
    sleep(1);
    body.moveShoulderByGaze( -45, -22 );
    sleep(1);
    body.moveShoulderByGaze( -15, -27 );
    sleep(1);
    body.moveShoulderByGaze( 15, -27 );
    sleep(1);
    body.moveShoulderByGaze( 45, -22 );
    sleep(1);
    body.moveShoulderByGaze( 75, -22 );

    return;
}

void Player::nod()
{
    body.moveHead( 0, 0 );
    sleep(1);
    body.moveHead( 0, -15 );
    usleep(50000);
    body.moveHead( 0, 0 );
    return;
}

ScanData Player::getCardAtMem( int index )
{
    return( kinestheticMemory[index] );
}

void Player::updateMem( int index, bool isMatched )
{
    kinestheticMemory[index].isMatched = isMatched;
    return;
}

void Player::updateMem( int index, Color color )
{
    kinestheticMemory[index].color = color;
    return;
}

