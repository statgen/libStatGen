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

#include "BgzfFileTypeRecovery.h"

#include <stdio.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>

#include <fstream>
#include <iostream>
#include <vector>

#pragma pack(push)
#pragma pack(1)

class GzipHeader {
private:
    uint8_t m_ID1;
    uint8_t m_ID2;
    uint8_t m_CM;
    uint8_t m_FLG;
    uint32_t m_MTIME;
    uint8_t m_XFL;
    uint8_t m_OS;
    uint16_t m_XLEN;
public:
    GzipHeader() {;}

    void defaults() {
        m_ID1 = 31;
        m_ID2 = 139;
        m_CM = 8;
        m_FLG = 4;
        m_MTIME = 0;
        m_XFL = 0;
        m_OS = 255;
        m_XLEN = 6;
    }
    uint8_t ID1() {return m_ID1;}
    uint8_t ID2() {return m_ID2;}
    uint8_t CM() {return m_CM;}
    uint8_t FLG() {return m_FLG;}
    uint32_t MTIME() {return m_MTIME;}
    uint8_t XFL() {return m_XFL;}
    uint8_t OS() {return m_OS;}
    uint16_t XLEN() {return m_XLEN;}
    bool sane() {
        return (m_ID1==31 && m_ID2==139 && m_CM==8 && m_FLG==4 && m_MTIME==0 && m_XFL == 0 && m_OS == 255 && m_XLEN==6);
    }
};

class BGZFHeader : public GzipHeader {
private:
    uint8_t m_SI1;
    uint8_t m_SI2;
    uint16_t m_SLEN;    // little endian
    uint16_t m_BSIZE;   // little endian
public:
    BGZFHeader(
            uint8_t m_SI1 = 'B',
            uint8_t m_SI2 = 'C',
            uint16_t m_SLEN = 2,
            uint16_t m_BSIZE = 0
    ) : m_SI1(m_SI1), m_SI2(m_SI2), m_SLEN(m_SLEN), m_BSIZE(m_BSIZE) {;}
    uint8_t SI1() {return m_SI1;}
    uint8_t SI2() {return m_SI2;}
    uint16_t SLEN() {return m_SLEN;}
    uint16_t BSIZE() {return m_BSIZE;}
    bool sane() {
        return GzipHeader::sane() && 
            (m_SI1=='B' && m_SI2=='C' && m_SLEN==2);
    }
};

#pragma pack(pop)

//
// PeekaheadBuffer allows non-destructive peekahead and resyncing
// after read errors when the underlying stream has signatures in the
// data that allow it.
//
// In addition, it has a peek() capability to allow
// the caller to look ahead in the stream to see
// a certain number of bytes before actually consuming them.
//
// The intent is that this class behave as something of a poor
// man's FIFO - with the cost of buffer movement when data is removed.
//
// This is far from ideal, but we basically are moving data around
// when allowing arbitrary peekahead regardless.
//
// The basis for the design is the fact that most read calls to
// various streams at best allow a single character to be peeked
// at, and secondly, do not allow for recovery after an underfling
// framing error occurs.
//
// That is, getchar()/putchar/ungetchar() support a single byte
// peek.  This may be fine for simply parsing applications, but here
// we need to look at blocks up to 64K or more in size to search
// for signatures while re-synchronizing on the underlying stream.
//
class PeekaheadBuffer : public std::vector<uint8_t> {

protected:
    ssize_t m_startPosition;  // start of fresh data

public:
    enum ReturnCode {
        endOfFile = -1,
        reSync = 0,
        ok = 1
    };

    ssize_t startPosition() {return m_startPosition;}

private:
    //
    // when remaining data is 1/8 the size of the full
    // buffer, shift it back down to the start.
    //
    // for use by read(), which will consume data from the buffer.
    //
    void shiftData() {
        if(dataRemaining() < (ssize_t) (std::vector<uint8_t>::size() / 8) ) {
            erase(begin(), begin() + m_startPosition);
            m_startPosition = 0;
        }
    }
    // called when read reports an error for some
    // reason - 
    virtual ReturnCode sync();
public:
    PeekaheadBuffer();
    virtual ~PeekaheadBuffer();

    // return the amount of unused data:
    ssize_t dataRemaining();

    //
    // overload size() to throw an exception - too confusing otherwise
//    size_t size() {abort();}

