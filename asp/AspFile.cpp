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

#include "AspFile.h"
#include <stdexcept>

int AspFileWriter::myGapSize = AspFileWriter::DEFAULT_GAP_SIZE;

AspFile::AspFile()
    : myFilePtr(NULL)
{
    reset();
}


AspFile::~AspFile()
{
    close();
}


void AspFile::close()
{
    if(myFilePtr != NULL)
    {
        ifclose(myFilePtr);
        myFilePtr = NULL;
    }
}


bool AspFile::isOpen()
{
    if (myFilePtr != NULL)
    {
        // File Pointer is set, so return if it is open.
        return(myFilePtr->isOpen());
    }
    // File pointer is not set, so return false, not open.
    return false;
}


bool AspFile::open(const char* fileName, const char* mode)
{
    // Close any already open file.
    close();

    myFilePtr = ifopen(fileName, mode, InputFile::BGZF);
    if(myFilePtr == NULL)
    {
        std::string errorMessage = "Failed to Open ";
        errorMessage += fileName;
        throw(std::runtime_error(errorMessage.c_str()));
        return(false);
    }

    return(true);
}


void AspFile::reset()
{
    // Close the file.
    close();
    myNumPosRecs = 0;
    myNumEmptyRecs = 0;
    myNumRefOnlyRecs = 0;
    myNumDetailedRecs = 0;
    myNumUnknownRecs = 0;
}


void AspFile::updateRecordCount(AspRecord& record)
{
    if(record.isEmptyType())
    {
        ++myNumEmptyRecs;
    }
    else if(record.isPosType())
    {
        ++myNumPosRecs;
    }
    else if(record.isRefOnlyType())
    {
        ++myNumRefOnlyRecs;
    }
    else if(record.isDetailedType())
    {
        ++myNumDetailedRecs;
    }
    else
    {
        ++myNumUnknownRecs;
    }
}


AspFileReader::AspFileReader()
    : AspFile()
{
    reset();
}


AspFileReader::~AspFileReader()
{
}


bool AspFileReader::open(const char* fileName)
{
    // Open the file.
    if(!AspFile::open(fileName, "r"))
    {
        return(false);
    }

    return(myHeader.read(myFilePtr));
}


bool AspFileReader::isEof()
{
    if(isOpen())
    {
        // File is open, so return if eof.
        return(ifeof(myFilePtr));
    }
    // File is not open, so return true, eof.
    return true;
}


bool AspFileReader::getNextRecord(AspRecord& rec)
{
    if(!rec.read(myFilePtr, prevChrom, prevPos))
    {
        return(false);
    }

    // Update the record count.
    updateRecordCount(rec);

    return(true);
}


bool AspFileReader::getNextDataRecord(AspRecord& rec)
{
    // Read as long as successful and not the end of the file.
    // The loop will be stopped when a data record is found.
    while(!isEof() && (rec.read(myFilePtr, prevChrom, prevPos)))
    {
        // Update the record count.
        updateRecordCount(rec);

        if(rec.isRefOnlyType() || rec.isDetailedType())
        {
            // Found a data record.
            return(true);
        }
    }
    return(false);
}


const AspRecord* AspFileReader::getRefOnlyRecord(const char* chromName,
                                                 int32_t pos0Based)
{
    if(!advanceToPos(chromName, pos0Based))
    {
        // Position does not have data, so return NULL.
        return(NULL);
    }

    // Check if the record is a detailed record.
    if(!myStoredRecord.isDetailedType())
    {
        // Not a detailed record, so return NULL.
        return(NULL);
    }
    return(&myStoredRecord);
}


const AspRecord* AspFileReader::getDetailedRecord(const char* chromName,
                                                  int32_t pos0Based)
{
    if(!advanceToPos(chromName, pos0Based))
    {
        // Position does not have data, so return NULL.
        return(NULL);
    }

    // Check if the record is a detailed record.
    if(!myStoredRecord.isDetailedType())
    {
        // Not a detailed record, so return NULL.
        return(NULL);
    }
    return(&myStoredRecord);
}


int AspFileReader::getLikelihood(const char* chromName, int32_t pos0Based,
                                 char base1, char base2, char refBase)
{
    static const int UNKNOWN_LIKELIHOOD = 0;

    if(!advanceToPos(chromName, pos0Based))
    {
        // Position does not have data, so return nothing.
        return(UNKNOWN_LIKELIHOOD);
    }
    // Data was found for this position, so return the likelihood.
    return(myStoredRecord.getLikelihood(base1, base2, refBase));
}


