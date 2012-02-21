/*
 *  Copyright (C) 2012  Regents of the University of Michigan
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

#include "SamRecordHelper.h"
#include <stdexcept>

bool SamRecordHelper::checkSequence(SamRecord& record, int32_t pos0Based, 
                                    const char* sequence)
{
    const char* readSeq = record.getSequence();

    // Get the cigar.
    Cigar* cigar = record.getCigarInfo();

    if(cigar == NULL)
    {
        throw std::runtime_error("Failed to get Cigar.");
    }

    int32_t readStartIndex = 
        cigar->getQueryIndex(pos0Based, record.get0BasedPosition());

    // if the read start is negative, this position was deleted, so 
    // return false, it doesn't match.
    if(readStartIndex == Cigar::INDEX_NA)
    {
        return(false);
    }

    // Increment the readSeq start to where this position is found.
    readSeq += readStartIndex;
    int cmpReturn = strncmp(readSeq, sequence, strlen(sequence));
    // cmpReturn is 0 if the strings match.
    return(cmpReturn == 0);
}
