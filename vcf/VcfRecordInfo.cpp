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
#include "VcfRecordInfo.h"


VcfRecordInfo::VcfRecordInfo()
{
    reset();
}


VcfRecordInfo::~VcfRecordInfo()
{
}


bool VcfRecordInfo::read(IFILE filePtr)
{
    // Clear out any previously set values.
    reset();
    
    if(ifeof(filePtr))
    {
        // End of file, just return false.
        return(false);
    }
    
    static const std::string keyStopChars   = "\n\t;=";
    static const std::string valueStopChars = "\n\t;";
    // The start of the first character in stopChars
    // that means there is more information for this object, so
    // continue reading the file.
    static const int contPos = 2;
    static const int tabPos = 1;

    // Keep reading.  Loop will be exited
    // when a \t, \n, or EOF is found.
    int stopPos = contPos;
    while(stopPos >= contPos)
    {
        // Get the next element to write the key into.
        InfoContainer::InfoKeyValue& nextKeyVal = myInfo.getNextEmpty();
        // Read the next key.
        stopPos = filePtr->readTilChar(keyStopChars, nextKeyVal.first);

        if(keyStopChars[stopPos] == '=')
        {
            // Stoped at the value part, so read the value
            // associated with the key.
            stopPos = filePtr->readTilChar(valueStopChars, nextKeyVal.second);
        }
    }

    // Return whether or not a tab was found at the end of the field.
    return(stopPos == tabPos);
}


bool VcfRecordInfo::write(IFILE filePtr)
{
    return(myInfo.write(filePtr));
}


void VcfRecordInfo::reset()
{
    myInfo.reset();
}


const std::string* VcfRecordInfo::getValue(const char* string)
{
    InfoContainer::InfoKeyValue* pairPtr = myInfo.find(string);
    if(pairPtr == NULL)
    {
        // Not found, return NULL.
        return(NULL);
    }
    // Found, so return the value.
    return(&(pairPtr->second));
}


/////////////////////////////////////////////////////////////
// Info Container
VcfRecordInfo::InfoContainer::InfoContainer()
    : myCont()
{
    myNextEmpty = myCont.end();
}


void VcfRecordInfo::InfoContainer::reset()
{
    // Set the next empty element to be the first one on the list.
    // That means there are none used.
    myNextEmpty = myCont.begin();
}


VcfRecordInfo::InfoContainer::InfoKeyValue& VcfRecordInfo::InfoContainer::getNextEmpty()
{
    if(myNextEmpty == myCont.end())
    {
        // We are at the end of the list of available entries, so add a new one.
        myCont.resize(myCont.size() + 1);
        // The next empty one will also be the new end.
        myNextEmpty = myCont.end();
        // The default constructors will initialize both key & value to
        // the empty strings.
        // return the last element (the one that was just added).
        return(myCont.back());
    }
    // myNextEmpty is an element, and not the end.
    // So, clear out the strings.
    myNextEmpty->first.clear();
    myNextEmpty->second.clear();
    InfoKeyValue& returnVal = *myNextEmpty;
    // Increment next empty to the next element.
    ++myNextEmpty;
    // return the element to be used.
    return(returnVal);
}


VcfRecordInfo::InfoContainer::InfoKeyValue* VcfRecordInfo::InfoContainer::find(const char* key)
{
    for(InfoContainerIter iter = myCont.begin();
        iter != myNextEmpty; iter++)
    {
        if(iter->first == key)
        {
            // Found it.
            return(&(*iter));
        }
    }

    // Did not find it, so return null.
    return(NULL);
}


bool VcfRecordInfo::InfoContainer::write(IFILE filePtr)
{
    int numWritten = 0;
    int numExpected = 0;

    for(InfoContainerIter iter = myCont.begin();
        iter != myNextEmpty; iter++)
    {
        if(iter == myCont.begin())
        {
            // First entry, so no ';'
            // Check if there is a value.
            if(iter->second.empty())
            {
                // write just the key, no value.
                numWritten = ifprintf(filePtr, "%s", iter->first.c_str());
                numExpected = iter->first.size();
            }
            else
            {
                // write the key, and the value.
                numWritten = ifprintf(filePtr, "%s=%s", 
                                      iter->first.c_str(), 
                                      iter->second.c_str());
                numExpected = iter->first.size() + iter->second.size() + 1;
            }
        }
        else
        {
            // Not first entry, so use a ';'
            // Check if there is a value.
            if(iter->second.empty())
            {
                // write the ';' and just the key, no value.
                numWritten = ifprintf(filePtr, ";%s", iter->first.c_str());
                numExpected = 1 + iter->first.size();
            }
            else
            {
                // write the ';', the key, and the value
                numWritten = ifprintf(filePtr, ";%s=%s", 
                                      iter->first.c_str(), 
                                      iter->second.c_str());
                numExpected = 1 + iter->first.size() + iter->second.size() + 1;
            }
        }
    } // End loop through entries.
    return(numWritten == numExpected);
}
