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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "AspRecord.h"
#include <stdexcept>
#include "BaseAsciiMap.h"
#include "BaseUtilities.h"

/////////////////////////////////////////////////////////////////////////////
//
// AspRecord
//

IFILE AspRecord::ourOutput = NULL;
const unsigned int AspRecord::REC_TYPE_LEN = 1;
const uint8_t AspRecord::EMPTY_REC = 0x0;
const uint8_t AspRecord::POS_REC = 0x1;
const uint8_t AspRecord::REF_ONLY_REC = 0x2;
const uint8_t AspRecord::DETAILED_REC = 0x3;


AspRecord::AspRecord()
{
    reset();
}


AspRecord::~AspRecord()
{
}


// Add an entry
bool AspRecord::add(char base, char qual, int cycle, bool strand, int mq)
{
    // If se are already at the max number of bases, just return.
    if(myNumBases >= MAX_NUM_BASES)
    {
        return(false);
    }

    // Have not yet incremented myNumBases so as an example:
    // myNumBases = 0; 1st base; basesIndex = 0;
    // myNumBases = 1; 2nd base; basesIndex = 0;
    // myNumBases = 2; 3rd base; basesIndex = 1;
    // myNumBases = 3; 4th base; basesIndex = 1;
    // myNumBases = 4; 5th base; basesIndex = 2;
    int basesIndex = myNumBases/2;
    unsigned char intBase = BaseAsciiMap::base2int[(int)base];
    if((myNumBases % 2) == 0)
    {
        // When myNumBases is a multiple of 2, 
        // we want the upper bits of the index.
        myBases[basesIndex] = (intBase & 0xF) << 4;
    }
    else
    {
        // Not a multiple of 2, so add it to the lower bits.
        myBases[basesIndex] |= (intBase & 0xF);
    }
    int phredQual = BaseUtilities::getPhredBaseQuality(qual);
    myQuals[myNumBases] = phredQual;
    myCycles[myNumBases] = cycle;

    int strandsIndex = myNumBases/8;
    int strandsSubIndex = myNumBases%8;
    if(strandsSubIndex == 0)
    {
        if(strand != 0)
        {
            // Turn on the upper-most bit, default the rest to off.
            myStrands[strandsIndex] = 0x80;
        }
        else
        {
            // The upper-most bit is off, set the rest to off too.
            myStrands[strandsIndex] = 0;
        }
    }
    else if(strand != 0)
    {
        // Set this strand to 1 based on the position in the int.
        if(strandsSubIndex == 1)
        {
            myStrands[strandsIndex] |= 0x40;
        }
        else if(strandsSubIndex == 2)
        {
            myStrands[strandsIndex] |= 0x20;
        }
        else if(strandsSubIndex == 3)
        {
            myStrands[strandsIndex] |= 0x10;
        }
        else if(strandsSubIndex == 4)
        {
            myStrands[strandsIndex] |= 0x8;
        }
        else if(strandsSubIndex == 5)
        {
            myStrands[strandsIndex] |= 0x4;
        }
        else if(strandsSubIndex == 6)
        {
            myStrands[strandsIndex] |= 0x2;
        }
        else if(strandsSubIndex == 7)
        {
            myStrands[strandsIndex] |= 0x1;
        }
        
    }
    myMQs[myNumBases] = mq;
    ++myNumBases;

    // Update the GLH/GLA
    if((phredQual > 13) && 
       (phredQual != BaseUtilities::UNKNOWN_QUALITY_INT))
    {
        myGLH += 3;
        myGLAd += 4.77;
        myGLAd += phredQual;
        myGLAi = (uint8_t) myGLAd;
    }
    return(true);
}



void AspRecord::setEmptyType()
{
    myType = EMPTY_REC;
}


void AspRecord::setPosType()
{
    myType = POS_REC;
}


void AspRecord::setRefOnlyType()
{
    myType = REF_ONLY_REC;
}


void AspRecord::reset()
{
    myType = DETAILED_REC;
    myNumBases = 0;
    myChromID = INVALID_CHROMOSOME_INDEX;
    my0BasedPos = -1;
    myGLH = 0;
    myGLAi = 0;
    myGLAd = 0;
}


