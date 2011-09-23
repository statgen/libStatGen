/*
 *  Copyright (C) 2010  Regents of the University of Michigan
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

#include "assert.h"
#include "ctype.h"
#include "stdio.h"
#include "zlib.h"
#include "Error.h"


#include "Generic.h"
#include "ReferenceSequence.h"

#include <algorithm>
#include <istream>
#include <fstream>
#include <sstream>
#include <stdexcept>

//
// Given a buffer with a fasta format contents, count
// the number of chromsomes in it and return that value.
//
bool getFastaStats(const char *fastaData, size_t fastaDataSize, uint32_t &chromosomeCount, uint64_t &baseCount)
{
    chromosomeCount = 0;
    baseCount = 0;
    bool atLineStart = true;

    //
    // loop over the fasta file, essentially matching for the
    // pattern '^>.*$' and counting them.
    //
    for (size_t fastaIndex = 0; fastaIndex < fastaDataSize; fastaIndex++)
    {
        switch (fastaData[fastaIndex])
        {
            case '\n':
            case '\r':
                atLineStart = true;
                break;
            case '>':
            {
                if (!atLineStart) break;
                chromosomeCount++;
                //
                // eat the rest of the line
                //
                while (fastaIndex < fastaDataSize && fastaData[fastaIndex]!='\n' && fastaData[fastaIndex]!='\r')
                {
                    fastaIndex++;
                }
                break;
            }
            default:
                baseCount++;
                atLineStart = false;
                break;
        }

    }
    return false;
}

//
// Load a fasta format file from filename into the buffer
// provided by the caller.
// While parsing the fasta file, record each chromosome name,
// its start location, and its size.
//
// NB: the caller must implement the logic to determine how
// large the sequence data is.  There is no correct way to do
// this, because we can't reliably estimate here how much sequence
// data is contained in a compressed file.
//
// To safely pre-allocate space in sequenceData, use the reserve() method
// before calling this function.
//
// XXX consider turning this into a template as well:
//
bool loadFastaFile(const char *filename,
        std::vector<PackedSequenceData> &sequenceData,
        std::vector<std::string> &chromosomeNames)
{
    InputFile inputStream(filename, "r", InputFile::DEFAULT);

    if(!inputStream.isOpen()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return true;
    }

    int whichChromosome = -1;
    chromosomeNames.clear();

    char ch;
    while((ch = inputStream.ifgetc()) != EOF) {
        switch (ch)
        {
            case '\n':
            case '\r':
                break;
            case '>':
            {
                std::string chromosomeName = "";
                //
                // pull out the chromosome new name
                //
                while (!isspace((ch = inputStream.ifgetc())) && ch != EOF)
                {
                    chromosomeName += ch;  // slow, but who cares
                }
                //
                // eat the rest of the line
                //
                do {
                    ch = inputStream.ifgetc();
                } while(ch != EOF && ch != '\n' && ch != '\r');
                //
                // save the Chromosome name and index into our
                // header so we can use them later.
                //
                chromosomeNames.push_back(chromosomeName);

                whichChromosome++;

                break;
            }
            default:
                // we get here for sequence data.
                //
                // save the base value
                // Note: invalid characters come here as well, but we
                // let ::set deal with mapping them.
#if 0
                if (isColorSpace())
                {
//
// anything outside these values represents an invalid base
// base codes: 0-> A,    1-> C,     2-> G,      3-> T
// colorspace: 0-> blue, 1-> green, 2-> oragne, 3->red
//
                    const char fromBase2CS[] =
                    {
                        /* 0000 */ 0,   // A->A
                        /* 0001 */ 1,   // A->C
                        /* 0010 */ 2,   // A->G
                        /* 0011 */ 3,   // A->T
                        /* 0100 */ 1,   // C->A
                        /* 0101 */ 0,   // C->C
                        /* 0110 */ 3,   // C->G
                        /* 0111 */ 2,   // C->T
                        /* 1000 */ 2,   // G->A
                        /* 1001 */ 3,   // G->C
                        /* 1010 */ 0,   // G->G
                        /* 1011 */ 1,   // G->T
                        /* 1100 */ 3,   // T->A
                        /* 1101 */ 2,   // T->C
                        /* 1110 */ 1,   // T->G
                        /* 1111 */ 0,   // T->T
                    };
                    //
                    // we are writing color space values on transitions,
                    // so we don't write a colorspace value when we
                    // get the first base value.
                    //
                    // On second and subsequent bases, write based on
                    // the index table above
                    //
                    char thisBase = base2int[(int)(fasta[fastaIndex])];
                    if (lastBase>=0)
                    {
                        char color;
                        if (lastBase>3 || thisBase>3) color=4;
                        else color = fromBase2CS[(int)(lastBase<<2 | thisBase)];
                        // re-use the int to base, because ::set expects a base char (ATCG), not
                        // a color code (0123).  It should only matter on final output.
                        set(header->elementCount++, int2base[(int) color]);
                    }
                    lastBase = thisBase;
                }
                else
                {
                    set(header->elementCount++, toupper(fasta[fastaIndex]));
                }
