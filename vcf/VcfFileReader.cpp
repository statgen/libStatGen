/*
 *  Copyright (C) 2010-2011  Regents of the University of Michigan,
 *                           Hyun Min Kang, Matthew Flickenger, Matthew Snyder,
 *                           and Goncalo Abecasis
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

#include "VcfFileReader.h"

VcfFileReader::VcfFileReader()
    : VcfFile(),
      mySampleSubset(),
      myUseSubset(false),
      myDiscardRules(0),
      myNumKeptRecords(0)
{
  myFilePtr = NULL;
}


VcfFileReader::~VcfFileReader() 
{
}


bool VcfFileReader::open(const char* filename, VcfHeader& header)
{
    // Close an already open file.
    close();

    myStatus = StatGenStatus::SUCCESS;
    if(VcfFile::open(filename, "r"))
    {
        // Successfully opened, so read the header.
        if(!header.read(myFilePtr))
        {
            // Failed, so copy the status.
            myStatus = header.getStatus();
            return(false);
        }
    }
    else
    {
        // Failed, status set by VcfFile::open.
        return(false);
    }

    // Successfully opened and read the header.
    return(true);
}


bool VcfFileReader::open(const char* filename, VcfHeader& header,
                         const char* sampleFileName, const char* delims)
{
    if(!open(filename, header))
    {
        // Failed to open & read header, so return.
        return(false);
    }

    // Successfully opened and read the header, so setup the sample subset
    // object based on the specified sample file and the header.
    if(!mySampleSubset.init(header, sampleFileName, delims))
    {
        // Failed to setup the subsetting.
        std::cerr << "VcfFileReader - failed to setup sample subsetting\n";
    }

    myUseSubset = true;

    // Successfully opened and read the header.
    return(true);
}


bool VcfFileReader::readRecord(VcfRecord& record)
{
    myStatus = StatGenStatus::SUCCESS;
    // Subset the read if there are subsets specified.
    VcfSubsetSamples* subsetPtr = NULL;
    if(myUseSubset)
    {
        subsetPtr = &mySampleSubset;
    }

    // Keep looping until a desired record is found.
    bool recordFound = false;
    while(!recordFound)
    {
        if(!record.read(myFilePtr, mySiteOnly, subsetPtr))
        {
            myStatus = record.getStatus();
            return(false);
        }

        ++myNumRecords;
        
        // Record successfully read, so check to see if it is discarded.
        if((myDiscardRules & DISCARD_NON_PHASED) && !record.allPhased())
        {
            // Not all samples are phased, so discard this record.
            continue;
        }
        if((myDiscardRules & DISCARD_MISSING_GT) &&
           !record.hasAllGenotypeAlleles())
        {
            // discard missing GTs and this record had missing alleles,
            // so keep reading.
            continue;
        }
        if((myDiscardRules & DISCARD_FILTERED) && 
           !(record.getFilter().passedAllFilters()))
        {
            // Record was filtered, so discard it.
            continue;
        }
        if((myDiscardRules & DISCARD_MULTIPLE_ALTS) &&
           (record.getNumAlts() > 1))
        {
            // Record had multiple alternates, so discard.
            continue;
        }

        // Record was not discarded.
        recordFound = true;
    }

    // Increment the number of kept records.
    ++myNumKeptRecords;
    return(true);
}


// Returns whether or not the end of the file has been reached.
// return: int - true = EOF; false = not eof.
bool VcfFileReader::isEOF()
{
    if (myFilePtr != NULL)
    {
        // File Pointer is set, so return if eof.
        return(ifeof(myFilePtr));
    }
    // File pointer is not set, so return true, eof.
    return true;
}


void VcfFileReader::resetFile()
{
    mySampleSubset.reset();
    myUseSubset = false;
    myNumKeptRecords = 0;
}
