/*
 *  Copyright (C) 2011  Regents of the University of Michigan
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SamRecordBase.h"


//Shift indels to the left or right.
bool SamRecord::shiftIndels(bool left)
{
    // TODO, should a status indicate whether or not a shift was made separate from a failure.

    Cigar* cigar = getCigarInfo();

    if(cigar == NULL)
    {
        return(false);
    }

    if(left)
    {

        // Track where in the read we are.
        uint32_t readPos = 0;

        // Loop through the cigar operations from the 2nd operation since
        // the first operation is already on the end and can't shift.
        for(int i = 1; i < cigar.size(); i++)
        {
            if(cigar[i].operation == Cigar::insert)
            {
                // Loop through shifting until 
                for(int prevOpIndex = i-1; prevOpIndex >= 0; prevOpIndex--)
                {
                    if(!isMatchOrMismatch(cigar[prevOpIndex]))
                    {
                        // Not a match/mismatch, so can't shift into it.
                        break;
                    }
                    
                    // It is a match or mismatch, so check to see if we can
                    // shift into it.

                    // The end of the insert is calculated by adding the size
                    // of this insert minus 1 to the start of the insert.
                    uint32_t insertEndPos = 
                        currentPos + cigar[currentOp].size - 1;
                    // The position before the insert is what we want to compare
                    // to the end of the insert to see if they are the same.
                    // If they are the same, then we can shift.  If not, we
                    // can't shift of the implied reference would change.
                    uint32_t preInsertPos = currentPos - 1;
                    
                    while(sequence[
                        
                }
                // Loop through shifting.

                // This is an insert and it can be shifted.
                
                canShift = false;
            }
            else if((cigar[i].operation == Cigar::match) ||
                    (cigar[i].operation == Cigar::mismatch))
            {
                canShift = true;
            }

            // Check to see if the readPos should be incremented.
            
        }
    }

    // Check to see whether or not the Cigar has already been
    // set - this is determined by checking if alignment length
    // is set since alignment length and the cigar are set
    // at the same time.
    if(myAlignmentLength == -1)
    {
        // Not been set, so calculate it.
        parseCigar();
    }

    // Cigar is set, so now myCigarRoller can be used.
    myCigarRoller.
    if(!myCigarRoller.hasIndel())
    {
        // TODO - what should we return if no shift was required/no change made?
        return(false);
    }

    // This record has indels, so shift them in the correct direction.
    
}