bool AspRecord::read(IFILE filePtr, int32_t& chromID, int32_t& pos)
{
    reset();

    if(filePtr->ifeof())
    {
        // End of file, so just return false.
        return(false);
    }

    // Read the first byte that contains the type.
    if(ifread(filePtr, &myType, REC_TYPE_LEN) != REC_TYPE_LEN)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the record type."));
        return(false);
    }

    // Set the chrom/pos for this record, then update the position.
    myChromID = chromID;
    my0BasedPos = pos;
    ++pos;

    bool returnVal = true;
    switch(myType)
    {
        case EMPTY_REC:
            // This is an empty rec which has been entirely read, so just return
            break;
        case POS_REC:
            // Position record type.
            returnVal = readPosRecord(filePtr);
            // Update the passed in chrom/pos to this record's new chrom/pos
            chromID = myChromID;
            pos = my0BasedPos;
            break;
        case REF_ONLY_REC:
            returnVal = readRefOnlyRecord(filePtr);
            break;
        case DETAILED_REC:
            returnVal = readDetailedRecord(filePtr);
            break;
        default:
            throw(std::runtime_error("BamInfoRecord: Invalid record type."));
            returnVal = false;
            break;
    }
    return(returnVal);
}


bool AspRecord::isEmptyType()
{
    return(myType == EMPTY_REC);
}
bool AspRecord::isPosType()
{
    return(myType == POS_REC);
}
bool AspRecord::isRefOnlyType()
{
    return(myType == REF_ONLY_REC);
}
bool AspRecord::isDetailedType()
{
    return(myType == DETAILED_REC);
}


int AspRecord::getGLH()
{
    if(!isRefOnlyType())
    {
        // Not reference only, so return 0.
        return(0);
    }
    else
    {
        return((int)myGLH);
    }
}


int AspRecord::getGLA()
{
    if(!isRefOnlyType())
    {
        // Not reference only, so return 0.
        return(0);
    }
    else
    {
        return((int)myGLAi);
    }
}

int AspRecord::getLikelihood(char base1, char base2, char refBase)
{
    if(isRefOnlyType())
    {
        return(getRefOnlyLikelihood(base1, base2, refBase));
    }
    if(isDetailedType())
    {
        return(getDetailedLikelihood(base1, base2, refBase));
    }
    // Other type, does not have likelihood, so return 0.
    return(0);
}

char AspRecord::getBaseChar(int index)
{
    if((index >= myNumBases) || (index < 0))
    {
        // Invalid index, so just return 'N';
        return('N');
    }
    uint8_t basePair = myBases[index/2];
    char returnVal;
    if((index % 2) == 0)
    {
        // Higher bits, so shift and look up the base.
        returnVal = BaseAsciiMap::int2base[basePair >> 4];
    }
    else
    {
        returnVal = BaseAsciiMap::int2base[basePair & 0xF];
    }
    if(returnVal == 'M')
    {
        return(DELETION_BASE);
    }
    return(returnVal);
}


int AspRecord::getPhredQual(int index)
{
    if((index >= myNumBases) || (index < 0))
    {
        // Invalid index, so just return -1;
        return(-1);
    }
    return(myQuals[index]);
}


char AspRecord::getCharQual(int index)
{
    if((index >= myNumBases) || (index < 0))
    {
        // Invalid index, so just return -1;
        return(' ');
    }
    return(BaseUtilities::getAsciiQuality(myQuals[index]));
}


int AspRecord::getCycle(int index)
{
    if((index >= myNumBases) || (index < 0))
    {
        // Invalid index, so just return -1;
        return(-1);
    }
    return(myCycles[index]);
}


bool AspRecord::getStrand(int index)
{
    if((index >= myNumBases) || (index < 0))
    {
        // Invalid index, so just return false;
        return(false);
    }
    int strandIndex = index/8;
    int strandSubIndex = index%8;

    // Return whether or not the specified bit is 1.
    return((myStrands[strandIndex] >> (7-strandSubIndex)) && 0x1);
}


int AspRecord::getMQ(int index)
{
    if((index >= myNumBases) || (index < 0))
    {
        // Invalid index, so just return -1;
        return(-1);
    }
    return(myMQs[index]);
}


void AspRecord::writeEmpty(IFILE outputFile)
{
    ifwrite(outputFile, &EMPTY_REC, REC_TYPE_LEN);
}


void AspRecord::writePos(int32_t chrom, int32_t pos, IFILE outputFile)
{
    ifwrite(outputFile, &POS_REC, REC_TYPE_LEN);
    ifwrite(outputFile, &chrom, sizeof(int32_t));
    ifwrite(outputFile, &pos, sizeof(int32_t));
}


