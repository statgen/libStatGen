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

#ifndef __BGZFFILETYPE_H__
#define __BGZFFILETYPE_H__

#include <stdexcept> // stdexcept header file
#include "FileType.h"

class BgzfFileType : public FileType
{
public:
    BgzfFileType()
        : FileType(),
          myEOF(false),
          myStartPos(0)
    {}

    virtual ~BgzfFileType()
    {}

    virtual void open(const char * filename, const char * mode);

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

    // Read into a buffer from the file.
    virtual inline int read(void * buffer, unsigned int size)
    {
        int bytesRead = 0;
        if(isHandleOpen())
        {
            bytesRead = readFromHandle(buffer, size);
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


    // Get current position in the file. -1 return value indicates an error.
    virtual inline int64_t tell()
    {
        if(myUsingBuffer)
        {
            throw std::runtime_error("IFILE: CANNOT use buffered reads and tell for BGZF files");
        }
        if(isHandleOpen())
        {
            return(tellHandle());
        }
        // Can't tell without an open file.
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
        if(isHandleOpen())
        {
            returnVal = seekHandle(offset, origin);
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

    /// Get which type of threading to use - pbgzf or bgzf.
    /// True = use pbgzf (multi-threaded)
    /// False = use bgzf (single-threaded)
    static inline bool usePbgzf()
    {
        return(ourMultiThreaded != 0);
    }

    /// Set the type of compression to use.
    /// If it is non-zero, use PBGZF with that many threads.
    /// If it is 0, use BGZF compression/decompression.  
    static void setMultiThreaded(int numThreads);

protected:
    virtual int readFromHandle(void * buffer, unsigned int size) = 0;
    virtual int64_t tellHandle() = 0;
    virtual bool seekHandle(int64_t offset, int origin) = 0;
    virtual bool checkEofBlock() = 0;

    // Flag indicating EOF since there isn't one on the handle.
    bool myEOF;

    int64_t myStartPos;

    // Static variable to track whether or not to require the EOF Block
    // at the end of the file.  If the block is required, but not on the file,
    // the constructor fails to open the file.
    static bool ourRequireEofBlock;

    static int ourMultiThreaded;
};

#endif
