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

#ifndef __SAM_RECORD_HELPER_H__
#define __SAM_RECORD_HELPER_H__

#include "SamRecord.h"

/// Class for extracting information from a SAM Flag.
class SamRecordHelper
{
public:

    /// Helper method that checks if the record's read sequence starting
    /// at the specified 0-based reference position matches the passed in
    /// sequence.
    static bool checkSequence(SamRecord& record, int32_t pos0Based, 
                              const char* sequence);

private:
    SamRecordHelper();
};


#endif