#endif
                break;
        }
    }
    return false;
}

#if 0
// turn this into a template on read/quality/etc...
int GenomeSequence::debugPrintReadValidation(
    std::string &read,
    std::string &quality,
    char   direction,
    genomeIndex_t   readLocation,
    int sumQuality,
    int mismatchCount,
    bool recurse
) 
{
    int validateSumQ = 0;
    int validateMismatchCount = 0;
    int rc = 0;
    std::string genomeData;

    for (uint32_t i=0; i<read.size(); i++)
    {
        if (tolower(read[i]) != tolower((*this)[readLocation + i]))
        {
            validateSumQ += quality[i] - '!';
            // XXX no longer valid:
            if (direction=='F' ? i<24 : (i >= (read.size() - 24))) validateMismatchCount++;
            genomeData.push_back(tolower((*this)[readLocation + i]));
        }
        else
        {
            genomeData.push_back(toupper((*this)[readLocation + i]));
        }
    }
    assert(validateSumQ>=0);
    if (validateSumQ != sumQuality && validateMismatchCount == mismatchCount)
    {
        printf("SUMQ: Original Genome: %s  test read: %s : actual sumQ = %d, test sumQ = %d\n",
               genomeData.c_str(),
               read.c_str(),
               validateSumQ,
               sumQuality
              );
        rc++;
    }
    else if (validateSumQ == sumQuality && validateMismatchCount != mismatchCount)
    {
        printf("MISM: Original Genome: %s  test read: %s : actual mismatch %d test mismatches %d\n",
               genomeData.c_str(),
               read.c_str(),
               validateMismatchCount,
               mismatchCount
              );
        rc++;
    }
    else if (validateSumQ != sumQuality && validateMismatchCount != mismatchCount)
    {
        printf("BOTH: Original Genome: %s  test read: %s : actual sumQ = %d, test sumQ = %d, actual mismatch %d test mismatches %d\n",
               genomeData.c_str(),
               read.c_str(),
               validateSumQ,
               sumQuality,
               validateMismatchCount,
               mismatchCount
              );
        rc++;
    }

    if (recurse && abs(validateMismatchCount - mismatchCount) > (int) read.size()/2)
    {
        printf("large mismatch difference, trying reverse strand: ");
        std::string reverseRead = read;
        std::string reverseQuality = quality;
        getReverseRead(reverseRead);
        reverse(reverseQuality.begin(), reverseQuality.end());
        rc = debugPrintReadValidation(reverseRead, reverseQuality, readLocation, sumQuality, mismatchCount, false);
    }
    return rc;
}
#endif




#if defined(TEST)

