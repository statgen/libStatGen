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

//////////////////////////////////////////////////////////////////////////


#ifndef __ASP_HEADER_H__
#define __ASP_HEADER_H__

#include "SamFile.h"

class AspHeader
{
public:
    AspHeader();
    
    ~AspHeader();
    
    /// Set this header from the SamFileHeader.
    bool set(SamFileHeader& samHeader);

    /// Read the header from the ASP file.  It is assumed the file is in the 
    /// correct position (the beginning).
    bool read(IFILE filePtr);

    /// Write the header to the ASP file.  It is assumed the file is in the 
    /// correct position (the beginning).
    bool write(IFILE outputFile);

    /// Get the chromosome ID for the specified reference name.
    /// Returns -1 if the chromosome name is not found.
    int32_t getChromID(const char* chromName);

    /// Get the chromosome name for the specified chromosome id.
    /// False is returned if the chromosome ID was not found, true if it was.
    bool getChromName(int32_t chromID, std::string& chromName);
    
    void reset();

private:
    AspHeader(const AspHeader & rec);

    std::map<std::string, int> myNameToID;
    std::vector<std::string> myIDToName;
};


#endif