int AspFileReader::getNumBases(const char* chromName, int32_t pos0Based)
{
    if(!advanceToPos(chromName, pos0Based))
    {
        // Position does not have data, so return no bases.
        return(0);
    }
    // Data was found for this position, so return the number of bases.
    return(myStoredRecord.getNumBases());
}


void AspFileReader::reset()
{
    AspFile::reset();

    prevChrom = -1;
    prevPos = -1;
}


bool AspFileReader::advanceToPos(const char* chromName, int32_t pos0Based)
{
    // Convert the chromName to chromosome ID.
    int chromID = myHeader.getChromID(chromName);

    // Advance until this position is found or passed.
    while((chromID > myStoredRecord.getChromID()) ||
          ((chromID == myStoredRecord.getChromID()) && 
           (pos0Based > myStoredRecord.get0BasedPos())))
    {
        if(!myStoredRecord.read(myFilePtr, prevChrom, prevPos))
        {
            // failed to read, so return that the likelihood is unknown.
            return(false);
        }
        // Update the record count.
        updateRecordCount(myStoredRecord);
    }

    // Read until a data record is found.
    while(!myStoredRecord.isRefOnlyType() && !myStoredRecord.isDetailedType())
    {
        if(!myStoredRecord.read(myFilePtr, prevChrom, prevPos))
        {
            // failed to read, so return that the likelihood is unknown.
            return(false);
        }
        // Update the record count.
        updateRecordCount(myStoredRecord);
    }

    // Check if the position was found.
    if((chromID != myStoredRecord.getChromID()) ||
       (pos0Based != myStoredRecord.get0BasedPos()))
    {
        // not the correct position.
        return(false);
    }

    return(true);
}


AspFileWriter::AspFileWriter()
    : AspFile()
{
    reset();
}


AspFileWriter::~AspFileWriter()
{
}


bool AspFileWriter::open(const char* fileName, SamFileHeader samHeader)
{
    // Open the file.
    if(!AspFile::open(fileName, "w"))
    {
        return(false);
    }

    myHeader.set(samHeader);
    return(myHeader.write(myFilePtr));
}


void AspFileWriter::setGapSize(int gapSize)
{
    myGapSize = gapSize;
}


bool AspFileWriter::write(AspRecord& record, 
                          const char* chromName, int32_t pos0Based)
{
    // if not opened or the header has not written, exit without writing.
    if(!isOpen())
    {
        std::cerr << "Failed to write an AspRecord because the file is not open.\n";
        return(false);
    }

    int32_t chromID = myPrevChromID;
    // Check if the chromosome name changed.
    if(myPrevChromName != chromName)
    {
        chromID = myHeader.getChromID(chromName);
    }

    // Check to see if empty or position records need to be written
    // by checking the previous position.
    int posDiff = pos0Based - myPrevPos;
    if(myPrevChromID != chromID || posDiff > myGapSize)
    {
        // Write a new position record.
        AspRecord::writePos(chromID, pos0Based, myFilePtr);
        ++myNumPosRecs;
    }
    else if(posDiff < 0)
    {
        // Position is going back.  This is an error.
        std::cerr << "AspFileWriter trying to write out of order!\n";
        return(false);
    }
    else
    {
        // Write empty records to fill in the gap to this position.
        while(posDiff > 1)
        {
            AspRecord::writeEmpty(myFilePtr);
            --posDiff;
            ++myNumEmptyRecs;
        }
    }

    // Write this record.
    record.write(myFilePtr);
    
    if(record.isRefOnlyType())
    {
        ++myNumRefOnlyRecs;
    }
    else if(record.isDetailedType())
    {
        ++myNumDetailedRecs;
    }
    else
    {
        ++myNumUnknownRecs;
    }

   if(chromID != myPrevChromID)
   {
       myPrevChromID = chromID;
       myPrevChromName = chromName;
   }
   myPrevPos = pos0Based;
   return(true);
}


void AspFileWriter::reset()
{
    AspFile::reset();

    myPrevChromName.clear();
    myPrevChromID = -1;
    myPrevPos = -1;
}


