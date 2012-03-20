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

#include "AspHeader.h"
#include "CharBuffer.h"

AspHeader::AspHeader()
    : myNameToID(),
      myIDToName()
{
    reset();
}


AspHeader::~AspHeader()
{
    reset();
}

bool AspHeader::set(SamFileHeader& samHeader)
{
    reset();
    // Set the header based on the reference info from the
    // SAM/BAM file.
    const SamReferenceInfo* samInfo = samHeader.getReferenceInfo();
    if(samInfo == NULL)
    {
        // Failed to get the info, so could not set.
        return(false);
    }

    // Resize the vector.
    myIDToName.resize(samInfo->getNumEntries());
    // Have the info from the sam file, now copy it.
    for(int i = 0; i < samInfo->getNumEntries(); i++)
    {
        // Get the reference name for this entry.
        const char* refName = samInfo->getReferenceName(i);

        myNameToID[refName] = i;
        myIDToName[i] = refName;
    }
    return(true);
}


bool AspHeader::read(IFILE filePtr)
{
    reset();
    int numChroms;
    // Read the number of chromosomes.
    if(ifread(filePtr, &numChroms, sizeof(int32_t)) != sizeof(int32_t))
    {
        return(false);
    }

    // Resize the vector.
    myIDToName.resize(numChroms);

    CharBuffer refName;

    // Now read the info for each chromosome.
    for(int i = 0; i < numChroms; i++)
    {
        int nameLength;
        // Read the length of the reference name.
        if(ifread(filePtr, &nameLength, sizeof(int32_t)) != sizeof(int32_t))
        {
            return(false);
        }
      
        // Read the name.
        if(refName.readFromFile(filePtr, nameLength) != nameLength)
        {
            return(false);
        }

        myIDToName[i] = refName.c_str();
        myNameToID[refName.c_str()] = i;
    }
    return(true);
}

 
bool AspHeader::write(IFILE outputFile)
{
    // Write the reference names in the id order.
    int32_t numChrom = myIDToName.size();
    if(ifwrite(outputFile, &numChrom, sizeof(int32_t)) != sizeof(int32_t))
    {
        return(false);
    }
    for(int i = 0; i < numChrom; i++)
    {
        // Add one for the null value.
        uint32_t nameLength = myIDToName[i].length() + 1;
        // Write the length of the reference name.
        if(ifwrite(outputFile, &nameLength, sizeof(int32_t)) != sizeof(int32_t))
        {
            return(false);
        }
      
        // Write the name.
        if(ifwrite(outputFile, myIDToName[i].c_str(), nameLength) != nameLength)
        {
            return(false);
        }
    }
    return(true);
}


int32_t AspHeader::getChromID(const char* chromName)
{
    std::map<std::string, int>::iterator chromIter = myNameToID.find(chromName);
    if(chromIter == myNameToID.end())
    {
        return(-1);
    }
    // Return the chrom id.
    return((*chromIter).second);
}

bool AspHeader::getChromName(int32_t chromID, std::string chromName)
{
    if((chromID < 0) || ((uint32_t)chromID >= myIDToName.size()))
    {
        // chromosome id is out of range.
        chromName = "*";
        return(false);
    }
    // Chromosome id is in range, so return the name.
    chromName = myIDToName[chromID];
    return(true);
}

void AspHeader::reset()
{
    myNameToID.clear();
    myIDToName.clear();
}

