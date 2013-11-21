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

#include "PhoneHome.h"
#include "knetfile.h"

#include <iostream>
#include <string.h>

std::string PhoneHome::ourBaseURL = "http://csgph.sph.umich.edu/ph/";
std::string PhoneHome::ourURL = ourBaseURL;
String PhoneHome::ourReturnString = "";
char PhoneHome::ourPrefixChar = '?';

void PhoneHome::addParams(const char* params)
{
     add("args", params);
}


bool PhoneHome::checkVersion(const char* programName, const char* version, 
                             const char* params)
{
    add("pgm", programName);
    add("vsn", version);

    connect();

    if(ourReturnString.SlowCompareToStem("true") != 0)
    {
        std::cerr << "A new version of " << programName << " is available\n";
    }
    
    return(true);
}

void PhoneHome::completionStatus(const char* programName, const char* status)
{
    add("pgm", programName);
    add("status", status);
    connect();
}


void PhoneHome::resetURL()
{
    ourURL = ourBaseURL;
    ourPrefixChar = '?';
}


void PhoneHome::add(const char* name, const char* val)
{
    if((name != NULL) && (strlen(name) != 0) &&
       (val != NULL) && (strlen(val) != 0))
    {
        // Check if the value is already set.
        if(ourURL.find(name) != std::string::npos)
        {
            // value already set, so do not set it.
            return;
        }

        // A value was passed in, so add it to the URL.
        ourURL += ourPrefixChar;
        ourURL += name;
        ourURL += '=';
        // If it is a tool name, trim anything before the last '/'
        if(strstr(name, "pgm") != NULL)
        {
            // toolname, so trim the val..
            const char* toolVal = strrchr(val, '/');
            if(toolVal != NULL)
            {
                toolVal++;
            }
            else
            {
                toolVal = val;
            }
            ourURL.append(toolVal);
        }
        else
        {
            ourURL += val;
        }
        ourPrefixChar = '&';
    }
}


void PhoneHome::connect()
{
    //    std::cerr << "url = " << ourURL << std::endl;
    ourReturnString.Clear();
#ifndef _NO_PHONEHOME
    knet_silent(1);
    knetFile *file = knet_open(ourURL.c_str(), "r");
    if (file == 0) return;

    const int BUF_SIZE = 100;
    char buf[BUF_SIZE];

    knet_read(file, buf, BUF_SIZE);

    //    std::cerr << buf << std::endl;
    knet_close(file);
    knet_silent(0);
    ourReturnString = buf;
#endif
}
