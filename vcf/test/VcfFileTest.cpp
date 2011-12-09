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
#include <assert.h>

void testVcfFile()
{
    //    VcfFileHeader header;

    // Test open for read via the constructor with return.
    VcfFileReader vcfReader;
    VcfRecord record;
    // Try reading without opening.
    bool caughtException = false;
    try
    {
        assert(vcfReader.readRecord(record) == false);
    }
    catch (std::exception& e) 
    {
        caughtException = true;
    }
    //TODO    assert(vcfReader.getStatus() == StatGenStatus::FAIL_ORDER);

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
}
