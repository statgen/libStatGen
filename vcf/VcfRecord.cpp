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

#include "VcfRecord.h"


VcfRecord::VcfRecord()
{
    reset();
}


VcfRecord::~VcfRecord()
{
}


bool VcfRecord::read(IFILE filePtr, bool siteOnly)
{
    // Clear out any previously set values.
    reset();
    
    if(filePtr == NULL)
    {
        myStatus.setStatus(StatGenStatus::FAIL_ORDER,
                           "Error reading VCF record before opening the file.");
        return(false);
    }

    if(ifeof(filePtr))
    {
        // End of file, just return false.
        return(false);
    }
    
    // Read the chromosome.
    if(!readTilTab(filePtr, myChrom))
    {
        myStatus.setStatus(StatGenStatus::FAIL_PARSE, 
                           "Error reading VCF Record CHROM.");
        return(false);
    }
    // Read the 1-based Position
    if(!readTilTab(filePtr, my1BasedPos))
    {
        myStatus.setStatus(StatGenStatus::FAIL_PARSE, 
                           "Error reading VCF Record POS.");
        return(false);
    }
    else
    {
        // Read the position, so convert to an integer.
        my1BasedPosNum = atoi(my1BasedPos.c_str());
    }
    // Read the ID.
    if(!readTilTab(filePtr, myID))
    {
        myStatus.setStatus(StatGenStatus::FAIL_PARSE, 
                           "Error reading VCF Record ID.");
        return(false);
    }
    // Read the Ref.
    if(!readTilTab(filePtr, myRef))
    {
        myStatus.setStatus(StatGenStatus::FAIL_PARSE, 
                           "Error reading VCF Record REF.");
        return(false);
    }
    // Read the Alt.
    if(!readTilTab(filePtr, myAlt))
    {
        myStatus.setStatus(StatGenStatus::FAIL_PARSE, 
                           "Error reading VCF Record ALT.");
        return(false);
    }
    // Read the Qual.
    if(!readTilTab(filePtr, myQual))
    {
        myStatus.setStatus(StatGenStatus::FAIL_PARSE, 
                           "Error reading VCF Record QUAL.");
        return(false);
    }
    else
    {
        if(myQual != ".")
        {
            // Read the quality, so convert to an integer.
            myQualNum = atof(myQual.c_str());
        }
        else
        {
            myQualNum = -1;
        }
    }
    // Read the Filter.
    if(!readTilTab(filePtr, myFilter))
    {
        myStatus.setStatus(StatGenStatus::FAIL_PARSE, 
                           "Error reading VCF Record FILTER.");
        return(false);
    }
    // Read the Info (could be the last word in the line or file).
    if(!myInfo.read(filePtr))
    {
        // Found the end of the line after the info field, so return true,
        // successfully read the record.
        return(true);
    }

    if(siteOnly)
    {
        // Do not store genotypes, so just consume the rest of the line.
        filePtr->readTilChar("\n");
    }
    else
    {
        // Not yet at the end of the line, so read the genotype fields
        // (format & samples)
        myGenotype.read(filePtr);
    }
    // Found the end of the line, return true since all required fields
    // were read.
    return(true);
}


bool VcfRecord::write(IFILE filePtr, bool siteOnly)
{
    if(filePtr == NULL)
    {
        myStatus.setStatus(StatGenStatus::FAIL_ORDER,
                           "Error writing VCF record before opening the file.");
        return(false);
    }

    int numWritten = ifprintf(filePtr, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t", 
             myChrom.c_str(), my1BasedPos.c_str(), myID.c_str(), myRef.c_str(), myAlt.c_str(), myQual.c_str(), myFilter.c_str());

    // Write the info.
    bool writeSuccess = myInfo.write(filePtr);

    // Add the string lengths and the tabs (7 in between these 8 values).
    int expWritten = myChrom.length() + my1BasedPos.length() + myID.length() + 
        myRef.length() + myAlt.length() + myQual.length() + myFilter.length()+ 
        7;

    // Only write the format & genotype if we are not just writing siteOnly
    // data and there is at least one sample 
    if((!siteOnly) && (myGenotype.getNumSamples() != 0))
    {
        numWritten += ifprintf(filePtr, "\t");
        writeSuccess += myGenotype.write(filePtr);
    }

    // Write the new line.
    numWritten += ifprintf(filePtr, "\n");
    expWritten += 1;

    return((numWritten == expWritten) && writeSuccess);
}



void VcfRecord::reset()
{
    myChrom.clear();
    my1BasedPosNum = 0;
    my1BasedPos.clear();
    myID.clear();
    myRef.clear();
    myAlt.clear();
    myQualNum = 0;;
    myQual.clear();
    myFilter.clear();
    myInfo.clear();
    myGenotype.clear();
    myStatus = StatGenStatus::SUCCESS;
}


// Return the error after a failed call.
const StatGenStatus& VcfRecord::getStatus()
{
    return(myStatus);
}


// bool VcfRecord::isValid()
// {
    
// }


bool VcfRecord::readTilTab(IFILE filePtr, std::string& stringRef)
{
    int charRead = 0;
    while(1)
    {
        charRead = ifgetc(filePtr);
        
        if((charRead == '\n') || (charRead == EOF))
        {
            // Didn't find a tab, found a '\n' or eof
            // It still populated the string with values up
            // until the tab.
            return(false);
        }
        if(charRead == '\t')
        {
            // hit the tab character, so exit the loop.
            break;
        }
        stringRef += charRead;
    }
    return(true);
}
