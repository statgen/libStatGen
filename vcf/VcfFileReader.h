/*
 *  Copyright (C) 2010-2011  Regents of the University of Michigan,
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

#ifndef __VCF_FILE_READER_H__
#define __VCF_FILE_READER_H__

#include "VcfFile.h"
#include "VcfRecord.h"

/// This header file provides interface to read/write VCF files.
class VcfFileReader : public VcfFile
{
public:
    /// Default Constructor, initializes the variables, but does not open
    /// any files.
    VcfFileReader();
    /// Destructor
    virtual ~VcfFileReader();

    /// Open the vcf file with the specified filename for reading.
    /// \param  filename the vcf file to open for reading.
    /// \param header to be read from the file
    /// \return true = success; false = failure.
    virtual bool open(const char* filename, VcfHeader& header);
    
    /// Read the next Vcf data line from the file.
    /// \param record record to populate with the next data line.
    /// \return true if successful, false if not.
    bool readRecord(VcfRecord& record);

    /// Return a reference to the meta/header information.
    /// \return reference 

protected: 

private:
};

#endif
