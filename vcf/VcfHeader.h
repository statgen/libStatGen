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

#ifndef __VCF_HEADER_H__
#define __VCF_HEADER_H__

#include <list>
#include "StringBasics.h"
#include "StatGenStatus.h"

/// This header file provides interface for dealing with VCF Meta/Header lines.
class VcfHeader
{
public:
    /// Default Constructor, initializes the variables.
    VcfHeader();
    /// Destructor
    virtual ~VcfHeader();

    /// Read the header from the specified file, fails if a header
    /// has already been completely read.
    /// \param filePtr IFILE to read from.
    /// \return true if an entire meta/header was successfully read from
    /// the specified filePtr, false if not.
    bool read(IFILE filePtr);

    /// Write the header to the specified file.
    /// \param filePtr IFILE to write to.
    /// \return true if an entire meta/header was successfully written to
    /// the specified filePtr, false if not.
    bool write(IFILE filePtr);

    // Add Meta Line.

    //    void setFormat(const char* format);

    /// Reset this header, preparing for a new one.
    void reset();

    /// Returns the status associated with the last method that sets the status.
    /// \return StatGenStatus of the last command that sets status.
    const StatGenStatus& getStatus();
    
protected: 

private:
    // Is set to true once the header line has been set, false until then.
    bool hasHeaderLine;

    std::list<String> myHeaderLines;

    // The status of the last failed command.
    StatGenStatus myStatus;
};

#endif
