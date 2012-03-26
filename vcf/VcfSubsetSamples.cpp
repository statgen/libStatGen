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

#include "VcfSubsetSamples.h"

bool VcfSubsetSamples::init(VcfHeader& header, const char* sampleFileName, 
                            const char* delims)
{
    // Setup the sample list.
    if(!mySampleList.readFromFile(sampleFileName, delims))
    {
        // Failed, so return.
        return(false);
    }

    int origNumSamples = header.getNumSamples();

    // Resise the sampleSubsetIndicator to nothing to clear it out.
    mySampleSubsetIndicator.resize(0);

    // Now resize sampleSubsetIndicator to indicate that all of the original
    // samples are to be kept.  The ones that are not to be kept will be 
    // modified to be unkept (false).
    // mySampleSubsetIndicator is sized to the original number of samples
    // so it can be used when reading records to determine which ones should
    // be removed/kept.
    mySampleSubsetIndicator.resize(origNumSamples, true);

    // if no samples, return.
    if(origNumSamples == 0)
    {
        return(true);
    }

    // Now that the sample list is setup and the indicator vector is setup,
    // subset the header removing samples not found in the subset list.
    // Loop from the back of the samples to the beginning since
    // removing samples changes the index of all following samples.
    for(int i = (origNumSamples-1); i >= 0; i--)
    {
        // Check if the sample should be kept.
        if(!mySampleList.contains(header.getSampleName(i)))
        {
            // This sample should be removed since it is not in the subset.
            header.removeSample(i);
            mySampleSubsetIndicator[i] = false;
        }
    }
    return(true);
}


bool VcfSubsetSamples::keep(unsigned int sampleIndex)
{
    if(sampleIndex >= mySampleSubsetIndicator.size())
    {
        // index out of range.
        return(false);
    }
    return(mySampleSubsetIndicator[sampleIndex]);
}


void VcfSubsetSamples::reset()
{
    mySampleList.clear();
    mySampleSubsetIndicator.clear();
}