    //
    // just populate data in buffer from stream - not generic
    //
    // XXX note that it simply ensures that count bytes of data
    // are actually loaded into the buffer - if that amount of
    // data (or more) is present, this call is a NOP.
    //
    virtual ReturnCode readahead(ssize_t count) = 0;

    // read is generic.
    // remove data from our buffer - call non-generic readahead to populate data.
    ReturnCode read(uint8_t *buffer, ssize_t count) {
        ReturnCode rc;

        rc = readahead(count);

        if(rc == ok) {
            uint8_t *src = &(*begin()) + m_startPosition;
            uint8_t *dest = buffer;

            memcpy(dest, src, count);

            m_startPosition += count;  // consume data

            // recover space if wasting too much:
            shiftData();
        } else if(rc == reSync) {
            // peek puked - CRC error, other errors, see if we can sync forwards
            return reSync;
        }  else {
            // failed to get needed data - premature EOF, I guess
            return endOfFile;
        }

        return ok;
    }

};

PeekaheadBuffer::PeekaheadBuffer() : m_startPosition(0)
{
}

PeekaheadBuffer::~PeekaheadBuffer()
{
}

PeekaheadBuffer::ReturnCode PeekaheadBuffer::sync() {
    clear();
    return ok;
}

ssize_t PeekaheadBuffer::dataRemaining()
{
    return std::vector<uint8_t>::size() - m_startPosition;
}


// peekahead buffered file reader class
class FileReader : public PeekaheadBuffer {
    FILE    *m_stream;
public:
    FileReader();
    ~FileReader();
    FileReader(FILE *stream);
    PeekaheadBuffer::ReturnCode readahead(ssize_t count);
    FILE *stream() {return m_stream;}
    bool eof() {return m_stream ? feof(m_stream) : false;}
};

FileReader::FileReader()
{
    m_stream = NULL;
}

FileReader::FileReader(FILE *stream) : m_stream(stream)
{
}

FileReader::~FileReader()
{
    fclose(m_stream);
    m_stream = NULL;
}

//
// fill buffer until we have count bytes of valid
// data.
//
// need to detect error and eof and return appropriate values.
//
PeekaheadBuffer::ReturnCode FileReader::readahead(ssize_t count)
{
    uint8_t buffer[4096];
    while(dataRemaining() < count) {
        int bytesRead = fread(buffer, 1, sizeof(buffer), m_stream);
        if(bytesRead==0) {
            if(ferror(m_stream)) {
                return reSync;
            }
        }
        insert(end(), &buffer[0], &buffer[0] + bytesRead);
    }
    return ok;
}

class BGZFReader : public PeekaheadBuffer {
    FileReader  m_fileReader;

public:

    BGZFReader(FILE *stream) : m_fileReader(stream) {;}

    PeekaheadBuffer::ReturnCode readahead(ssize_t count);

    //
    // This will be reading data, and needs to return EOF, etc
    //
    ReturnCode sync() {
        // my internal data is now bad, so we'll scan ahead seeing
        // if we can find a good header
        clear();
        while(1) {
            BGZFHeader *header;
            PeekaheadBuffer::ReturnCode rc = m_fileReader.readahead(sizeof(header));
            if(rc==endOfFile) return rc;
            // a rc==reSync is ok provided readahead still ensures that header is present
            header = (BGZFHeader *) &(*m_fileReader.begin());
            if(header->sane()) {
                return ok;
            }
            // consume a byte, then see if we're at a valid block header
            uint8_t throwAwayBuffer;
            rc = m_fileReader.read(&throwAwayBuffer, 1);
        }
        // NOTREACHED
        return ok;
    }
    FILE *stream() {return m_fileReader.stream();}

    bool eof() {return m_fileReader.eof();}

};

