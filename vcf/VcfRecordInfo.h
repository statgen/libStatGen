/*
 *  Copyright (C) 2011  Regents of the University of Michigan,
 *                      Hyun Min Kang, Matthew Flickenger, Matthew Snyder,
 *                      and Goncalo Abecasis
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


#ifndef __VCF_RECORD_INFO_H__
#define __VCF_RECORD_INFO_H__

#include <list>

#include "VcfRecordField.h"


/// This header file provides interface to read/write VCF files.
class VcfRecordInfo : public VcfRecordField
{
public:
    /// Default Constructor, initializes the variables.
    VcfRecordInfo();

    /// Destructor
    virtual ~VcfRecordInfo();
    
    /// Read this info field from the file up until the next \t,\n, or EOF.
    /// \param filePtr IFILE to read from.
    /// \return true if a tab ended the field, false if it was \n or EOF.
    bool read(IFILE filePtr);

    /// Write the info field to the file, without printing the
    // starting/trailing '\t'.
    /// \param filePtr IFILE to write to.
    /// \return true if the field was successfully written to the specified
    ///  filePtr, false if not.
    bool write(IFILE filePtr);

    /// reset the field for a new entry.
    void reset();
    /// reset the field for a new entry.
    void clear() {reset();}

    /// Get a pointer to the string containing the value associated with the
    /// specified key (the pointer will be invalid if the field is
    /// changed/reset).  
    /// \param key to find the value for.
    /// \return const pointer to the string value for this key, NULL if
    /// the key was not found, a pointer to an empty string if the key
    /// was found, but does not have a value.
    const std::string* getString(const char* key);

    /// Set the string value associated with the specified key.  
    /// \param key to set the value for.
    /// \param const pointer to the string value for this key, NULL if
    /// the key was not found, a pointer to an empty string if the key
    /// was found, but does not have a value.
    void setString(const char* key, const char* stringVal);

protected:

private:
    class InfoContainer
    {
    public:
        typedef std::pair<std::string, std::string> InfoKeyValue;
        InfoContainer();
        void reset();
        InfoKeyValue& getNextEmpty();
        InfoKeyValue* find(const char* key);
        // Does not print the starting/trailing '\t'
        bool write(IFILE filePtr);
    private:
        typedef std::list<InfoKeyValue>::iterator InfoContainerIter;
        std::list<InfoKeyValue> myCont;
        InfoContainerIter myNextEmpty;
    };

    InfoContainer myInfo;
};


#endif
