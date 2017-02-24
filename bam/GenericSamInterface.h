/*
 *  Copyright (C) 2010  Regents of the University of Michigan
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

#ifndef __GENERIC_SAM_INTERFACE_H__
#define __GENERIC_SAM_INTERFACE_H__

#include "SamStatus.h"
#include "InputFile.h"
#include "SamFileHeader.h"
#include "SamRecord.h"
#include <sam.h>
#include <cram.h>



class GenericSamInterface
{
public:
    GenericSamInterface(const char*const fn, const char* mode, const char*const ref_fn = nullptr);
    GenericSamInterface(const char*const fn, const char* mode, const htsFormat& fmt, const char*const ref_fn = nullptr);
    ~GenericSamInterface();
   
    // Pure virtual method that reads the header section from the specified file
    // and stores it in the passed in header, returns false and sets the status
    // on failure.
    // Will be implemented specifically for sam/bam files.
    bool readHeader(SamFileHeader& header,
                            SamStatus& status);

    // Pure virtual method that writes the specified header into the specified
    // file, returns false and sets the status on failure.
    // Will be implemented specifically for sam/bam files.
    bool writeHeader(SamFileHeader& header,
                             SamStatus& status);

    // Pure virtual method that reads the next record from the specified file 
    // and stores it in the passed in record.
    // Will be implemented specifically for sam/bam files.
    // TODO On error, a more detailed message is appended to statusMsg.
    void readRecord(SamFileHeader& header,
                            SamRecord& record, 
                            SamStatus& samStatus);
   
    // Pure virtual method that writes the specified record into the specified
    // file.
    // Will be implemented specifically for sam/bam files.
    SamStatus::Status writeRecord(SamFileHeader& header,
                                          SamRecord& record,
                                          SamRecord::SequenceTranslation translation);
    bool isEOF();

    //[[deprecated]]
    //bool isEOF(IFILE filePtr) { return isEOF(); }
    //[[deprecated]]
    //bool readHeader(IFILE filePtr, SamFileHeader& header, SamStatus& status) { return readHeader(header, status); }
    //[[deprecated]]
    //bool writeHeader(IFILE filePtr, SamFileHeader& header, SamStatus& status) { return writeHeader(header, status); }
    //[[deprecated]]
    //SamStatus::Status writeRecord(IFILE filePtr, SamFileHeader& header, SamRecord& record, SamRecord::SequenceTranslation translation) { return writeRecord(header, record, translation); }
    //[[deprecated]]
    //void readRecord(IFILE filePtr, SamFileHeader& header, SamRecord& record, SamStatus& samStatus) { return readRecord(header, record, samStatus); }


private:
  samFile* fp_;
  bam_hdr_t* hdr_;
  bam1_t* rec_;
  bool eof_;
};

#endif
