/*
 *  Copyright (C) 2011  Regents of the University of Michigan,
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
#include "VcfRecordGenotype.h"


VcfRecordGenotype::VcfRecordGenotype()
{
    reset();
}


VcfRecordGenotype::~VcfRecordGenotype()
{
}


bool VcfRecordGenotype::read(IFILE filePtr)
{
    // Clear out any previously set values.
    reset();
    
    if(ifeof(filePtr))
    {
        // End of file, just return false.
        return(false);
    }
    
    static const std::string fieldStopChars = "\n\t:";

    // The start of the first character in stopChars that means there is more
    // genotype info in the format field, so continue reading the format field.
    static const int contPos = 2;
    static const int tabPos = 1;
    int stopPos = contPos;

    std::string* nextType = NULL;
    // Read/parse the format.
    while(stopPos >= contPos)
    {
        nextType = &(myPosToType.getNextEmpty());
        stopPos = readTilChar(filePtr, fieldStopChars, *nextType);
    }

    // Done reading the format field, so read the samples.
    SmartStringVector* nextSample = NULL;
    while(stopPos == tabPos)
    {
        // Read this sample.
        nextSample = &(mySamples.getNextEmpty());
        stopPos = contPos;
        // Read each data field for the sample.
        while(stopPos >= contPos)
        {
            nextType = &(nextSample->getNextEmpty());
            stopPos = readTilChar(filePtr, fieldStopChars, *nextType);
        }
    }
    
    // Return whether or not a tab was found at the end of the field.
    return(false);
}


bool VcfRecordGenotype::write(IFILE filePtr)
{
    bool status = true;
    
    // Write the format.
    status &= myPosToType.write(filePtr);

    // Write the tab.
    int numWritten = ifprintf(filePtr, "\t");
    
    if(numWritten != 1)
    {
        status = false;
    }

    // Write the samples.
    status &= mySamples.write(filePtr);
    return(status);
}


void VcfRecordGenotype::reset()
{
    myPosToType.reset();
    mySamples.reset();
}


const std::string* VcfRecordGenotype::getValue(const std::string& key, 
                                               int sampleNum)
{
    // Get this sample
    SmartStringVector* samplePtr = mySamples.get(sampleNum);
    // Check if the sample exists.
    if(samplePtr != NULL)
    {
        // the sample was found, so search for this field of the sample.
        std::map<const std::string*, int>::iterator iter =
            myTypeToPos.find(&key);
        if(iter != myTypeToPos.end())
        {
            // Found the field, so get it's value.
            return(samplePtr->get(iter->second));
        }
    }

    // field/sample was not found, so return null.
    return(NULL);
}

const int  VcfRecordGenotype::getNumSamples()
{
    return(mySamples.size());
}


/////////////////////////////////////////////////////////////
// SmartStringVector
bool VcfRecordGenotype::SmartStringVector::write(IFILE filePtr)
{
    int numWritten = 0;
    int numExpected = 0;

    for(unsigned int i = 0;
        i < myNextEmpty; i++)
    {
        if(i == 0)
        {
            // First entry, so no ':'
            numWritten += ifprintf(filePtr, "%s", myCont[i]->c_str());
            numExpected += myCont[i]->size();
        }
        else
        {
            // Not first entry, so use a ':'
            numWritten += ifprintf(filePtr, ":%s", myCont[i]->c_str());
            numExpected += 1 + myCont[i]->size();
        }
    } // End loop through entries.
    return(numWritten == numExpected);
}


/////////////////////////////////////////////////////////////
// SmartVectorOfStringVectors
bool VcfRecordGenotype::SmartVectorOfStringVectors::write(IFILE filePtr)
{
    int numWritten = 0;
    int numExpected = 0;
    bool status = true;
    
    for(unsigned int i = 0;
        i < myNextEmpty; i++)
    {
        if(i != 0)
        {
            // Not first entry, so use a '\t'
            numWritten += ifprintf(filePtr, "\t");
            numExpected += 1;

            status &= myCont[i]->write(filePtr);
        }
    } // End loop through entries.
    return((numWritten == numExpected) && status);
}
