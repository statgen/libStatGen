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

#include "ReadFiles.h"
#include "AspFile.h"
#include <assert.h>

void testRead()
{
    AspFileReader inFile;
    const AspRecord* recPtr = NULL;
    AspRecord record;

    assert(inFile.open("testFiles/asp.asp"));


    recPtr = &(inFile.getRecord("1", 76));
    recPtr = &(inFile.getRecord("1", 10016));
    recPtr = &(inFile.getRecord("3", 76));

    inFile.close();

    ////////////////////////////////////////
    // Test advanceToNextChromosome
    assert(inFile.open("testFiles/asp.asp"));

    std::string nextChrom;
    std::string chromName;

    assert(inFile.advanceToNextChromosome(nextChrom));
    assert(nextChrom == "1");
    assert(inFile.getNextDataRecord(record));
    assert(inFile.getChromName(record.getChromID(), chromName));
    assert(chromName == "1");
    assert(strcmp(inFile.getChromName(record.getChromID()), "1") == 0);
    assert(inFile.getNextDataRecord(record));
    assert(inFile.advanceToNextChromosome(nextChrom));
    assert(nextChrom == "2");
    assert(inFile.getNextDataRecord(record));
    assert(inFile.getChromName(record.getChromID(), chromName));
    assert(chromName == "2");
    assert(strcmp(inFile.getChromName(record.getChromID()), "2") == 0);
    assert(inFile.getNextDataRecord(record));
    assert(inFile.getChromName(record.getChromID(), chromName));
    assert(chromName == "2");
    assert(strcmp(inFile.getChromName(record.getChromID()), "2") == 0);
    assert(inFile.advanceToNextChromosome(nextChrom));
    assert(nextChrom == "3");
    assert(inFile.getNextDataRecord(record));
    assert(inFile.getNextDataRecord(record));
    assert(inFile.advanceToNextChromosome(nextChrom) == false);
    assert(nextChrom == "3");
    assert(inFile.getNextDataRecord(record) == false);

    assert(inFile.getChromName(55, chromName) == false);
    assert(chromName == "*");
    assert(inFile.getChromName(55) == NULL);

    inFile.close();
   
}

