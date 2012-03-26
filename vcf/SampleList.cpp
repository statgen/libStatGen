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

#include "SampleList.h"
#include "InputFile.h"

SampleList::SampleList()
    : mySampleList()
{
}


SampleList::~SampleList()
{
    mySampleList.clear();
}


bool SampleList::readFromFile(const char* fileName, const char* delims)
{
    // Open the file.
    IFILE sampleFile = ifopen(fileName, "r");

    if(sampleFile == NULL)
    {
        // Failed to open.
        return(false);
    }

    // read the file.
    std::string tempString;

    std::string delimString = delims;
    delimString += '\n';

    int readResult = 0;
    while(readResult != -1)
    {
        // Clear the temp string. tempString.clear().
        readResult = sampleFile->readTilChar(delimString, tempString);

        // Check to see if something was read (tempString is not empty).
        if(!tempString.empty())
        {
            // sample name found, so add it to the container.
            mySampleList.insert(tempString);
        }
        // Clear the string being read into.
        tempString.clear();
    }
    return(true);
}
