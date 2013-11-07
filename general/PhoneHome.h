/*
 *  Copyright (C) 2013  Regents of the University of Michigan
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

#ifndef __PHONEHOME_H__
#define __PHONEHOME_H__

#include <string>
#include "StringBasics.h"

class PhoneHome
{
public:
    inline static void setURL(const char* url)
    {
        ourURL = url;
    }

    static void addParams(const char* params);

    // Returns false if there is a new version available, otherwise true.
    static bool checkVersion(const char* programName,
                             const char* version,
                             const char* params = NULL);
    static void completionStatus(const char* programName, 
                                 const char* status);
    static void resetURL();
protected:
private:
    static void add(const char* name, const char* val);
    static void connect();

    static std::string ourBaseURL;
    static std::string ourURL;
    static char ourPrefixChar;

    static String ourReturnString;
};

#endif
