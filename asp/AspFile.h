/*
 *  Copyright (C) 2012  Regents of the University of Michigan
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

//////////////////////////////////////////////////////////////////////////


#ifndef __ASP_FILE_H__
#define __ASP_FILE_H__

#include "AspHeader.h"
#include "AspRecord.h"

/// Pure Virtual class, use AspFileReader or AspFileWriter.
class AspFile
{
public:
    AspFile();
    
    ~AspFile();
    
    /// Close the file if it is open.
    void close();

    /// Returns whether or not the file has been opened successfully.
    /// \return true = open; false = not open.
    bool isOpen();

    inline int getNumPosRecs() { return(myNumPosRecs); }
    inline int getNumEmptyRecs() { return(myNumEmptyRecs); }
    inline int getNumRefOnlyRecs() { return(myNumRefOnlyRecs); }
    inline int getNumDetailedRecs() { return(myNumDetailedRecs); }
    inline int getNumUnknownRecs() { return(myNumUnknownRecs); }

protected:
    /// Open the file
    /// \param  filename the file to open.
    /// \param  mode how to open (read or write)
    /// \return true = success; false = failure.
    bool open(const char* fileName, const char* mode);

    virtual void reset();

    void updateRecordCount(AspRecord& record);

    IFILE myFilePtr;

    // Stores the header of mapping chromosome name to id.
    AspHeader myHeader;

    int myNumPosRecs;
    int myNumEmptyRecs;
    int myNumRefOnlyRecs;
    int myNumDetailedRecs;
    int myNumUnknownRecs;
private:
    AspFile(const AspFile& file);
};


class AspFileReader : public AspFile
{
public:
    AspFileReader();
    
    ~AspFileReader();

    /// Open the file
    /// \param  filename the file to open for reading.
    /// \return true = success; false = failure.
    bool open(const char* fileName);

    /// Returns whether or not the end of the file has been reached.
    /// \return true = EOF; false = not eof.
    /// If the file is not open, true is returned.
    bool isEof();

    /// Get the next record without skipping empty and position records.
    /// \param rec reference to a record to populate with the next record.
    /// \return true if a record was successfully found, false if not.
    bool getNextRecord(AspRecord& rec);

    /// Get the next data record (skips over empty and position records).
    /// \param rec reference to a record to populate with the next data record.
    /// \return true if a record was successfully found, false if not.
    bool getNextDataRecord(AspRecord& rec);

    /// Get the data record at the specified position.  The file is advanced
    /// to this position.  If a data record is not found at the position or
    /// if the position has already been passed, NULL is returned.
    /// This is a constant pointer to the record in this class.  The record
    /// is replaced the next time a new position is read.
    /// \return pointer to the data record stored in this class or NULL
    const AspRecord* getDataRecord(const char* chromName, int32_t pos0Based);

    /// Get the reference only record at the specified position.  The file is
    /// advanced to this position.  If a reference only record is not found at
    /// the position or if the position has already been passed, NULL is
    /// returned.  This is a constant pointer to the record in this class.
    /// The record is replaced the next time a new position is read.
    /// \return pointer to the data record stored in this class or NULL
    const AspRecord* getRefOnlyRecord(const char* chromName, int32_t pos0Based);

    /// Get the detailed record at the specified position.  The file is
    /// advanced to this position.  If a detailed record is not found at
    /// the position or if the position has already been passed, NULL is
    /// returned.  This is a constant pointer to the record in this class.
    /// The record is replaced the next time a new position is read.
    /// \return pointer to the data record stored in this class or NULL
    const AspRecord* getDetailedRecord(const char* chromName, 
                                       int32_t pos0Based);

    /// Get the likelihood at the specified position of the genotype with
    /// the 2 specified bases advancing in the file to the specified 
    /// position.
    /// Notes:
    ///   If the position has already been passed, 0 is returned.
    ///   If the position is not in the file, it is advanced (if necessary)
    ///   to the first position in the file that is after the specified
    ///   position and 0 is returned.
    ///   Make sure all bases are the same case (upper/lower).
    int getLikelihood(const char* chromName, int32_t pos0Based,
                      char base1, char base2, char refBase);

    /// Get the number of bases at the specified position
    /// advancing in the file to the specified position.
    /// Notes:
    ///   If the position has already been passed, 0 is returned.
    ///   If the position is not in the file, it is advanced (if necessary)
    ///   to the first position in the file that is after the specified
    ///   position and 0 is returned.
    int getNumBases(const char* chromName, int32_t pos0Based);

protected:
    virtual void reset();

private:
    AspFileReader(const AspFileReader& file);

    bool advanceToPos(const char* chromName, int32_t pos0Based);

    // Store the previous chromosome id/position which is necessary for getting
    // the chromid/pos for non-pos records.
    int32_t prevChrom;
    int32_t prevPos;

    // Stores the header of mapping chromosome name to id.
    AspHeader myHeader;

    // Used to store a record that is used in the accessor logic that
    // takes a position and returns information rather than a record.
    // The record is stored here.
    AspRecord myStoredRecord;
};


class AspFileWriter : public AspFile
{
public:
    AspFileWriter();
    
    ~AspFileWriter();
    
    /// Set the gap size threshold such that gaps smaller than this size are
    /// filled with empty records and gaps this size or larger are writen
    /// as new chromosome/position. 
    static void setGapSize(int gapSize);

    /// Open the file
    /// \param  filename the file to open for writing.
    /// \param samHeader SamFileHeader to use to generate the AspHeader.
    /// \return true = success; false = failure.
    bool open(const char* fileName, SamFileHeader samHeader);

    /// Write the specified data record.
    /// Writes preceding  empty records or a position record as necessary,
    /// so do not specifically write those.
    bool write(AspRecord& record, const char* chromName, int32_t pos0Based);

    static const int DEFAULT_GAP_SIZE = 100;

protected:
    virtual void reset();

private:
    AspFileWriter(const AspFileWriter& file);

    static int myGapSize;

    // Store the previous chromosome id/position which is necessary for
    // determining if empty or position records need to be written.
    std::string myPrevChromName;
    int32_t myPrevChromID;
    int32_t myPrevPos;

};


#endif
