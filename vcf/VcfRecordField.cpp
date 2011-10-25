
/*
 *  Copyright (C) 2011  Regents of the University of Michigan,
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
#include "VcfRecordField.h"


int VcfRecordField::readTilChar(IFILE filePtr, const std::string& stopChars, 
                                std::string& stringRef)
{
    int charRead = 0;
    size_t pos = std::string::npos;
    // Loop until the character was not found in the stop characters.
    while(pos == std::string::npos)
    {
        charRead = ifgetc(filePtr);

        // First Check for EOF.  If EOF is found, just return -1
        if(charRead == EOF)
        {
            return(-1);
        }
        
        // Try to find the character in the stopChars.
        pos = stopChars.find(charRead);

        if(pos == std::string::npos)
        {
            // Didn't find a stop character and it is not an EOF, 
            // so add it to the string.
            stringRef += charRead;
        }
    }
    return(pos);
}
