
void Body::scanFilter( ScanData* input, ScanData* output, int percent, bool isFinal )
{
    ScanData stop = {};
    Point2D stopPoint = Point2D(-1000,-1000);
    stop.location = stopPoint;

    output[0] = stop;
    int iter = 0;
    for( int i=0; i>=0; i++ )
    {
        ScanData temp = input[i];
        if( temp.location.X == -1000 )
        {
            break;
        }

        // no choice but to iterate over all matches again :sigh:
        bool doWeAlreadyHaveThisCard = false;
        for( int j=0; j>=0; j++ )
        {
            ScanData innerTemp = output[j];
            if( innerTemp.location.X == -1000 )
            {
                break;
            }

            // only consider cards that have the same color
            if( innerTemp.color != temp.color )
            {
                continue;
            }

            // if the tilts and pans are no more than a few degrees apart, skip
            // -> about 10% of the angular range :shrug:
            // if the x and y coords are within... 10% horiz and vert, skip
            // assume no two cards are this close together
            int panTolerance = 9;
            int tiltTolerance = 5;
            int xTolerance = Camera::WIDTH*0.05;
            int yTolerance = Camera::HEIGHT*0.05;

            // be sure to calc the absolute locations
            double tempX = temp.maculaOrigin.X + temp.location.X;
            double tempY = temp.maculaOrigin.Y + temp.location.Y;
            double innerTempX = innerTemp.maculaOrigin.X + innerTemp.location.X;
            double innerTempY = innerTemp.maculaOrigin.Y + innerTemp.location.Y;

            bool isPanMatched = (innerTemp.pan-panTolerance < temp.pan) && temp.pan < (innerTemp.pan + panTolerance);
            bool isTiltMatched = (innerTemp.tilt-tiltTolerance < temp.tilt) && temp.tilt < (innerTemp.tilt+tiltTolerance);
            bool isXMatched = (innerTempX-xTolerance < tempX) && (tempX < innerTempX+xTolerance);
            bool isYMatched = (innerTempY-yTolerance < tempY) && (tempY < innerTempY+yTolerance);

            if( isPanMatched && isTiltMatched && (isXMatched && isYMatched || isFinal) )
            {
                // skip this one!
                doWeAlreadyHaveThisCard = true;
                break;
            }

            // otherwise, keep considering it!
            doWeAlreadyHaveThisCard = false;
        }
        if( !doWeAlreadyHaveThisCard )
        {
            output[iter] = temp;
            iter++;
            output[iter] = stop;
        }
    }
    return;
}

void Body::reorient( ScanData* input, ScanData* output, int percent )
{
    ScanData stop = {};
    Point2D stopPoint = Point2D(-1000,-1000);
    stop.location = stopPoint;

    int iter = 0;
    for( int i=0; i>=0; i++ )
    {
        ScanData temp = input[i];
        if( temp.location.X == -1000 )
        {
            break;
        }
        if( temp.color != UNKNOWN )
        {
            // center the gaze
            ScanData pantilt = centerGaze( temp );

            // grow the macula
            ScanData result = eyes.growMacula();
            result.pan = pantilt.pan;
            result.tilt = pantilt.tilt;

            output[iter] = result;
            iter++;
        }
    }
    output[iter] = stop;
}

void Body::reorientFilter( ScanData* input, ScanData* output, int percent, int numPossible, bool isFinal )
{
    ScanData* intermediate = (ScanData*)malloc(sizeof(ScanData)*numPossible);
    reorient( input, intermediate, percent );
    scanFilter( intermediate, output, percent, isFinal );
    delete( intermediate );
}

