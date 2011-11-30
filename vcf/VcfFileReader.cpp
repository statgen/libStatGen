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
    : VcfFile()
{
  myFilePtr = NULL;
}


VcfFileReader::~VcfFileReader() 
{
}


bool VcfFileReader::open(const char* filename, VcfHeader& header)
{
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


bool VcfFileReader::readRecord(VcfRecord& record)
{
    myStatus = StatGenStatus::SUCCESS;
    if(!record.read(myFilePtr, mySiteOnly))
    {
        myStatus = record.getStatus();
        return(false);
    }
    return(true);
}