PeekaheadBuffer::ReturnCode BGZFReader::readahead(ssize_t count)
{
    BGZFHeader header;
    // size of inflateBuffer can be determined from ISIZE, I think
    uint8_t inflateBuffer[4*1024*1024];
    uint8_t gzipBuffer[64*1024];

    while(dataRemaining() < count) {

        // here we actually read data:
        //  read what should be the header
        //  verify the header
        //  read the remainder of the block
        //  check the CRC validity or perhaps just call unzip
        //
        // XXX the sizeof(header) is wrong:
        PeekaheadBuffer::ReturnCode rc = m_fileReader.read((uint8_t *) (&header), sizeof(header));

        if(rc == endOfFile) {
            return endOfFile;
        }

        // if we have a bad header, start looking forward for a good one,
        if(!header.sane()) {
            // sync does not consume the next good header, it simply syncs()
            // the data stream to the next believed good BGZF header:
            sync();
            //
            // even though we can now decompress, we need to tell the caller
            // what is up before they call for more data (caller needs to
            // sync its own record stream):
            return reSync;
        }

        rc = m_fileReader.read((uint8_t *) &gzipBuffer, header.BSIZE() - sizeof(header));

        if(rc == reSync) {
            sync();
            return reSync;
        }

        z_stream zs;
        zs.zalloc = NULL;
        zs.zfree = NULL;
        zs.next_in = gzipBuffer;
        zs.avail_in = header.BSIZE() - 16;  // XXX need to check docs for inflate
        zs.next_out = inflateBuffer;
        zs.avail_out = sizeof(inflateBuffer);

        // -15 --> raw inflate - don't look for gzip or zlib header
        if(     inflateInit2(&zs, -15) == Z_OK &&
                inflate(&zs, Z_FINISH) == Z_STREAM_END &&
                inflateEnd(&zs) == Z_OK) {
            // do something with zs.total_out
            std::cout << "hey, got data!  zs.total_out == " << zs.total_out << "\n";

            // append the newly decompressed data
            insert(end(), &inflateBuffer[0], &inflateBuffer[0] + zs.total_out);
        } else {
            // zlib inflate failure - CRC errors, who knows...
            sync();
            return reSync;
        }
    }

    return ok;

}


#if 0
void testBGZFBuffer()
{
    BGZFReader b(stdin);
    std::vector<uint8_t>::iterator position;
    BGZFReader::ReturnCode rc;

    std::cout << "size = " << b.dataRemaining() << "\n";

    //
    // this should:
    //  decompress a BGZF block, populating the buffer with
    //  unzipped data, possibly returning a BGZFBuffer::ReturnCode of 
    //  resync if it turns out the BGZF data was interrupted by bad
    //  CRC checks.
    //
    rc = b.readahead(64);
    std::cout << "rc = " << rc << " - expect ok (1)\n";
    std::cout << "size (expect 64) = " << b.size() << "\n";
}


int main(int argc, const char **argv)
{
    testBGZFBuffer();
}
#endif



int BgzfFileTypeRecovery::close()
{
    if(bgzfReader) delete bgzfReader;
    bgzfReader = NULL;
    return true;
}


BgzfFileTypeRecovery::BgzfFileTypeRecovery(const char * filename, const char * mode)
{
    if(tolower(mode[0])=='r') {
        FILE *f = fopen(filename,"r");
        bgzfReader = new BGZFReader(f);
    } else {
        // die for now
        std::cerr << "Unable to open " << filename << " in mode " << mode << ".\n";
        close();
    }
}

//
// Why is this ever called?
//
bool BgzfFileTypeRecovery::operator == (const BgzfFileTypeRecovery &rhs)
{
    // if both are defined
    if(bgzfReader && this->bgzfReader) {
        // return true if file descriptor is the same
        return fileno(bgzfReader->stream()) == fileno(this->bgzfReader->stream());
    }

    // if both are not defined, they are technically equivalent, IMHO
    if(bgzfReader == NULL && rhs.bgzfReader == NULL) return true;

    return false;

}

int BgzfFileTypeRecovery::eof()
{
    return bgzfReader->eof();
}

unsigned int BgzfFileTypeRecovery::write(const void * buffer, unsigned int size)
{
    // currently unsupported
    return 0;
}

int BgzfFileTypeRecovery::read(void * buffer, unsigned int size)
{
    PeekaheadBuffer::ReturnCode rc = bgzfReader->read((uint8_t *) buffer, size);
   //     endOfFile = -1,
//        reSync = 0,
//        ok = 1
    switch(rc) {
        case PeekaheadBuffer::endOfFile:
            // set a flag?
            break;
        case PeekaheadBuffer::reSync:
            // throw exception
            break;
        case PeekaheadBuffer::ok:
            // find the peekahead iterator and copy the data from there
            // 
            break;
    }
    return 0;
}

int64_t BgzfFileTypeRecovery::tell()
{
    // currently unsupported
    return 0;
}

bool BgzfFileTypeRecovery::seek(int64_t offset, int origin)
{
    // currently unsupported
    return 0;
}