void AspRecord::write(IFILE outputFile)
{
    switch(myType)
    {
        case EMPTY_REC:
            writeEmpty(outputFile);
            break;
        case POS_REC:
            writePos(myChromID, my0BasedPos, outputFile);
            break;
        case REF_ONLY_REC:
            writeRefOnly(outputFile);
            break;
        case DETAILED_REC:
            writeDetailed(outputFile);
            break;
        default:
            throw(std::runtime_error("BamInfoRecord::write, Invalid record type."));
            break;
    }
}


bool AspRecord::readPosRecord(IFILE filePtr)
{
    // The type has already been read, so read the chrom/pos.
    unsigned int readSize = sizeof(myChromID);
    if(ifread(filePtr, &myChromID, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the chromID from a position record."));
        return(false);
    }

    // Read position
    readSize = sizeof(my0BasedPos);
    if(ifread(filePtr, &my0BasedPos, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the position from a position record."));
        return(false);
    }
    return(true);
}


bool AspRecord::readRefOnlyRecord(IFILE filePtr)
{
    // The type has already been read, so read the number of bases.
    unsigned int readSize = sizeof(myNumBases);
    if(ifread(filePtr, &myNumBases, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the number of bases from a reference only record."));
        return(false);
    }

    // Read GLH
    readSize = 1;
    if(ifread(filePtr, &myGLH, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the GLH from a reference only record."));
        return(false);
    }

    // Read GLA
    if(ifread(filePtr, &myGLAi, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the GLA from a reference only record."));
        return(false);
    }
    return(true);
}


bool AspRecord::readDetailedRecord(IFILE filePtr)
{
    // The type has already been read, so read the number of bases.
    unsigned int readSize = sizeof(myNumBases);
    if(ifread(filePtr, &myNumBases, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the number of bases from a detailed record."));
        return(false);
    }

    // We now have the number of records so can read the remaining fields.
    readSize = getBasesSize();
    if(ifread(filePtr, &myBases, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the bases from a detailed record."));
        return(false);
    }
    
    readSize = myNumBases;
    if(ifread(filePtr, &myQuals, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the qualities from a detailed record."));
        return(false);
    }
    if(ifread(filePtr, &myCycles, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the cycles from a detailed record."));
        return(false);
    }
    readSize = getStrandsSize();
    if(ifread(filePtr, &myStrands, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the strands from a detailed record."));
        return(false);
    }
    readSize = myNumBases;
    if(ifread(filePtr, &myMQs, readSize) != readSize)
    {
        throw(std::runtime_error("BamInfoRecord: Failed reading the MQs from a detailed record."));
        return(false);
    }
    return(true);
}


void AspRecord::writeRefOnly(IFILE outputFile)
{
    ifwrite(outputFile, &REF_ONLY_REC, REC_TYPE_LEN);

    ifwrite(outputFile, &myNumBases, sizeof(myNumBases));
    uint8_t val = (uint8_t)myGLH;
    ifwrite(outputFile, &val, 1);
    ifwrite(outputFile, &myGLAi, 1);
}


void AspRecord::writeDetailed(IFILE outputFile)
{
    ifwrite(outputFile, &DETAILED_REC, REC_TYPE_LEN);

    // Write the number of bases.
    ifwrite(outputFile, &myNumBases, sizeof(myNumBases));
    int basesSize = getBasesSize();
    ifwrite(outputFile, myBases, basesSize);

    ifwrite(outputFile, myQuals, myNumBases);
    ifwrite(outputFile, myCycles, myNumBases);
    ifwrite(outputFile, myStrands, getStrandsSize());
    ifwrite(outputFile, myMQs, myNumBases);
}


int AspRecord::getRefOnlyLikelihood(char base1, char base2, char refBase)
{
    // Check if either base matches the reference.
    if((base1 == refBase) || (base2 == refBase))
    {
        // If both bases match, then it is homozygous reference and
        // return 0.
        if(base1 == base2)
        {
            return(0);
        }
        // The bases are different, so it is Heterzygous Reference
        return(myGLH);
    }
    // Neither base matches the reference, so return alternate.
    return(myGLAi);
}


int AspRecord::getDetailedLikelihood(char base1, char base2, char refBase)
{
    // TBD
    return(0);
}
