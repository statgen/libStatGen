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

#ifndef __SAMPLE_LIST_H__
#define __SAMPLE_LIST_H__

#include <set>
#include <string>

class SampleList
{
public:
    SampleList();
    ~SampleList();

    /// Return true if there are no samples, false if there is at least one.
    inline bool empty() const { return(mySampleList.empty()); }

    /// Return true if the specified sample is found in this container, 
    /// false if not.
    inline bool contains(const char* sample) const
    { return(mySampleList.count(sample) != 0); }

    /// Read the samples from the specified file delimited by any of the
    /// specified delimiters (default is new line).  Return true if the file was
    /// successfully read and stored, false if not.
    /// Does not clear any previously entered values.
    /// In addition to the specified delimiters, new line will also be used.
    bool readFromFile(const char* fileName, const char* delims="\n");

    /// Clear the samples contained in the object.
    inline void clear() { mySampleList.clear(); }

private:
    SampleList(const SampleList& sampleList);

    std::set<std::string> mySampleList;
};

#endif
