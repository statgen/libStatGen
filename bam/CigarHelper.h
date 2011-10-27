/*
 *  Copyright (C) 2011  Regents of the University of Michigan
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

#ifndef __CIGAR_HELPER_H__
#define __CIGAR_HELPER_H__

#include "SamRecord.h"

/// Class for helping to filter a SAM/BAM record.
class CigarHelper
{
public:
    // 
    static const int32_t NO_CLIP = -1;


    /// Soft clip the cigar from the back of the read at the specified
    /// reference position.  If the clip position is deleted/skipped
    /// or is immediately preceded by a deletion/skip/pad, that entire CIGAR
    /// operation is also removed.  If the clip position is immediately
    /// preceded by an insertion, the insertion is left in the CIGAR.
    /// Nothing is clipped if the reference position does not fall within
    /// the region spanned by the record.
    /// \param record record to be clipped (input/output parameter).
    /// \param refPosition0Based 0-based reference position to start clip at
    /// \param newCigar cigar object to set with the updated cigar.
    /// \param read position where the clip starts or 
    //         NO_CLIP if nothing is clipped.
    static int32_t softClipEndByRefPos(SamRecord& record, 
                                       int32_t refPosition0Based,
                                       CigarRoller& newCigar);
};

#endif