void simplestExample(void)
{
    GenomeSequence  reference;
    genomeIndex_t   index;

    // a particular reference is set by:
    // reference.setFastaName("/usr/cluster/share/karma/human_g1k_v37_12CS.fa")
    //
    // In the above example, the suffix .fa is stripped and replaced with .umfa,
    // which contains the actual file being opened.
    //
    if (reference.open())
    {
        perror("GenomeSequence::open");
        exit(1);
    }


    index = 1000000000; // 10^9
    //
    // Write the base at the given index.  Here, index is 0 based,
    // and is across the whole genome, as all chromosomes are sequentially
    // concatenated, so the allowed range is
    //
    // 0.. (reference.getChromosomeStart(last) + reference.getChromosomeSize(last))
    //
    // (where int last = reference.getChromosomeCount() - 1;)
    //
    std::cout << "base[" << index << "] = " << reference[index] << std::endl;

    //
    // Example for finding chromosome and one based chromosome position given
    // and absolute position on the genome in 'index':
    //
    int chr = reference.getChromosome(index);
    genomeIndex_t   chrIndex = index - reference.getChromosomeStart(chr) + 1;   // 1-based

    std::cout << "genome index " << index << " corresponds to chromosome " << chr << " position " << chrIndex << std::endl;

    //
    // Example for finding an absolute genome index position when the
    // chromosome name and one based position are known:
    //
    const char *chromosomeName = "5";
    chr = reference.getChromosome(chromosomeName);     // 0-based
    chrIndex = 100000;                      // 1-based

    index = reference.getChromosomeStart(chr) + chrIndex - 1;

    std::cout << "Chromosome '" << chromosomeName << "' position " << chrIndex << " corresponds to genome index position " << index << std::endl;

    reference.close();
}

void testGenomeSequence(void)
{
    GenomeSequence reference;

#if 0
    std::string referenceName = "someotherreference";
    if (reference.setFastaName(referenceName))
    {
        std::cerr << "failed to open reference file "
                  << referenceName
                  << std::endl;
        exit(1);
    }
#endif

    std::cerr << "open and prefetch the reference genome: ";

    // open it
    if (reference.open())
    {
        exit(1);
    }
    std::cerr << "done!" << std::endl;

    //
    // For the human genome, genomeIndex ranges from 0 to 3.2x10^9
    //
    genomeIndex_t   genomeIndex;    // 0 based
    unsigned int chromosomeIndex;   // 1 based
    unsigned int chromosome;        // 0..23 or so
    std::string chromosomeName;

    //
    // Here we'll start with a chromosome name, then obtain the genome
    // index, and use it to find the base we want:
    //
    chromosomeName = "2";
    chromosomeIndex = 1234567;
    // this call is slow (string search for chromsomeName):
    genomeIndex = reference.getGenomePosition(chromosomeName.c_str(), chromosomeIndex);
    assert(genomeIndex!=INVALID_GENOME_INDEX);
    std::cout << "Chromosome " << chromosomeName << ", index ";
    std::cout << chromosomeIndex << " contains base " << reference[genomeIndex];
    std::cout << " at genome index position " << genomeIndex << std::endl;

    //
    // now reverse it - given a genomeIndex from above, find the chromosome
    // name and index:
    //

    // slow (binary search on genomeIndex):
    chromosome = reference.getChromosome(genomeIndex);
    unsigned int newChromosomeIndex;
    // not slow:
    newChromosomeIndex = genomeIndex - reference.getChromosomeStart(chromosome) + 1;

    assert(chromosomeIndex == newChromosomeIndex);

}

//
// After I build libcsg, I compile and run this test code using:
//
// g++ -DTEST -o try GenomeSequence.cpp -L. -lcsg -lm -lz -lssl
// you also may need -fno-rtti
// ./try
//
int main(int argc, const char **argv)
{
#if 1
    simplestExample();
#else
    testGenomeSequence();
#endif
    exit(0);
}
#endif
