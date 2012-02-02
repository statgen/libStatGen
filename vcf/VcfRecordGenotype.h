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


#ifndef __VCF_RECORD_GENOTYPE_H__
#define __VCF_RECORD_GENOTYPE_H__

#include "VcfRecordField.h"
#include "ReusableVector.h"

/// This header file provides interface to read/write VCF files.
class VcfRecordGenotype : public VcfRecordField
{
public:
    /// Default Constructor, initializes the variables.
    VcfRecordGenotype();

    /// Destructor
    virtual ~VcfRecordGenotype();
    
    /// Read this genotype field from the file up until the next \t,\n, or EOF.
    /// \param filePtr IFILE to read from.
    /// \return true if a tab ended the field, false if it was \n or EOF (always
    /// returns false since this is the last field on the line).
    bool read(IFILE filePtr);

    /// Write the genotype field to the file, without printing the
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
    /// specified key for the specified sample
    /// (the pointer will be invalid if the field is changed/reset).  
    /// \param key to find the falue for.
    /// \param sampleNum which sample to get the value for (starts at 0).
    /// \return const pointer to the string value for this key, NULL if
    /// the sample or the key wer not found.
    const std::string* getString(const std::string& key, 
                                 int sampleNum);

    /// Set the string associated with the specified key for the specified
    /// sample, returns true if set, false if not.
    bool setString(const std::string& key, int sampleNum, 
                   const std::string& value);

    /// Get the number of samples.
    const int getNumSamples();

protected:

private:

    //   std::map<const std::string*, int> myTypeToPos;

    // Format field indexed by position with the value
    // as the datatype.
    typedef ReusableVector<std::string> VCF_SAMPLE;
    VCF_SAMPLE myPosToType;
    ReusableVector<VCF_SAMPLE> mySamples;

    // Can also be used for writing the format as it has the same syntax as a sample.
    bool writeSample(IFILE filePtr, VCF_SAMPLE& sample);
};

#endif
