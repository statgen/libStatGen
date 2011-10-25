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

#ifndef __VCF_RECORD_H__
#define __VCF_RECORD_H__

#include <vector>
#include "VcfRecordInfo.h"
#include "VcfRecordGenotype.h"
#include "StatGenStatus.h"

/// This header file provides interface to read/write VCF files.
class VcfRecord
{
public:
    /// Default Constructor, initializes the variables, but does not open
    /// any files.
    VcfRecord();
    /// Destructor
    virtual ~VcfRecord();
    
    /// Read the next Vcf data line from the file.
    /// \param filePtr IFILE to read from.
    /// \param siteOnly only store the first 8 columns
    /// \return true if a line was successfully read from the specified filePtr,
    /// false if not.
    bool read(IFILE filePtr, bool siteOnly);

    /// Write this data line to the file (including the newline).
    /// \param filePtr IFILE to write to.
    /// \param siteOnly only write the first 8 columns
    /// \return true if a line was successfully written to the specified filePtr,
    /// false if not.
    bool write(IFILE filePtr, bool siteOnly);

    /// Reset this header, preparing for a new one.
    void reset();

    /// Returns the status associated with the last method that sets the status.
    /// \return StatGenStatus of the last command that sets status.
    const StatGenStatus& getStatus();

//     bool isValid();

    const char* getChromStr() {return(myChrom.c_str());}
    int get1BasedPosition() {return(my1BasedPosNum);}
    const char* getIDStr() {return(myID.c_str());}
    const char* getRefStr() {return(myRef.c_str());}

    /// Get a reference to the information field.
    VcfRecordInfo& getRefInfo() {return myInfo;}


    void setRef(const char* ref) {myRef = ref;}

protected: 

    /// Read the specified file until a tab, '\n', or EOF is found
    /// and appending the read characters to stringRef (except for the 
    /// stopping character).
    /// \param filePtr open file to be read.
    /// \param stringRef reference to a string that should be appended to
    /// with the characters read from the file until a '\t', '\n', or EOF
    /// is found.
    /// \return true if a '\t' stopped the reading, false if '\n' or
    /// EOF stopped the reading.
    bool readTilTab(IFILE filePtr, std::string& stringRef);

private:

    std::string myChrom;
    int my1BasedPosNum;
    std::string my1BasedPos;
    std::string myID;
    std::string myRef;
    std::string myAlt;
    float myQualNum;
    std::string myQual;
    std::string myFilter;
    VcfRecordInfo myInfo;
    VcfRecordGenotype myGenotype;

    // The status of the last failed command.
    StatGenStatus myStatus;

    // Set Pointers to each piece.

};

#endif
