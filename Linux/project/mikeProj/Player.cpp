/*
 * Michael Neises
 * 1 nov 2019
 * player
 */

#include "Player.h"
#include "playerLibrary.cpp"

Player::Player() : body(Body())// , linux_cm730(LinuxCM730("/dev/ttyUSB0")), cm730(CM730(&linux_cm730)) 
{
    for(int i=0; i<8; i++)
    {
        boardMemory[i] = UNKNOWN;
    }

    lastDetected = UNKNOWN;

    // init the voice
    int heap_size = 310000;  // default scheme heap size
    int load_init_files = 1; // we want the festival init files loaded

    festival_initialize(load_init_files,heap_size);

    // if we don't wait for the spooler, an error will be thrown
    // festival will crash, and anything we want to say
    // will be lost if it's queued after the current output
    festival_wait_for_spooler();

    // Say simple file
    //festival_say_file("/etc/motd");

    //festival_eval_command("(voice_ked_diphone)");
    // Say some text;
    festival_say_text("hello world");
}

void Player::learnColors()
{
    Point2D down = Point2D(180.0,20.0);

    body.moveHead(down);
    festival_say_text("I need you to help me learn my colors.");
    printf( "\nred\n" );
    learnRed();
    body.moveHead(down);
    printf( "\norange\n" );
    learnOrange();
    body.moveHead(down);
    printf( "\nyellow\n" );
    learnYellow();
    body.moveHead(down);
    printf( "\ngreen\n" );
    learnGreen();
    body.moveHead(down);
    printf( "\nblue\n" );
    learnBlue();
    body.moveHead(down);
    printf( "\npurple\n" );
    learnPurple();
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

// TODO
void Player::makeUtterance()
{
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

void Player::scan()
{
    int detected_color = body.eyes.look();
    if((detected_color & RED) != 0 && lastDetected != RED)
    {
        lastDetected = RED;
        body.changeGemColor( RED );
        festival_say_text("red");
    }
    else if((detected_color & ORANGE) != 0 &lastDetected != ORANGE)
    {
        lastDetected = ORANGE;
        body.changeGemColor( ORANGE );
        festival_say_text("orange");
    }
    else if((detected_color & YELLOW) != 0 &lastDetected != YELLOW)
    {
        lastDetected = YELLOW;
        body.changeGemColor( YELLOW );
        festival_say_text("yellow");
    }
    else if((detected_color & GREEN) != 0 &lastDetected != GREEN)
    {
        lastDetected = GREEN;
        body.changeGemColor( GREEN );
        festival_say_text("green");
    }
    else if((detected_color & BLUE) != 0 &lastDetected != BLUE)
    {
        lastDetected = BLUE;
        body.changeGemColor( BLUE );
        festival_say_text("blue");
    }
    else if((detected_color & PURPLE) != 0 &lastDetected != PURPLE)
    {
        lastDetected = PURPLE;
        body.changeGemColor( PURPLE );
        festival_say_text("purple");
    }
}

void Player::statusCheck()
{
    body.statusCheck();
    return;
}
