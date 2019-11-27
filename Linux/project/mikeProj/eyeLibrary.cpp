
Color getDominantColor( int red, int blue, int green, int backside )
{
    if( red == 0 && blue == 0 && green == 0 && backside == 0 )
    {
        return( UNKNOWN );
    }
    if( red >= blue && red >= green && red >= backside )
    {
        return( RED );
    }
    
    if( blue >= red && blue >= green && blue >= backside )
    {
        return( BLUE );
    }

    if( green >= red && green >= blue && green >= backside )
    {
        return( GREEN );
    }

    if( backside >= red && backside >= blue && backside >= green )
    {
        return( BACKSIDE );
    }

    return( UNKNOWN );
}
