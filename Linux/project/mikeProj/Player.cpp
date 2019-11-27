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
    body.moveHead(0, 10);
    say("Please help me learn my colors.");

    printf( "\nred\n" );
    //learnRed();
    printf( "\ngreen\n" );
    //learnGreen();
    printf( "\nblue\n" );
    //learnBlue();
    printf( "\nback\n" );
    learnBack();

    body.moveHead(0, 20);
    say("Okay, I'm ready to identify those colors.");
    body.moveHead(0, -10);
    return;
}

void Player::learnCardSize()
{
    //say("I need you to help me learn how big a card is.");

    // to ensure high quality color matching
    int greenHue = learnGreen();

    say("Now, please show me a green card close to me.");
    body.moveHead( 0, -20 );
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
    body.moveHead( 0, -15 );
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
    //double thisLocation = kinestheticMemory[pos];

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

void Player::say(const EST_String str)
{
    festival_say_text( str );
    return;
}

// TODO
void Player::makeBodyLanguage()
{
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
    return;
}

void Player::statusCheck()
{
    body.statusCheck();
    return;
}

void Player::greet()
{
    changeEyeColor( GREEN );
    body.moveHead(0, 10);
    say("Hello, my name is Darwin.");
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

        body.centerGaze( temp );

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
    return;
}

