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

    /*

    // init the voice
    int heap_size = 21000000;  // default scheme heap size
    int load_init_files = 1; // we want the festival init files loaded

    festival_initialize(load_init_files,heap_size);

    // if we don't wait for the spooler, an error will be thrown
    // festival will crash, and anything we want to say
    // will be lost if it's queued after the current output
    festival_wait_for_spooler();

    */
}

void Player::debug()
{
    body.eyes.learnRed( 340, false );
    return;
}

void Player::learnColors()
{
    body.moveHead(180, 40);
    say("I need you to help me learn my colors.");

    printf( "\nred\n" );
    learnRed();
    /*
    printf( "\norange\n" );
    learnOrange();
    printf( "\nyellow\n" );
    learnYellow();
    */
    printf( "\ngreen\n" );
    learnGreen();
    printf( "\nblue\n" );
    learnBlue();
    /*
    printf( "\npurple\n" );
    learnPurple();
    */

    body.moveHead(180, 40);
    say("Okay, I'm ready to identify those colors.");
    body.moveHead(0, -20);
    return;
}

// TODO
void Player::calibrateVision()
{
    for( int i=0; i<8; i++ )
    {
        kinestheticMemory[i] = 0;
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
    double thisLocation = kinestheticMemory[pos];

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

void Player::interpretColor( int col )
{
    if((col & RED) != 0 && lastDetected != RED)
    {
        lastDetected = RED;
        body.changeGemColor( RED );
        festival_say_text("red");
    }
    else if((col & ORANGE) != 0 &lastDetected != ORANGE)
    {
        lastDetected = ORANGE;
        body.changeGemColor( ORANGE );
        festival_say_text("orange");
    }
    else if((col & YELLOW) != 0 &lastDetected != YELLOW)
    {
        lastDetected = YELLOW;
        body.changeGemColor( YELLOW );
        festival_say_text("yellow");
    }
    else if((col & GREEN) != 0 &lastDetected != GREEN)
    {
        lastDetected = GREEN;
        body.changeGemColor( GREEN );
        festival_say_text("green");
    }
    else if((col & BLUE) != 0 &lastDetected != BLUE)
    {
        lastDetected = BLUE;
        body.changeGemColor( BLUE );
        festival_say_text("blue");
    }
    else if((col & PURPLE) != 0 &lastDetected != PURPLE)
    {
        lastDetected = PURPLE;
        body.changeGemColor( PURPLE );
        festival_say_text("purple");
    }
    return;
}

void Player::observe()
{
    interpretColor(body.eyes.look());
    return;
}

void Player::scan()
{
    //interpretColor(body.eyes.maculaLook());
    body.scan();
    return;
}

void Player::statusCheck()
{
    body.statusCheck();
    return;
}

void Player::greet()
{
    Point2D up = Point2D(180.0, 150.0);
    body.moveHead(up);
    say("Hello, my name is Darwin.");
    return;
}
