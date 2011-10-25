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
    if(hasHeaderLine)
    {
        // Already set the header line, so can't add any more lines.
        myStatus.setStatus(StatGenStatus::FAIL_ORDER, 
                           "Error reading VCF Meta/Header, Header line has already been set, so can't read more lines from a file.");
        return(false);
    }

    // Read until the header line has been read (after the meta lines).
    while(!hasHeaderLine)
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
            hasHeaderLine = true;
        }
        else if((newStr[0] != '#') || (newStr[1] != '#'))
        {
            // A meta line must start with "##", we expect mets lines until
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
    int numWritten = 0;
    int numExpected = 0;
    for(std::list<String>::iterator iter = myHeaderLines.begin(); 
        iter != myHeaderLines.end(); iter++)
    {
        numWritten += ifprintf(filePtr, "%s\n", iter->c_str());
        // expected to write string + new line.
        numExpected += iter->Length();
        numExpected += 1;
    }
    return(numWritten == numExpected);
}


void VcfHeader::reset()
{
    hasHeaderLine = false;
    myHeaderLines.clear();
}


// Return the error after a failed call.
const StatGenStatus& VcfHeader::getStatus()
{
    return(myStatus);
}
