/*
 * Michael Neises
 * 1 nov 2019
 * player
 */
#include "Player.h"

Player::Player()
{
    for(int i=0; i<8; i++)
    {
        boardMemory[i] = UNKNOWN;
    }
}

void Player::init()
{
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

void Player::changeGemColor(CM730* body, Color col )
{
    switch( col )
    {
        case RED:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(255, 0, 0), 0);
            break;
        case GREEN:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(0, 255, 0), 0);
            break;
        case BLUE:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(0, 0, 255), 0);
            break;
        case ORANGE:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(0, 0, 0), 0);
            break;
        case YELLOW:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(255, 128, 0), 0);
            break;
        case PURPLE:
            body->WriteWord(body->ID_CM, body->P_LED_HEAD_L, body->MakeColor(0, 0, 0), 0);
            break;
    }
    return;
}

