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


#ifndef __ASP_RECORD_H__
#define __ASP_RECORD_H__

#include "SamFile.h"
#include "BaseAsciiMap.h"

class AspRecord
{
public:
    static const char DELETION_BASE = 'D';

    AspRecord();
    
    ~AspRecord();

    /// Set whether or not to include reads with bases of 'N' in 
    /// detailed records.  Default is true (include them).
    /// They are not included in RefOnly records.
    /// The setting in place when the information is added is what is used.
    static void setIncludeNinDetailed(bool includeN)
    { ourIncludeNinDetailed = includeN; }
    
    ////////////////////////////////////////////////
    // Setting/adding values methods.
    // Add will set values for both refOnly & detailed.
    // The record defaults to detailed.

    // Qual is the character quality.
    // Returns true if the information got added.  Returns false
    // if it did not get added (already hit max number of entries or
    // 'N' and it is being ignored). 
    bool add(char refBase, char base, char qual,
             int cycle, bool strand, int mq);

    // Sets this record to an empty record type.
    void setEmptyType();
    // Sets this record to a position type.
    void setPosType();
    // Sets this record to a ref only type.
    void setRefOnlyType();
    // Sets this record to a detailed type.
    void setDetailedType();

    void reset();

    //////////////////////////////////
    // Reading methods.

    /// Read a record from the file.  It is assumed the file is in the 
    /// correct position.  The chromID/position are the chromID & position
    /// of the previous record and will be updated to this record's
    /// chromosomeID & position.  If this is the first record, then they
    /// should be passed in as -1 and an error will be reported if the
    /// first record is not a position record.
    bool read(IFILE filePtr, int32_t& chromID, int32_t& pos);

    inline bool isEmptyType()    { return(myType == EMPTY_REC);    }
    inline bool isPosType()      { return(myType == POS_REC);      }
    inline bool isRefOnlyType()  { return(myType == REF_ONLY_REC); }
    inline bool isDetailedType() { return(myType == DETAILED_REC); }

    /////////////////////////////////
    // Accessor methods

    /// Get the chrom.
    inline int32_t getChromID() {return(myChromID);}
    inline int32_t get0BasedPos() {return(my0BasedPos);}

    /// For data records, get the reference base
    /// Returns 'N' if it is not a data record.
    char getRefBase();

    /// Get the number of bases in this record.
    inline int getNumBases() {return(myNumBases);}

    /// Get the number of non-N bases in this record.
    /// May be the same as getNumBases() if no 'N' bases
    /// were added to the record.
    int getNumNonNBases();

    /// Get the number of bases that could not be represented because
    /// it went over the maximum number allowed.
    inline int32_t getNumOverMaxBases() {return(myOverMaxNumBases);}

    /// Get the GLH.  0 is returned if this is not a RefOnly record.
    int getGLH();
    /// Get the GLA.  0 is returned if this is not a RefOnly record.
    int getGLA();

    // Get the likelihood of the genotype with the 2 specified bases.
    // Make sure all bases are the same case (upper/lower).
    int getLikelihood(char base1, char base2, char refBase);
    

    ////////////////////////
    // Detailed Record Only
    /// Get the base at the specified index (starts at 0).
    /// An out of range index returns 'N'.
    char getBaseChar(int index);
    /// Get the phred quality at the specified index (starts at 0).
    /// An out of range index returns -1.  An unknown quality returns -1
    int getPhredQual(int index);
    /// Get the quality as the character representation at the specified index
    /// (starts at 0).  An out of range index returns ' '. 
    /// An unknown quality returns ' '
    char getCharQual(int index);
    /// Get the cycle at the specified index (starts at 0).
    /// An out of range index/deletion returns -1.
    int getCycle(int index);
    /// Get the strand at the specified index (starts at 0).
    /// An out of range index returns false.
    bool getStrand(int index);
    /// Get the MQ at the specified index (starts at 0).
    /// An out of range index returns -1.
    int getMQ(int index);


