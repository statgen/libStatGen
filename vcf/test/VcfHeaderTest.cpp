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

#include "VcfHeaderTest.h"
#include "VcfHeader.h"
#include <assert.h>

//extern const std::string HEADER_LINE="#CHROM	POS	ID	REF	ALT	QUAL	FILTER	INFO	FORMAT	NA00001	NA00002	NA00003";
//extern const std::string SAMPLES[NUM_SAMPLES] = {"NA00001","NA00002","NA00003"};
//extern const std::string META_LINES[NUM_META_LINES]

void testVcfHeader()
{
    VcfHeader header;

    // Test accessing the header without having read anything.
    assert(header.getNumMetaLines() == 0);
    assert(header.getMetaLine(0) == NULL);
    assert(header.getMetaLine(2) == NULL);
    assert(header.getHeaderLine() == NULL);
    assert(header.getNumSamples() == 0);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == NULL);
    assert(header.getSampleName(1) == NULL);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == -1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == -1);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == -1);


    IFILE filePtr = NULL; // Input File
    IFILE outputFile = NULL; // Output File.

    // Try reading without opening.
    bool caughtException = false;
    try
    {
        assert(header.read(filePtr) == false);
    }
    catch (std::exception& e) 
    {
        caughtException = true;
    }
    assert(caughtException);

    // Open the file, then read.
    filePtr = ifopen("testFiles/vcfFile.vcf", "r");
    assert(header.read(filePtr));
    assert(header.getNumMetaLines() == NUM_META_LINES);
    assert(header.getMetaLine(0) == META_LINES[0]);
    assert(header.getMetaLine(2) == META_LINES[2]);
    assert(header.getMetaLine(23) == NULL);
    assert(header.getHeaderLine() == HEADER_LINE);
    assert(header.getNumSamples() == NUM_SAMPLES);
    assert(header.getSampleName(2) == SAMPLES[2]);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == 2);


    // Reset and verify it is empty.
    header.reset();
    assert(header.getNumMetaLines() == 0);
    assert(header.getMetaLine(0) == NULL);
    assert(header.getMetaLine(2) == NULL);
    assert(header.getHeaderLine() == NULL);
    assert(header.getNumSamples() == 0);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == NULL);
    assert(header.getSampleName(1) == NULL);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == -1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == -1);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == -1);

    // Close the file and read again.
    ifclose(filePtr);
    filePtr = ifopen("testFiles/vcfFile.vcf", "r");
    assert(header.read(filePtr));
    assert(header.getNumMetaLines() == NUM_META_LINES);
    assert(header.getMetaLine(0) == META_LINES[0]);
    assert(header.getMetaLine(2) == META_LINES[2]);
    assert(header.getMetaLine(23) == NULL);
    assert(header.getHeaderLine() == HEADER_LINE);
    assert(header.getNumSamples() == NUM_SAMPLES);
    assert(header.getSampleName(2) == SAMPLES[2]);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == 2);

    // Try writing without opening.
    caughtException = false;
    try
    {
        assert(header.write(outputFile) == false);
    }
    catch (std::exception& e) 
    {
        caughtException = true;
    }
    assert(caughtException);
    caughtException = false;

    // write.
    outputFile = ifopen("results/vcfHeader.vcf", "w");
    assert(header.write(outputFile));

}
