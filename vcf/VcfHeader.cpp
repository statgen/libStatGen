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

#include "VcfHeader.h"

VcfHeader::VcfHeader()
    : myHeaderLines()
{
    reset();
}


VcfHeader::~VcfHeader()
{
}


bool VcfHeader::read(IFILE filePtr)
{
    if(myHasHeaderLine)
    {
        // Already set the header line, so can't add any more lines.
        myStatus.setStatus(StatGenStatus::FAIL_ORDER, 
                           "Error reading VCF Meta/Header, Header line has already been set, so can't read more lines from a file.");
        return(false);
    }
    if(filePtr == NULL)
    {
        // No file was passed in.
        myStatus.setStatus(StatGenStatus::FAIL_ORDER, 
                           "Need to pass in an open file ptr to VcfHeader::read.");
        return(false);
    }

    // Read until the header line has been read (after the meta lines).
    while(!myHasHeaderLine)
    {
        // Increase the size of headerlines by 1 to fit the new line.
        myHeaderLines.resize(myHeaderLines.size() + 1);
        
        // Read the next line from the file into the header structure.
        String& newStr = myHeaderLines.back();
        if(newStr.ReadLine(filePtr) < 0)
        {
            // Error, unable to read an entire header from the file.
            myStatus.setStatus(StatGenStatus::INVALID, 
                               "Error reading VCF Meta/Header, EOF found before the header line.");
            return(false);
        }
        if(newStr.Length() <= 2)
        {
            // A header/meta line must have at least 2 characters
            // ## or # and 8 fields, so if less than 2 characters,
            // error.
            myStatus.setStatus(StatGenStatus::INVALID, 
                               "Error reading VCF Meta/Header, line without at least 2 characters found before the header line.");
            return(false);
        }

        // Check if it is a header (first char is # and 2nd one is not).
        if((newStr[0] == '#') && (newStr[1] != '#'))
        {
            myHasHeaderLine = true;
            // Parse the header line to get the samples.
            myParsedHeaderLine.ReplaceColumns(newStr, '\t');
        }
        else if((newStr[0] != '#') || (newStr[1] != '#'))
        {
            // A meta line must start with "##", we expect meta lines until
            // the header line is found.
            myStatus.setStatus(StatGenStatus::INVALID, 
                               "Error reading VCF Meta/Header, line not starting with '##' found before the header line.");
            return(false);
        }
    }
    return(true);
}


bool VcfHeader::write(IFILE filePtr)
{
    if(filePtr == NULL)
    {
        // No file was passed in.
        myStatus.setStatus(StatGenStatus::FAIL_ORDER, 
                           "Need to pass in an open file ptr to VcfHeader::write.");
        return(false);
    }
    
    int numWritten = 0;
    int numExpected = 0;
    for(std::vector<String>::iterator iter = myHeaderLines.begin(); 
        iter != myHeaderLines.end(); iter++)
    {
        numWritten += ifprintf(filePtr, "%s\n", iter->c_str());
        // expected to write string + new line.
        numExpected += iter->Length();
        numExpected += 1;
    }
    if(numWritten != numExpected)
    {
        myStatus.setStatus(StatGenStatus::FAIL_IO, 
                           "Failed writing VCF Meta/Header.");
    }
    return(numWritten == numExpected);
}


void VcfHeader::reset()
{
    myHasHeaderLine = false;
    myHeaderLines.clear();
}


// Return the error after a failed call.
const StatGenStatus& VcfHeader::getStatus()
{
    return(myStatus);
}


int VcfHeader::getNumMetaLines()
{
    int numHeaderLines = myHeaderLines.size();
    if((numHeaderLines > 1) && (myHasHeaderLine))
    {
        // Remove the header line from the count.
        return(numHeaderLines-1);
    }
    return(numHeaderLines);
}


const char* VcfHeader::getMetaLine(unsigned int index)
{
    if(index >= myHeaderLines.size())
    {
        return(NULL);
    }
    else
    {
        return(myHeaderLines[index].c_str());
    }
    return(NULL);
}


const char* VcfHeader::getHeaderLine()
{
    if(myHasHeaderLine)
    {
        return(myHeaderLines.back().c_str());
    }
    return(NULL);
}


int VcfHeader::getNumSamples()
{
    if(!myHasHeaderLine)
    {
        return(0);
    }
    
    int numFields = myParsedHeaderLine.Length();

    if(numFields > NUM_NON_SAMPLE_HEADER_COLS)
    {
        // There are samples.
        return(numFields - NUM_NON_SAMPLE_HEADER_COLS);
    }

    // No sample fields
    return(0);
}


const char* VcfHeader::getSampleName(unsigned int index)
{
    if(!myHasHeaderLine)
    {
        // No header.
        return(NULL);
    }
    int position = index + NUM_NON_SAMPLE_HEADER_COLS;

    if(position >= myParsedHeaderLine.Length())
    {
        // Out of range.
        return(NULL);
    }

    return(myParsedHeaderLine[position].c_str());
}
