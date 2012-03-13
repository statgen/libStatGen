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

#ifndef __FILETYPE_H__
#define __FILETYPE_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>

class FileType
{
public:
    FileType()
        : myUsingBuffer(true)
    {}

    virtual ~FileType()
    {
        // The child classes should close, because
        // a close call would call into virtual methods
        // which is bad when it is destructing.
    }

    virtual void open(const char * filename, const char * mode);

    virtual bool operator == (void * rhs)
    {
        // No two file pointers are the same, so if rhs is not NULL, then
        // the two pointers are different (false).
        if(rhs != NULL)
            return false;
        // Return true if the file handle is also null.
        return(isHandleNull());
    }

    virtual bool operator != (void * rhs)
    {
        // No two file pointers are the same, so if rhs is not NULL, then
        // the two pointers are different (true).
        if (rhs != NULL)
            return true;
        // If the file handle is not null, then they are different.
        return (!isHandleNull());
    }

    // Close the file.
    virtual inline int close()
    {
        if(isHandleOpen())
        {
            return closeHandle();
        }
        // Not open, so just return 0.
        return(0);
    }

    // Reset to the beginning of the file.
    virtual void rewind() = 0;

    // Check to see if we have reached the EOF.
    virtual int eof() = 0;

    // Check to see if the file is open.
    virtual inline bool isOpen()
    {
        return(isHandleOpen());
    }

    // Write to the file
    virtual inline unsigned int write(const void * buffer, unsigned int size)
    {
        if(isHandleOpen())
        {
            return(writeToHandle(buffer, size));
        }
        // Not open, so return that nothing is written.
        return(0);
    }

    // Read into a buffer from the file.
    virtual int read(void * buffer, unsigned int size) = 0;

    // Get current position in the file.
    // -1 return value indicates an error.
    virtual int64_t tell() = 0;

    // Seek to the specified offset from the origin.
    // origin can be any of the following:
    // Note: not all are valid for all filetypes.
    //   SEEK_SET - Beginning of file
    //   SEEK_CUR - Current position of the file pointer
    //   SEEK_END - End of file
    // Returns true on successful seek and false on a failed seek.
    virtual bool seek(int64_t offset, int origin) = 0;

    // Set by the InputFile to inform this class if buffering
    // is used.  Maybe used by child clases (bgzf) to disable 
    // tell.  NOTE: this class does no buffering, the
    // buffering is handled by the calling class.
    void setBuffered(bool buffered);

    //
    // When caller catches an exception, it may call this method.
    // It is implemented only in BgzfFileTypeRecovery.
    //
    virtual bool attemptRecoverySync(bool (*checkSignature)(void *data) , int length);

protected:

    virtual void openHandleFromFilePtr(FILE* filePtr, const char* mode) = 0;
    virtual void openHandleFromName(const char* filename, const char* mode) = 0;
    virtual bool isHandleNull() = 0;
    virtual bool isHandleOpen() = 0;
    // The following methods do not check that the file was open, it is
    // assumed that the calling method checked that prior to calling.
    virtual int closeHandle() = 0;
    virtual unsigned int writeToHandle(const void * buffer,
                                              unsigned int size) = 0;




    // Set by the InputFile to inform this class if buffering
    // is used.  Maybe used by child clases (bgzf) to disable 
    // tell.
    bool myUsingBuffer;
};

#endif

