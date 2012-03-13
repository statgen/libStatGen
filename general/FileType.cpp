/*
 *  Copyright (C) 2010-2012  Regents of the University of Michigan
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

#include "FileType.h"

void FileType::open(const char * filename, const char * mode)
{
    // close any already opened file.
    close();

    // If the file is for write and is '-', then write to stdout.
    if(((mode[0] == 'w') || (mode[0] == 'W')) && 
       (strcmp(filename, "-") == 0))
    {
        // Write to stdout.
        openHandleFromFilePtr(stdout, mode);
    }
    else if(((mode[0] == 'r') || (mode[0] == 'R')) && 
       (strcmp(filename, "-") == 0))
    {
        // read from stdin
        openHandleFromFilePtr(stdin, mode);
    }
    else
    {
        openHandleFromName(filename, mode);
    }
}


// Set by the InputFile to inform this class if buffering
// is used.  Maybe used by child clases (bgzf) to disable 
// tell.  NOTE: this class does no buffering, the
// buffering is handled by the calling class.
void FileType::setBuffered(bool buffered)
{
    myUsingBuffer = buffered;
}

//
// one class, BgzfFileTypeRecovery overloads this method because
// it is able to sync on a new record using the checkSignature
// callback function.
//
// For all other classes, this is a NOP (sync fails).
//
bool FileType::attemptRecoverySync(bool (*checkSignature)(void *data) , int length)
{
    return false;
}

