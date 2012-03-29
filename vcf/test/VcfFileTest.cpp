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

#include "VcfFileTest.h"
#include "VcfFileReader.h"
#include "VcfHeaderTest.h"
#include <assert.h>

const std::string HEADER_LINE_SUBSET1="#CHROM	POS	ID	REF	ALT	QUAL	FILTER	INFO	FORMAT	NA00001	NA00002";
const std::string HEADER_LINE_SUBSET2="#CHROM	POS	ID	REF	ALT	QUAL	FILTER	INFO	FORMAT	NA00002	NA00003";
const int NUM_SAMPLES_SUBSET1 = 2;
const int NUM_SAMPLES_SUBSET2 = 2;

void testVcfFile()
{
    //    VcfFileHeader header;

    // Test open for read via the constructor with return.
    VcfFileReader reader;
    VcfHeader header;
    VcfRecord record;
    // Try reading without opening.
    bool caughtException = false;
    try
    {
        assert(reader.readRecord(record) == false);
    }
    catch (std::exception& e) 
    {
        caughtException = true;
    }

    assert(caughtException);


    // Try opening a file that doesn't exist.
    caughtException = false;
    try
    {
        assert(reader.open("fileDoesNotExist.txt", header) == false);
    }
    catch (std::exception& e) 
    {
        caughtException = true;
    }
    assert(caughtException);
// "testFiles/testVcf.vcf");
//     assert(vcfInConstructorReadDefault.WriteHeader(header) == false);
//     assert(vcfInConstructorReadDefault.ReadHeader(header) == true);

//     // Test open for write via the constructor.
//     VcfFile vcfInConstructorWrite("results/newWrite.vcf", VcfFile::WRITE,
//                                   ErrorHandler::RETURN);
//     assert(vcfInConstructorWrite.ReadHeader(header) == false);
//     assert(vcfInConstructorWrite.WriteHeader(header) == true);

//     // Test open for read via the constructor
//     VcfFile vcfInConstructorRead("testFiles/testVcf.vcf", VcfFile::READ);
//     bool caughtException = false;
//     try
//     {
//         assert(vcfInConstructorRead.WriteHeader(header) == false);
//     }
//     catch (std::exception& e) 
//     {
//         caughtException = true;
//     }
//     assert(caughtException);
//     assert(vcfInConstructorRead.ReadHeader(header) == true);

//     // Test open for write via child class.
//     VcfFileWriter vcfWriteConstructor("results/newWrite1.vcf");
//     caughtException = false;
//     try
//     {
//         assert(vcfWriteConstructor.ReadHeader(header) == false);
//     }
//     catch (std::exception& e) 
//     {
//         caughtException = true;
//     }
//     assert(caughtException);
//     assert(vcfWriteConstructor.WriteHeader(header) == true);

//     // Test open for read via child class.
//     VcfFileReader vcfReadConstructor("testFiles/testVcf.vcf");
//     caughtException = false;
//     try
//     {
//         assert(vcfReadConstructor.WriteHeader(header) == false);
//     }
//     catch (std::exception& e) 
//     {
//         caughtException = true;
//     }
//     assert(caughtException);
//     assert(vcfReadConstructor.ReadHeader(header) == true);


    ////////////////////////////////
    // Test the subset logic.
    VcfRecordGenotype* sampleInfo = NULL;

    reader.open("testFiles/vcfFile.vcf", header, "testFiles/subset1.txt", ";");

    assert(header.getHeaderLine() == HEADER_LINE_SUBSET1);
    assert(header.getNumSamples() == NUM_SAMPLES_SUBSET1);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == -1);
 
    // Read the records to make sure they were subset.
    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0/0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == true);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == false);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "1|2");
    assert(*(sampleInfo->getString("GT", 1)) == "2|1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 2);

    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 0);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == true);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0/1");
    assert(*(sampleInfo->getString("GT", 1)) == "0/2");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == true);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 2);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == false);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(sampleInfo->getString("GT", 0) == NULL);
    assert(sampleInfo->getString("GT", 1) == NULL);
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 2);

    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == false);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|1");
    assert(*(sampleInfo->getString("GT", 1)) == "0|.");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 7);
    assert(reader.getNumRecords() == 7);

    reader.close();

    //////////////////////////
    // Subset with a different file.
    reader.open("testFiles/vcfFile.vcf", header, "testFiles/subset2.txt");
    
    assert(header.getHeaderLine() == HEADER_LINE_SUBSET2);
    assert(header.getNumSamples() == NUM_SAMPLES_SUBSET2);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == SAMPLES[1]);
    assert(header.getSampleName(1) == SAMPLES[2]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == -1);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == 1);

    // Read the records to make sure they were subset.
    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "1|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1/1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->allPhased() == false);
    assert(sampleInfo->allUnphased() == false);
    assert(sampleInfo->hasAllGenotypeAlleles() == true);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|1");
    assert(*(sampleInfo->getString("GT", 1)) == "0/0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->allPhased() == false);
    assert(sampleInfo->allUnphased() == false);
    assert(sampleInfo->hasAllGenotypeAlleles() == true);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "2|1");
    assert(*(sampleInfo->getString("GT", 1)) == "2/2");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->allPhased() == false);
    assert(sampleInfo->allUnphased() == false);
    assert(sampleInfo->hasAllGenotypeAlleles() == true);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0/0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->allPhased() == false);
    assert(sampleInfo->allUnphased() == false);
    assert(sampleInfo->hasAllGenotypeAlleles() == true);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == true);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0/2");
    assert(*(sampleInfo->getString("GT", 1)) == "1/1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->allPhased() == false);
    assert(sampleInfo->allUnphased() == true);
    assert(sampleInfo->hasAllGenotypeAlleles() == true);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == true);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == false);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(sampleInfo->getString("GT", 0) == NULL);
    assert(sampleInfo->getString("GT", 1) == NULL);
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->allPhased() == false);
    assert(sampleInfo->allUnphased() == false);
    assert(sampleInfo->hasAllGenotypeAlleles() == false);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == false);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|.");
    assert(*(sampleInfo->getString("GT", 1)) == "1|1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->allPhased() == true);
    assert(sampleInfo->allUnphased() == false);
    assert(sampleInfo->hasAllGenotypeAlleles() == false);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 7);
    assert(reader.getNumRecords() == 7);


    //////////////////////////
    // Add in discarding non-phased.
    reader.setDiscardRules(VcfFileReader::DISCARD_NON_PHASED);
    reader.open("testFiles/vcfFile.vcf", header, "testFiles/subset1.txt", ";");

    assert(header.getHeaderLine() == HEADER_LINE_SUBSET1);
    assert(header.getNumSamples() == NUM_SAMPLES_SUBSET1);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == -1);
 
    // Read the records to make sure they were subset.
    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "1|2");
    assert(*(sampleInfo->getString("GT", 1)) == "2|1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|1");
    assert(*(sampleInfo->getString("GT", 1)) == "0|.");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 4);
    assert(reader.getNumRecords() == 7);

    reader.close();

    //////////////////////////
    // Discard missing GTs.
    reader.setDiscardRules(VcfFileReader::DISCARD_MISSING_GT);
    reader.open("testFiles/vcfFile.vcf", header, "testFiles/subset1.txt", ";");

    assert(header.getHeaderLine() == HEADER_LINE_SUBSET1);
    assert(header.getNumSamples() == NUM_SAMPLES_SUBSET1);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == -1);
 
    // Read the records to make sure they were subset.
    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0/0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == true);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "1|2");
    assert(*(sampleInfo->getString("GT", 1)) == "2|1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0/1");
    assert(*(sampleInfo->getString("GT", 1)) == "0/2");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == true);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 5);
    assert(reader.getNumRecords() == 7);

    //////////////////////////
    // Discard missing GTs & non-Phased.
    reader.setDiscardRules(VcfFileReader::DISCARD_MISSING_GT | 
                           VcfFileReader::DISCARD_NON_PHASED);
    reader.open("testFiles/vcfFile.vcf", header, "testFiles/subset1.txt", ";");

    assert(header.getHeaderLine() == HEADER_LINE_SUBSET1);
    assert(header.getNumSamples() == NUM_SAMPLES_SUBSET1);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == -1);
 
    // Read the records to make sure they were subset.
    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "1|2");
    assert(*(sampleInfo->getString("GT", 1)) == "2|1");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record));
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 3);
    assert(reader.getNumRecords() == 7);

    reader.close();


    ////////////////////////////////
    // Test Discarding filtered without subsetting.
    reader.open("testFiles/vcfFile.vcf", header);

    reader.setDiscardRules(VcfFileReader::DISCARD_FILTERED); 

    assert(header.getHeaderLine() == HEADER_LINE);
    assert(header.getNumSamples() == NUM_SAMPLES);
    assert(header.getSampleName(2) == SAMPLES[2]);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == 2);
 
    // Read the records.
    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1|0");
    assert(*(sampleInfo->getString("GT", 2)) == "1/1");
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == true);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "1|2");
    assert(*(sampleInfo->getString("GT", 1)) == "2|1");
    assert(*(sampleInfo->getString("GT", 2)) == "2/2");
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == true);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 2);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|0");
    assert(*(sampleInfo->getString("GT", 2)) == "0/0");
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == true);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 0);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == true);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0/1");
    assert(*(sampleInfo->getString("GT", 1)) == "0/2");
    assert(*(sampleInfo->getString("GT", 2)) == "1/1");
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == true);
    assert(sampleInfo->isUnphased(1) == true);
    assert(sampleInfo->isUnphased(2) == true);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 2);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == false);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(sampleInfo->getString("GT", 0) == NULL);
    assert(sampleInfo->getString("GT", 1) == NULL);
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == false);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 2);

    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == false);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0|1");
    assert(*(sampleInfo->getString("GT", 1)) == "0|.");
    assert(*(sampleInfo->getString("GT", 2)) == "1|1");
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == true);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 6);
    assert(reader.getNumRecords() == 7);

    reader.close();

    ////////////////////////////////
    // Test Discarding multiple Alts without subsetting.
    reader.open("testFiles/vcfFile.vcf", header);

    reader.setDiscardRules(VcfFileReader::DISCARD_MULTIPLE_ALTS); 

    assert(header.getHeaderLine() == HEADER_LINE);
    assert(header.getNumSamples() == NUM_SAMPLES);
    assert(header.getSampleName(2) == SAMPLES[2]);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == 2);
 
    // Read the records.
    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1|0");
    assert(*(sampleInfo->getString("GT", 2)) == "1/1");
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == true);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0/0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|1");
    assert(*(sampleInfo->getString("GT", 2)) == "0/0");
    assert(sampleInfo->isPhased(0) == false);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == true);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == true);
    assert(record.passedAllFilters() == false);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record));
    assert(record.allPhased() == false);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|0");
    assert(*(sampleInfo->getString("GT", 2)) == "0/0");
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == true);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 0);

    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == false);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 3);
    assert(*(sampleInfo->getString("GT", 0)) == "0|1");
    assert(*(sampleInfo->getString("GT", 1)) == "0|.");
    assert(*(sampleInfo->getString("GT", 2)) == "1|1");
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == true);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 4);
    assert(reader.getNumRecords() == 7);

    reader.close();

    ////////////////////////////////
    // Test subsetting and discarding multiple Alts, filter failures,
    // non-phased, and missing genotypes.
    reader.open("testFiles/vcfFile.vcf", header);

    reader.setDiscardRules(VcfFileReader::DISCARD_MULTIPLE_ALTS|
                           VcfFileReader::DISCARD_FILTERED |
                           VcfFileReader::DISCARD_MISSING_GT | 
                           VcfFileReader::DISCARD_NON_PHASED);
    reader.open("testFiles/vcfFile.vcf", header, "testFiles/subset1.txt", ";");

    assert(header.getHeaderLine() == HEADER_LINE_SUBSET1);
    assert(header.getNumSamples() == NUM_SAMPLES_SUBSET1);
    assert(header.getSampleName(2) == NULL);
    assert(header.getSampleName(0) == SAMPLES[0]);
    assert(header.getSampleName(1) == SAMPLES[1]);
    assert(header.getSampleIndex(SAMPLES[1].c_str()) == 1);
    assert(header.getSampleIndex(SAMPLES[0].c_str()) == 0);
    assert(header.getSampleIndex(SAMPLES[2].c_str()) == -1);
 
    // Read the records to make sure they were subset.
    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "1|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 1);

    assert(reader.readRecord(record));
    assert(record.allPhased() == true);
    assert(record.allUnphased() == false);
    assert(record.hasAllGenotypeAlleles() == true);
    sampleInfo = &(record.getGenotypeInfo());
    assert(sampleInfo->getNumSamples() == 2);
    assert(*(sampleInfo->getString("GT", 0)) == "0|0");
    assert(*(sampleInfo->getString("GT", 1)) == "0|0");
    assert(sampleInfo->getString("GT", 2) == NULL);
    assert(sampleInfo->isPhased(0) == true);
    assert(sampleInfo->isPhased(1) == true);
    assert(sampleInfo->isPhased(2) == false);
    assert(sampleInfo->isUnphased(0) == false);
    assert(sampleInfo->isUnphased(1) == false);
    assert(sampleInfo->isUnphased(2) == false);
    assert(record.passedAllFilters() == true);
    assert(record.getNumAlts() == 0);

    assert(reader.readRecord(record) == false);

    assert(reader.getNumKeptRecords() == 2);
    assert(reader.getNumRecords() == 7);

    reader.close();
}
