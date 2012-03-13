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

#ifndef __BGZF_FILE_TYPE_PBGZF_H__
#define __BGZF_FILE_TYPE_PBGZF_H__

#include "pbgzf.h"
#include "BgzfFileType.h"

class BgzfFileTypePbgzf : public BgzfFileType
{
public:
    BgzfFileTypePbgzf()
        : BgzfFileType(),
          pbgzfHandle(NULL)
    {}

    virtual ~BgzfFileTypePbgzf()
    {
        if(isHandleOpen())
        {
            closeHandle();
        }
    }

protected:
    virtual inline void openHandleFromFilePtr(FILE* filePtr, const char* mode)
    {
        pbgzfHandle = pbgzf_fdopen(fileno(filePtr), mode);
    }

    virtual inline void openHandleFromName(const char* filename, 
                                           const char* mode)
    {
        pbgzfHandle = pbgzf_open(filename, mode);
    }

    virtual inline bool isHandleNull()
    {
        return(pbgzfHandle == NULL);
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
        result = pbgzf_close(pbgzfHandle);
        pbgzfHandle = NULL;
        return result;
    }

    virtual inline unsigned int writeToHandle(const void * buffer,
                                              unsigned int size)
    {
        return pbgzf_write(pbgzfHandle, buffer, size);
    }

    virtual inline int readFromHandle(void * buffer, unsigned int size)
    {
        return(pbgzf_read(pbgzfHandle, buffer, size));
    }
    virtual inline int64_t tellHandle()
    {
        // The calling method, checks that myUsingBuffer is set to false.
        return pbgzf_tell(pbgzfHandle);
    }

    virtual inline bool seekHandle(int64_t offset, int origin)
    {
        return(pbgzf_seek(pbgzfHandle, offset, origin));
    }

    // Returns true if it is there, false if not.
    virtual inline bool checkEofBlock()
    {
        return(pbgzf_check_EOF(pbgzfHandle) == 0);
    }

private:
    // A pbgzfFile is used.
    PBGZF* pbgzfHandle;
};

#endif
