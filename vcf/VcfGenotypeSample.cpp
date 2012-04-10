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

#include "VcfGenotypeSample.h"

VcfGenotypeSample::VcfGenotypeSample()
    : VcfGenotypeField(),
      myFormatPtr(NULL),
      myPhased(false),
      myUnphased(false),
      myHasAllGenotypeAlleles(false)
{
}


VcfGenotypeSample::~VcfGenotypeSample()
{
    myFormatPtr = NULL;
}


bool VcfGenotypeSample::read(IFILE filePtr, VcfGenotypeFormat& format)
{
    static const char* GT_DELIM = "\n\t:|/.";
    static const int END_GT = 2; // Ends at index 2 or less
    static const int PHASED_CHAR_POS = 3;
    static const int UNPHASED_CHAR_POS = 4;
    static const int MISSING_GT_POS = 5;

    // Clear out any previously set values.
    reset();
    
    myFormatPtr = &format;

    int gtIndex = format.getGTIndex();

    // Read the subfields.
    SUBFIELD_READ_STATUS readStatus = MORE_SUBFIELDS;  
    std::string* nextType = NULL;
    int subFieldIndex = 0;
    while(readStatus == MORE_SUBFIELDS)
    {
        // Get the field to write into.
        if(format.storeIndex(subFieldIndex))
        {
            nextType = &(myGenotypeSubFields.getNextEmpty());
            // Check if this is the GT field.
            if(subFieldIndex == gtIndex)
            {
                // There is a GT field, so set that all GT fields are there.
                // if any are missing it will be turned back to false.
                myHasAllGenotypeAlleles = true;
                // This is the GT field, so parse manually looking to see if it
                // is phased and store the genotypes.
                int stopChar = END_GT + 1;
                // Read until a new subfield is found.
                while(stopChar > END_GT)
                {
                    // todo - store the previous nextType len in order to
                    // do string conversion to ints...
                    stopChar = filePtr->readTilChar(GT_DELIM, *nextType);
                    if(stopChar == PHASED_CHAR_POS)
                    {
                        nextType->push_back('|');
                        myPhased = true;
                    }
                    else if(stopChar == UNPHASED_CHAR_POS)
                    {
                        nextType->push_back('/');
                        myUnphased = true;
                    }
                    else if(stopChar == MISSING_GT_POS)
                    {
                        nextType->push_back('.');
                        myHasAllGenotypeAlleles = false;
                    }
                }
            }
            else
            {
                // more subfields to read.
                readStatus = readGenotypeSubField(filePtr, nextType);
            }
        }
        else
        {
            readStatus = readGenotypeSubField(filePtr, NULL);
        }
        ++subFieldIndex;
    }

    // Return true if there is a tab - it is just END_OF_FIELD.
    return(readStatus == END_OF_FIELD);
}


const std::string* VcfGenotypeSample::getString(const std::string& key)
{
    if(myFormatPtr == NULL)
    {
        return(NULL);
    }

    int index = myFormatPtr->getIndex(key);
    if(index != VcfGenotypeFormat::GENOTYPE_INDEX_NA)
    {
        return(&(myGenotypeSubFields.get(index)));
    }
    // key was not found, so return NULL.
    return(NULL);
}


bool VcfGenotypeSample::setString(const std::string& key, const std::string& value)
{
    if(myFormatPtr == NULL)
    {
        return(false);
    }

    int index = myFormatPtr->getIndex(key);
    if(index != VcfGenotypeFormat::GENOTYPE_INDEX_NA)
    {
        // Found the type, so set it.
        myGenotypeSubFields.get(index) = value;
        return(true);
    }
    // field was not found, so return false.
    return(false);
}


void VcfGenotypeSample::internal_reset()
{
    myFormatPtr = NULL;
    myPhased = false;
    myUnphased = false;
    myHasAllGenotypeAlleles = false;
}
