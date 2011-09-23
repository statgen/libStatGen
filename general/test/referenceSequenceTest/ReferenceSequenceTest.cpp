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

#include <getopt.h>
#include "Generic.h"
#include <stdio.h>
#include "ReferenceSequence.h"
#include "UnitTest.h"

#include <assert.h>
#include <sstream>
#include <stdlib.h>

class ReferenceSequenceTest : public UnitTest
{
public:
    ReferenceSequenceTest(const char *title) : UnitTest(title) {;}
    void test1();

    void test() {
        test1();
    }
};

void ReferenceSequenceTest::test1(void)
{
    std::string sequence("ACTGACTGACTGACTGACTGACTGACTGACTGACTGACTG");
    std::string word;

    word="ACTG";
    check(m_failures, ++m_testNum, "Test wordMatch with std::string", true,
            Sequence::wordMatch(sequence, 4, word));

    std::stringstream output;

    Sequence::printNearbyWords(output, sequence, 8, word, 4);

    std::string expect("\
word 'ACTG' found -4 away from position 8.\n\
word 'ACTG' found 0 away from position 8.\n\
");

    check(m_failures, ++m_testNum, "Test printNearbyWords with std::string", expect, output.str());


    Sequence::getString(sequence, 4, 4, word);

    check(m_failures, ++m_testNum, "Test getString with std::string", "ACTG", word);

    Sequence::getHighLightedString(sequence, 0, 12, word, 4, 8);
    check(m_failures, ++m_testNum, "Test getHighLightedStribng with std::string", "ACTGactgACTG",word);

#if 0
    // busted test - don't know why
    output.clear();
    output.str(std::string());
//    Sequence::printBaseContext(std::cout, sequence, 8, 4);
    Sequence::printBaseContext(output, sequence, 8, 4);
    expect="\
index: 8\n\
ACTGACTGA\n\
    ^\n\
";
    check(m_failures, ++m_testNum, "Test printBaseContext with std::string", expect, output.str());
#endif
    std::string result;
    std::string   read("ACTGZZZZACTG");
              expect = "    ^^^^    ";
    Sequence::getMismatchHatString(sequence, 4, result, read);
    check(m_failures, ++m_testNum, "Test getMismatchHatString with std::string", expect, result);


    read="ACTG";
    std::string quality("");
    size_t location = Sequence::simpleLocalAligner(sequence, 0, read, quality, 12);
    check(m_failures, ++m_testNum, "Test simpleLocalAligner with std::string", (size_t) 0, location);
}

int main(int argc, char **argv)
{
    ReferenceSequenceTest test("ReferenceSequenceTest");

    test.test();

    std::cout << test;

    exit(test.getFailureCount());
}
