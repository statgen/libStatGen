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

#include <vector>
#include <map>

#include "VcfRecordField.h"


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
    const std::string* getValue(const std::string& key, 
                                int sampleNum);

    /// Get the number of samples.
    /// \param key to find the falue for.
    /// \return const pointer to the string value for this key, NULL if
    /// the key was not found, a pointer to an empty string if the key
    /// does not have a value.
    const int getNumSamples();

protected:

private:
    template <class DATA_TYPE>
    class SmartVector
    {
    public:
        SmartVector(): myCont(), myNextEmpty(0) {}
        virtual ~SmartVector();
        void reset();
        void clear() {reset();}
        DATA_TYPE& getNextEmpty();
        DATA_TYPE* get(unsigned int index);
        // The next empty position is the same as the size.
        int size() {return(myNextEmpty);}
        // Does not print the starting/trailing '\t'
        virtual bool write(IFILE filePtr) = 0;
    protected:
        std::vector<DATA_TYPE*> myCont;
        unsigned int myNextEmpty;
    };
 
    class SmartStringVector : public SmartVector<std::string>
    {
    public:
        SmartStringVector()
            : SmartVector<std::string>()
        {}
        virtual ~SmartStringVector(){}
        // Does not print the starting/trailing '\t'
        bool write(IFILE filePtr);
    };

    class SmartVectorOfStringVectors : public SmartVector<SmartStringVector>
    {
    public:
        SmartVectorOfStringVectors()
            : SmartVector<SmartStringVector>()
        {}
        virtual ~SmartVectorOfStringVectors(){}
        // Does not print the starting/trailing '\t'
        bool write(IFILE filePtr);
    };

    //   std::map<const std::string*, int> myTypeToPos;

    // Format field indexed by position with the value
    // as the datatype.
    SmartStringVector myPosToType;
    
    SmartVectorOfStringVectors mySamples;
};

/////////////////////////////////////////////////////////////
// SmartVector
template <class DATA_TYPE>
VcfRecordGenotype::SmartVector<DATA_TYPE>::~SmartVector()
{
    for(unsigned int i = 0; i < myCont.size(); i++)
    {
        // Delete all the entries.
        delete myCont[i];
        myCont[i] = NULL;
    }
    myCont.clear();
    myNextEmpty = 0;
}
template <class DATA_TYPE>
void VcfRecordGenotype::SmartVector<DATA_TYPE>::reset()
{
    // Set the next empty element to be the first one on the list.
    // That means there are none used.
    myNextEmpty = 0;
}
        
template <class DATA_TYPE>
DATA_TYPE& VcfRecordGenotype::SmartVector<DATA_TYPE>::getNextEmpty()
{
    if(myNextEmpty == myCont.size())
    {
        // We are at the end of the available entries, so add a new one.
        myCont.resize(myCont.size() + 1);

        // Create a new string.
        myCont[myNextEmpty] = new DATA_TYPE;
    }
    else
    {
        // myNextEmpty is an element, and not the end.
        // So, clear out the string.
        myCont[myNextEmpty]->clear();
    }

    DATA_TYPE* returnVal = myCont[myNextEmpty];

    // Increment next empty to the next element.
    ++myNextEmpty;
    // return the element to be used.
    return(*returnVal);
}


template <class DATA_TYPE>
DATA_TYPE* VcfRecordGenotype::SmartVector<DATA_TYPE>::get(unsigned int index)
{
    if((index < myNextEmpty) && (index >= 0))
    {
        // index is a valid position, so return that string.
        return(myCont[index]);
    }

    // Not set in the vector, so return null.
    return(NULL);
}


#endif
