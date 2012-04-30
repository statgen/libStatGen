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
#include "BaseUtilities.h"

/////////////////////////////////////////////////////////////////////////////
//
// AspRecord
//

bool AspRecord::ourIncludeNinDetailed = true;
const unsigned int AspRecord::REF_TYPE_LEN = 1;
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
bool AspRecord::add(char refBase, char base, char qual,
                    int cycle, bool strand, int mq)
{
    // If we are already at the max number of bases, just return.
    if(myNumBases >= MAX_NUM_BASES)
    {
        ++myOverMaxNumBases;
        return(false);
    }

    // Check if the base is 'N' and it is being ignored.
    if(!ourIncludeNinDetailed && (base == 'N'))
    {
        return(false);
    }

    myRefBase = refBase;

    myBases[myNumBases] = BaseAsciiMap::base2int[(int)base];
    int phredQual = BaseUtilities::getPhredBaseQuality(qual);
    myQuals[myNumBases] = phredQual;
    myCycles[myNumBases] = cycle;
    myStrands[myNumBases] = strand;
    
    myMQs[myNumBases] = mq;
    ++myNumBases;

    myNumNonNBasesSet = true;
    if(!BaseUtilities::isAmbiguous(base))
    {
        ++myNumNonNBases;
        // Update the GLH/GLA only if not 'N'
        if((phredQual > 13) && 
           (phredQual != BaseUtilities::UNKNOWN_QUALITY_INT))
        {
            myGLH += 3;
            myGLAd += 4.77;
            myGLAd += phredQual;
            myGLAi = (uint8_t) myGLAd;
        }
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


void AspRecord::setDetailedType()
{
    myType = DETAILED_REC;
}


void AspRecord::reset()
{
    myType = DETAILED_REC;
    myRefBase = 'N';
    myNumBases = 0;
    myNumNonNBases = 0;
    myNumNonNBasesSet = false;
    myOverMaxNumBases = 0;
    myChromID = INVALID_CHROMOSOME_INDEX;
    my0BasedPos = -1;
    myGLH = 0;
    myGLAi = 0;
    myGLAd = 0;
}


bool AspRecord::read(IFILE filePtr, int32_t& chromID, int32_t& pos)
{
    reset();

    if(ifeof(filePtr))
    {
        // End of file, so just return false.
        return(false);
    }

    // Read the first byte that contains the type.
    int8_t refBaseType = 0;
    if(ifread(filePtr, &refBaseType, REF_TYPE_LEN) != REF_TYPE_LEN)
    {
        throw(std::runtime_error("AspRecord: Failed reading the record type."));
        return(false);
    }
    // Extract the type.
    myType = refBaseType & REC_TYPE_MASK;
    // Extract and convert the reference base.
    myRefBase = BaseAsciiMap::int2base[refBaseType >> BASE_SHIFT];
    if(myRefBase == 'M')
    {
        myRefBase = DELETION_BASE;
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
            throw(std::runtime_error("AspRecord: Invalid record type."));
            returnVal = false;
            break;
    }
    return(returnVal);
}


char AspRecord::getRefBase()
{
    if(!isRefOnlyType() && !isDetailedType())
    {
        // Not a data record, so return 'N'
        std::cerr << "AspRecord: requested the reference base for a non"
                  << "data record, so returning 'N'\n";
        return('N');
    }
    return(myRefBase);
}


int AspRecord::getNumNonNBases()
{
    if(!myNumNonNBasesSet)
    {
        // It would only be not set if this is a detailed record
        // that was read from a file - so the bases should be set.
        myNumNonNBases = 0;
        // Number of non-N bases is not set, so set it.
        for(int i = 0; i < myNumBases; i++)
        {
            if(!BaseUtilities::isAmbiguous(getBaseChar(i)))
            {
                // Not an 'N'.
                ++myNumNonNBases;
            }
        }
        myNumNonNBasesSet = true;
    }
    return(myNumNonNBases);
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

int AspRecord::getLikelihood(char base1, char base2)
{
    if(isRefOnlyType())
    {
        return(getRefOnlyLikelihood(base1, base2));
    }
    if(isDetailedType())
    {
        return(getDetailedLikelihood(base1, base2));
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
    char returnVal;
    returnVal = BaseAsciiMap::int2base[myBases[index]];
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
    // Return whether or not the specified bit is 1.
    return(myStrands[index]);
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
    // An empty record has no reference base, so no need to add that.
    if(ifwrite(outputFile, &EMPTY_REC, REF_TYPE_LEN) != REF_TYPE_LEN)
    {
        throw(std::runtime_error("AspRecord: Failed writing an empty record."));
    }
}


void AspRecord::writePos(int32_t chrom, int32_t pos, IFILE outputFile)
{
    // An position record has no reference base, so no need to add that.
    if(ifwrite(outputFile, &POS_REC, REF_TYPE_LEN) != REF_TYPE_LEN)
    {
        throw(std::runtime_error("AspRecord: Failed writing a position record."));
    };
    if(ifwrite(outputFile, &chrom, sizeof(int32_t)) != sizeof(int32_t))
    {
        throw(std::runtime_error("AspRecord: Failed writing the chromID to a position record."));
    };
    if(ifwrite(outputFile, &pos, sizeof(int32_t)) != sizeof(int32_t))
    {
        throw(std::runtime_error("AspRecord: Failed writing the position to a position record."));
    };
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
            throw(std::runtime_error("AspRecord::write, Invalid record type."));
            break;
    }
}


bool AspRecord::readPosRecord(IFILE filePtr)
{
    // The type has already been read, so read the chrom/pos.
    unsigned int readSize = sizeof(myChromID);
    if(ifread(filePtr, &myChromID, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the chromID from a position record."));
        return(false);
    }

    // Read position
    readSize = sizeof(my0BasedPos);
    if(ifread(filePtr, &my0BasedPos, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the position from a position record."));
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
        throw(std::runtime_error("AspRecord: Failed reading the number of bases from a reference only record."));
        return(false);
    }
    // Ref-only does not include any 'N's.
    myNumNonNBases = myNumBases;
    myNumNonNBasesSet = true;

    // Read GLH
    readSize = 1;
    if(ifread(filePtr, &myGLH, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the GLH from a reference only record."));
        return(false);
    }

    // Read GLA
    if(ifread(filePtr, &myGLAi, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the GLA from a reference only record."));
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
        throw(std::runtime_error("AspRecord: Failed reading the number of bases from a detailed record."));
        return(false);
    }

    // We now have the number of records so can read the remaining fields.
    readSize = myNumBases;
    if(ifread(filePtr, &myBases, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the bases from a detailed record."));
        return(false);
    }
    if(ifread(filePtr, &myQuals, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the qualities from a detailed record."));
        return(false);
    }
    if(ifread(filePtr, &myCycles, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the cycles from a detailed record."));
        return(false);
    }
    if(ifread(filePtr, &myStrands, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the strands from a detailed record."));
        return(false);
    }
    readSize = myNumBases;
    if(ifread(filePtr, &myMQs, readSize) != readSize)
    {
        throw(std::runtime_error("AspRecord: Failed reading the MQs from a detailed record."));
        return(false);
    }
    return(true);
}


void AspRecord::writeRefOnly(IFILE outputFile)
{
    int8_t numBases = myNumNonNBases;
    if(!myNumNonNBasesSet)
    {
        numBases = myNumBases;
    }

    if(numBases == 0)
    {
        // No bases, so return without writing.
        return;
    }

    myType = REF_ONLY_REC;
    uint8_t refBaseType = getRefBaseType();
    
    if(ifwrite(outputFile, &refBaseType, REF_TYPE_LEN) != REF_TYPE_LEN)
    {
        throw(std::runtime_error("AspRecord: Failed writing a reference only record."));
    }

    if(ifwrite(outputFile, &numBases, sizeof(numBases)) != 
       sizeof(numBases))
    {
        throw(std::runtime_error("AspRecord: Failed writing numBases to a reference only record."));
    }
    uint8_t val = (uint8_t)myGLH;
    if(ifwrite(outputFile, &val, 1) != 1)
    {
        throw(std::runtime_error("AspRecord: Failed writing GLH to a reference only record."));
    }
    if(ifwrite(outputFile, &myGLAi, 1) != 1)
    {
        throw(std::runtime_error("AspRecord: Failed writing GLA to a reference only record."));
    }
}


void AspRecord::writeDetailed(IFILE outputFile)
{
    // If the number of Non-N bases is set, check if there are any, if not,
    // don't write a record.
    if(myNumNonNBasesSet && (myNumNonNBases == 0))
    {
        // no non-N bases, so return without writing.
        return;
    }

    // Do not worry about num Non-N Bases since if they are not supposed to
    // be included in the DETAILED count, they would have been skipped when
    // added.
    if(myNumBases == 0)
    {
        // No bases or no bases that aren't 'N', so return without writing.
        return;
    }

    myType = DETAILED_REC;
    uint8_t refBaseType = getRefBaseType();
    
    if(ifwrite(outputFile, &refBaseType, REF_TYPE_LEN) != REF_TYPE_LEN)
    {
        throw(std::runtime_error("AspRecord: Failed writing a detailed record."));
    }

    // Write the number of bases.
    if(ifwrite(outputFile, &myNumBases, sizeof(myNumBases)) != 
       sizeof(myNumBases))
    {
        throw(std::runtime_error("AspRecord: Failed writing num bases to a detailed record."));
    }

    if(ifwrite(outputFile, myBases, myNumBases) != myNumBases)
    {
        throw(std::runtime_error("AspRecord: Failed writing bases to a detailed record."));
    }

    if(ifwrite(outputFile, myQuals, myNumBases) != myNumBases)
    {
        throw(std::runtime_error("AspRecord: Failed writing qualities to a detailed record."));
    }
    if(ifwrite(outputFile, myCycles, myNumBases) != myNumBases)
    {
        throw(std::runtime_error("AspRecord: Failed writing cycles to a detailed record."));
    }
    if(ifwrite(outputFile, myStrands, myNumBases) != myNumBases)
    {
        throw(std::runtime_error("AspRecord: Failed writing strands to a detailed record."));
    }
    if(ifwrite(outputFile, myMQs, myNumBases) != myNumBases)
    {
        throw(std::runtime_error("AspRecord: Failed writing MQs to a detailed record."));
    }
}


int AspRecord::getRefOnlyLikelihood(char base1, char base2)
{
    // Check if either base matches the reference.
    if((base1 == myRefBase) || (base2 == myRefBase))
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


int AspRecord::getDetailedLikelihood(char base1, char base2)
{
    // TBD
    return(0);
}
