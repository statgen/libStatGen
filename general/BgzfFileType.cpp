/*
 *  Copyright (C) 2010  Regents of the University of Michigan
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

#ifdef __ZLIB_AVAILABLE__

#include <iostream>
#include <string.h>

#include "BgzfFileType.h"

// Default to require the EOF block at the end of the file.
bool BgzfFileType::ourRequireEofBlock = true;

BgzfFileType::BgzfFileType(const char * filename, const char * mode)
{
    BgzfFileType::numThreads = 1;
    char threadSpec[8];
    char bgzfMode[8];
    const char* thread_start = strchr(mode, '@');
    int mode_len = 0;
    if (thread_start != NULL){
        mode_len = thread_start-mode;
    } else {
        mode_len = strnlen(mode, 7);
    }
    printf("%s", mode);
    if(mode_len < 8){
        strncpy(bgzfMode, mode, mode_len);
        bgzfMode[mode_len] = '\0';
    } else {
        strncpy(bgzfMode, mode, 7);
        bgzfMode[7] = '\0';
    }
    if (thread_start != NULL && thread_start+1 != '\0'){
        // Advance past the @, then parse the number of threads
        thread_start++;
        strncpy(threadSpec, thread_start, 7);
        threadSpec[7] = '\0';
        numThreads = strtol(threadSpec, NULL, 10);
        // If we can't parse the number, revert to one thread
        if (numThreads == 0){ numThreads = 1; }
    }
    // If the file is for write and is '-', then write to stdout.
    if(((mode[0] == 'w') || (mode[0] == 'W')) &&
       (strcmp(filename, "-") == 0))
    {
        // Write to stdout.
        bgzfHandle = bgzf_dopen(fileno(stdout), bgzfMode);
    }
    else if(((mode[0] == 'r') || (mode[0] == 'R')) &&
       (strcmp(filename, "-") == 0))
    {
        // read from stdin
        bgzfHandle = bgzf_dopen(fileno(stdin), bgzfMode);
    }
    else
    {
        bgzfHandle = bgzf_open(filename, bgzfMode);
    }

    myStartPos = 0;
    if (bgzfHandle != NULL)
    {
        //Only do multithreaded IO if more than one thread is used.
        if(numThreads > 1){ bgzf_mt(bgzfHandle, numThreads, 256); }
        // Check to see if the file is being opened for read, if the eof block
        // is required, and if it is, if it is there.
        if ((mode[0] == 'r' || mode[0] == 'R') && (strcmp(filename, "-") != 0)
            && ourRequireEofBlock && (bgzf_check_EOF(bgzfHandle) != 1))
        {
            std::cerr << "BGZF EOF marker is missing in " << filename << std::endl;
            // the block is supposed to be there, but isn't, so close the file.
            close();
        }
        else
        {
            // Successfully opened a properly formatted file, so get the start
            // position.
            myStartPos = bgzf_tell(bgzfHandle);
        }
    }

    myEOF = false;
}


// Set whether or not to require the EOF block at the end of the
// file.  True - require the block.  False - do not require the block.
void BgzfFileType::setRequireEofBlock(bool requireEofBlock)
{
    ourRequireEofBlock = requireEofBlock;
}

#endif
