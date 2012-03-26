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
#include <stdlib.h>

VcfRecordGenotype::VcfRecordGenotype()
{
    reset();
}


VcfRecordGenotype::~VcfRecordGenotype()
{
}


bool VcfRecordGenotype::read(IFILE filePtr)
{
    return(read(filePtr, NULL));
}


bool VcfRecordGenotype::read(IFILE filePtr, VcfSubsetSamples* subsetInfo)
{
    // Clear out any previously set values.
    reset();
    
    if(ifeof(filePtr))
    {
        // End of file, just return false.
        return(false);
    }
    
    static const std::string fieldStopChars = "\n\t:";
    static const std::string discardSampleStopChars = "\n\t";

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
        stopPos = filePtr->readTilChar(fieldStopChars, *nextType);

        // Store map of format to position. 
        // TODO
    }

    // Done reading the format field, so read the samples.
    VCF_SAMPLE* nextSample = NULL;

    int sampleIndex = 0;
    while(stopPos == tabPos)
    {
        // Check if this sample should be kept.
        if(subsetInfo != NULL)
        {
            // Check if this sample should be kept.
            if(!subsetInfo->keep(sampleIndex))
            {
                // this sample should not be kept.
                stopPos = filePtr->readTilChar(discardSampleStopChars);
                ++sampleIndex;
                continue;
            }
        }
        // Read this sample.
        nextSample = &(mySamples.getNextEmpty());
        stopPos = contPos;
        // Read each data field for the sample.
        while(stopPos >= contPos)
        {
            nextType = &(nextSample->getNextEmpty());
            stopPos = filePtr->readTilChar(fieldStopChars, *nextType);
        }
        ++sampleIndex;
    }
    
    // Return whether or not a tab was found at the end of the field.
    return(false);
}


bool VcfRecordGenotype::write(IFILE filePtr)
{
    bool status = true;
    int numWritten = 0;
    int numExpected = 0;

    // Write the format.
    status &= writeSample(filePtr, myPosToType);

    VCF_SAMPLE* sample = NULL;
    
    // Loop through and write each sample.
    for(int i = 0; i < mySamples.size(); i++)
    {
        // Write the tab before this sample.
        numWritten += ifprintf(filePtr, "\t");
        ++numExpected;
        
        sample = &(mySamples.get(i));
        status &= writeSample(filePtr, *sample);
    }

    if(numWritten != numExpected)
    {
        status = false;
    }
    return(status);
}


void VcfRecordGenotype::reset()
{
    myPosToType.reset();
    mySamples.reset();
}


const std::string* VcfRecordGenotype::getString(const std::string& key, 
                                                int sampleNum)
{
    // Get this sample
    if(sampleNum >= mySamples.size())
    {
        // Out of range sample index.
        return(NULL);
    }

    VCF_SAMPLE* samplePtr = &(mySamples.get(sampleNum));

    //  Search for this field of the sample.
    for(int i = 0; i < myPosToType.size(); i++)
    {
        if(myPosToType.get(i) == key)
        {
            // Found the type.
            return(&(samplePtr->get(i)));
        }
    }
    // field was not found, so return null.
    return(NULL);
}


bool VcfRecordGenotype::setString(const std::string& key, 
                                  int sampleNum, 
                                  const std::string& value)
{
    // Get this sample
    VCF_SAMPLE* samplePtr = &(mySamples.get(sampleNum));

    // Search for this field of the sample.
    for(int i = 0; i < myPosToType.size(); i++)
    {
        if(myPosToType.get(i) == key)
        {
            // Found the type, so set it.
            samplePtr->get(i) = value;
            return(true);
        }
    }

    // field was not found, so return false.
    return(false);
}


const int  VcfRecordGenotype::getNumSamples()
{
    return(mySamples.size());
}


bool VcfRecordGenotype::writeSample(IFILE filePtr, 
                                    VCF_SAMPLE& sample)
{
    int numWritten = 0;
    int numExpected = 0;
    std::string* subFieldPtr = NULL;

    for(int i = 0; i < sample.size(); i++)
    {
        subFieldPtr = &(sample.get(i));
        if(i == 0)
        {
            // First entry, so no ':'
            numWritten += ifprintf(filePtr, "%s", subFieldPtr->c_str());
            numExpected += subFieldPtr->size();
        }
        else
        {
            // Not first entry, so use a ':'
            numWritten += ifprintf(filePtr, ":%s", subFieldPtr->c_str());
            numExpected += 1 + subFieldPtr->size();
        }
    } // End loop through entries.
    return(numWritten == numExpected);
}
