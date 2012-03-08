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

#ifndef __BGZFFILETYPE_H__
#define __BGZFFILETYPE_H__

#include <stdexcept> // stdexcept header file
#include "bgzf.h"
#include "pbgzf.h"
#include "FileType.h"

class BgzfFileType : public FileType
{
public:
    BgzfFileType()
        : bgzfHandle(NULL),
          pbgzfHandle(NULL),
          myEOF(false)
    {}

    virtual ~BgzfFileType()
    {
        close();
    }

    BgzfFileType(const char * filename, const char * mode);

    virtual bool operator == (void * rhs)
    {
        // No two file pointers are the same, so if rhs is not NULL, then
        // the two pointers are different (false).
        if (rhs != NULL)
            return false;
        // Also, both file handles must be null.
        return ((bgzfHandle == rhs) && (pbgzfHandle == rhs));
    }

    virtual bool operator != (void * rhs)
    {
        // No two file pointers are the same, so if rhs is not NULL, then
        // the two pointers are different (true).
        if (rhs != NULL)
            return true;
        // If either file handle is not null, then they are different.
        return ((bgzfHandle != rhs) || (pbgzfHandle != rhs));
    }

    // Close the file.
    virtual inline int close()
    {
        int result = 0;
        // Only one of the two handles can be non-NULL
        if(bgzfHandle != NULL)
        {
            result = bgzf_close(bgzfHandle);
            bgzfHandle = NULL;
        } else if(pbgzfHandle != NULL)
        {
            result = pbgzf_close(pbgzfHandle);
            pbgzfHandle = NULL;
        }
        return result;
    }


    // Reset to the beginning of the file.
    virtual inline void rewind()
    {
        // Just call rewind to move to the beginning of the file.
        seek(myStartPos, SEEK_SET);
    }

    // Check to see if we have reached the EOF.
    virtual inline int eof()
    {
        //  check the file for eof.
        return myEOF;
    }

    // Check to see if the file is open.
    virtual inline bool isOpen()
    {
        if((bgzfHandle != NULL)|| (pbgzfHandle != NULL))
        {
            // One of the two handles is not null, so the file is open.
            return(true);
        }
        return(false);
    }

    // Write to the file
    virtual inline unsigned int write(const void * buffer, unsigned int size)
    {
        if(bgzfHandle != NULL)
        {
            return bgzf_write(bgzfHandle, buffer, size);
        }
        if(pbgzfHandle != NULL)
        {
            return pbgzf_write(pbgzfHandle, buffer, size);
        }
        // Neither file is open so return 0 written.
        return(0);
    }

    // Read into a buffer from the file.  Since the buffer is passed in and
    // this would bypass the fileBuffer used by this class, this method must
    // be protected.
    virtual inline int read(void * buffer, unsigned int size)
    {
        int bytesRead = 0;

        if(bgzfHandle != NULL)
        {
            bytesRead = bgzf_read(bgzfHandle, buffer, size);
        }
        else if(pbgzfHandle != NULL)
        {
            bytesRead = pbgzf_read(pbgzfHandle, buffer, size);
        }
        if ((bytesRead == 0) && (size != 0))
        {
            myEOF = true;
        }
        else if((bytesRead != (int)size) & (bytesRead >= 0))
        {
            // Less then the requested size was read 
            // and an error was not returned (bgzf_read returns -1 on error).
            myEOF = true;
        }
        else
        {
            myEOF = false;
        }
        return bytesRead;
    }


    // Get current position in the file.
    // -1 return value indicates an error.
    virtual inline int64_t tell()
    {
        if(myUsingBuffer)
        {
            throw std::runtime_error("IFILE: CANNOT use buffered reads and tell for BGZF files");
        }
        if(bgzfHandle != NULL)
        {
            return bgzf_tell(bgzfHandle);
        }
        if(pbgzfHandle != NULL)
        {
            return pbgzf_tell(pbgzfHandle);
        }
        // Can't call tell without an open file.
        return(-1);
    }


    // Seek to the specified offset from the origin.
    // origin can be any of the following:
    // Note: not all are valid for all filetypes.
    //   SEEK_SET - Beginning of file
    //   SEEK_CUR - Current position of the file pointer
    //   SEEK_END - End of file
    // Returns true on successful seek and false on a failed seek.
    virtual inline bool seek(int64_t offset, int origin)
    {
        int64_t returnVal = 0;
        if(bgzfHandle != NULL)
        {
            returnVal = bgzf_seek(bgzfHandle, offset, origin);
        }
        else if(pbgzfHandle != NULL)
        {
            returnVal = pbgzf_seek(pbgzfHandle, offset, origin);
        }
        // Check for failure.
        if (returnVal == -1)
        {
            return false;
        }
        // Successful.
        // Reset EOF, assuming no longer at eof - first read will indicate
        // eof if it is at the end.
        myEOF = false;
        return true;
    }

    /// Set whether or not to require the EOF block at the end of the
    /// file.  True - require the block.  False - do not require the block.
    static void setRequireEofBlock(bool requireEofBlock);

    /// Set whether or not to use PBGZF instead of BGZF for multi-threaded
    /// compression/decompression.  
    /// file.  True - use PBGZF, False - use BGZF (default).
    static void setUsePgzf(bool usePbgzf);

protected:

    // A bgzfFile is used.
    BGZF* bgzfHandle;
    PBGZF* pbgzfHandle;

    // Flag indicating EOF since there isn't one on the handle.
    bool myEOF;

    int64_t myStartPos;

    // Static variable to track whether or not to require the EOF Block
    // at the end of the file.  If the block is required, but not on the file,
    // the constructor fails to open the file.
    static bool ourRequireEofBlock;

    static bool ourUsePbgzf;
};

#endif
