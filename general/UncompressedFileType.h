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

#ifndef __UNCOMPRESSEDFILETYPE_H__
#define __UNCOMPRESSEDFILETYPE_H__

#include <iostream>
#include "FileType.h"

class UncompressedFileType : public FileType
{
public:
    UncompressedFileType()
        : FileType(),
          fileHandle(NULL)
    {}

    virtual ~UncompressedFileType() 
    {
        if(isHandleOpen())
        {
            closeHandle();
        }
    }

    // Reset to the beginning of the file.
    inline void rewind()
    {
        // Just call rewind to move to the beginning of the file.
        ::rewind(fileHandle);
    }

    // Check to see if we have reached the EOF.
    inline int eof()
    {
        //  check the file for eof.
        return feof(fileHandle);
    }

    // Read into a buffer from the file.  Since the buffer is passed in and
    // this would bypass the fileBuffer used by this class, this method must
    // be protected.
    inline int read(void * buffer, unsigned int size)
    {
        return fread(buffer, 1, size, fileHandle);
    }


    // Get current position in the file.
    // -1 return value indicates an error.
    virtual inline int64_t tell()
    {
        return ftell(fileHandle);
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
        int64_t returnVal = fseek(fileHandle, offset, origin);
        // Check for success - 0 return value.
        if (returnVal == 0)
        {
            return true;
        }
        // Successful.
        return false;
    }


protected:
    virtual inline void openHandleFromFilePtr(FILE* filePtr, const char* mode)
    {
        fileHandle = filePtr;
    }

    virtual inline void openHandleFromName(const char* filename, 
                                           const char* mode)
    {
        fileHandle = fopen(filename, mode);
    }

    virtual inline bool isHandleNull()
    {
        return(fileHandle == NULL);
    }
    
    // Check to see if the file is open.
    virtual inline bool isHandleOpen()
    {
        return(!isHandleNull());
    }

    // Close the file.
    virtual inline int closeHandle()
    {
        // Do not close stdout or stdin.
        if((fileHandle != stdout) && (fileHandle != stdin))
        {
            int result = fclose(fileHandle);
            fileHandle = NULL;
            return result;
        }
        fileHandle = NULL;
        return 0;
    }

    // Write to the file
    virtual inline unsigned int writeToHandle(const void * buffer, unsigned int size)
    {
        return fwrite(buffer, 1, size, fileHandle);
    }


protected:
    // A FILE Pointer is used.
    FILE* fileHandle;
};

#endif