    ////////////////////////////////
    // Writing methods.
    static void writeEmpty(IFILE outputFile);
    static void writePos(int32_t chrom, int32_t pos, IFILE outputFile);
    // Write the record based on the type that was set (default is DETAILED)
    void write(IFILE outputFile);

private:
    AspRecord(const AspRecord & rec);

    static bool ourIncludeNinDetailed;
    static const unsigned int REF_TYPE_LEN;
    static const uint8_t EMPTY_REC;
    static const uint8_t POS_REC;
    static const uint8_t REF_ONLY_REC;
    static const uint8_t DETAILED_REC;
    static const int MAX_NUM_BASES = 255;
    static const int REC_TYPE_MASK = 0xF;
    static const int BASE_SHIFT = 4;

    bool readPosRecord(IFILE filePtr);
    bool readRefOnlyRecord(IFILE filePtr);
    bool readDetailedRecord(IFILE filePtr);
    
    void writeRefOnly(IFILE outputFile);
    void writeDetailed(IFILE outputFile);

    // Get the likelihood of the genotype with the 2 specified bases.
    // Make sure all bases are the same case (upper/lower).
    int getRefOnlyLikelihood(char base1, char base2, char refBase);
    
    // Get the likelihood of the genotype with the 2 specified bases.
    // Make sure all bases are the same case (upper/lower).
    int getDetailedLikelihood(char base1, char base2, char refBase);
    

    inline unsigned int getBasesSize()
    {
        // The number of bytes used for the bases array is the
        // (number of bases + 1) divided by 2.
        // Examples:  
        //   myNumBases = 0, Size = (0+1)/2 = 0
        //   myNumBases = 1, Size = (1+1)/2 = 1
        //   myNumBases = 2, Size = (2+1)/2 = 1
        //   myNumBases = 3, Size = (3+1)/2 = 2
        return((myNumBases+1)/2);
    }
    
    inline unsigned int getStrandsSize()
    {
        // The number of bytes used for the strands array is the
        // (number of bases + 7) divided by 8.
        // Examples:  
        //   myNumBases = 0, Size = (0+7)/8 = 0
        //   myNumBases = 1, Size = (1+7)/8 = 1
        //   myNumBases = 2, Size = (2+7)/8 = 1
        //   myNumBases = 8, Size = (8+7)/8 = 1
        //   myNumBases = 9, Size = (9+7)/8 = 2
        return((myNumBases+7)/8);
    }

    inline uint8_t getRefBaseType() 
    {
        return(((BaseAsciiMap::base2int[(int)myRefBase] & 0xF) << BASE_SHIFT) |
               (myType & REC_TYPE_MASK));
    }

    // myType is really only 4 bits.
    uint8_t myType;

    char myRefBase;

    uint8_t myNumBases;
    // Used for REF_ONLY and to determine if a DETAILED record
    // should be written.  Not used when reading a detailed record from
    // an ASP file.
    uint8_t myNumNonNBases;
    bool myNumNonNBasesSet;

    // This allows a count of additional bases that could
    // not be represented because the maximum number 
    // allowed was hit.
    uint32_t myOverMaxNumBases;

    uint8_t myGLH;
    double myGLAd;
    uint8_t myGLAi;

    // Since each base is only 4 bits, each index holds two bases.
    // The earlier base is in the upper bits.
    int8_t myBases[(MAX_NUM_BASES+1)/2];
    int8_t myQuals[MAX_NUM_BASES];
    int8_t myCycles[MAX_NUM_BASES];
    // Since strands are only 1 bit, each index holds 4 strands
    // with the first strand in the uppermost bit.
    int8_t myStrands[(MAX_NUM_BASES+7)/8];
    uint8_t myMQs[MAX_NUM_BASES];

    // The choromsome & position for this record.
    int32_t myChromID;
    int32_t my0BasedPos;
};


#endif
