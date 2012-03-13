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

#ifndef __BGZF_FILE_TYPE_BGZF_H__
#define __BGZF_FILE_TYPE_BGZF_H__

#include "bgzf.h"
#include "BgzfFileType.h"

class BgzfFileTypeBgzf : public BgzfFileType
{
public:
    BgzfFileTypeBgzf()
        : BgzfFileType(),
          bgzfHandle(NULL)
    {}

    virtual ~BgzfFileTypeBgzf()
    {
        if(isHandleOpen())
        {
            closeHandle();
        }
    }

protected:
    virtual inline void openHandleFromFilePtr(FILE* filePtr, const char* mode)
    {
        bgzfHandle = bgzf_fdopen(fileno(filePtr), mode);
    }

    virtual inline void openHandleFromName(const char* filename, 
                                           const char* mode)
    {
        bgzfHandle = bgzf_open(filename, mode);
    }

    virtual inline bool isHandleNull()
    {
        return(bgzfHandle == NULL);
    }
    
    // Check to see if the file is open.
    virtual inline bool isHandleOpen()
    {
        return(!isHandleNull());
    }

    // Close the file.
    virtual inline int closeHandle()
    {
        int result = 0;
        result = bgzf_close(bgzfHandle);
        bgzfHandle = NULL;
        return result;
    }

    virtual inline unsigned int writeToHandle(const void * buffer,
                                              unsigned int size)
    {
        return bgzf_write(bgzfHandle, buffer, size);
    }

    virtual inline int readFromHandle(void * buffer, unsigned int size)
    {
        return(bgzf_read(bgzfHandle, buffer, size));
    }
    virtual inline int64_t tellHandle()
    {
        // The calling method, checks that myUsingBuffer is set to false.
        return bgzf_tell(bgzfHandle);
    }

    virtual inline bool seekHandle(int64_t offset, int origin)
    {
        return(bgzf_seek(bgzfHandle, offset, origin));
    }

    // Returns true if it is there, false if not.
    virtual inline bool checkEofBlock()
    {
        return(bgzf_check_EOF(bgzfHandle) == 0);
    }

private:
    // A bgzfFile is used.
    BGZF* bgzfHandle;
};

#endif
