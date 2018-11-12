// PVision library for interaction with the Pixart sensor on a WiiMote
// This work was derived from Kako's excellent Japanese website
// http://www.kako.com/neta/2007-001/2007-001.html

// Steve Hobley 2009 - www.stephenhobley.com

#ifndef PVISION_H
#define PVISION_H

#include <stddef.h>

#define BUF_SIZE 16
#define NBLOBS 4

class PVision {
public:
    struct Blob {
        int x;
        int y;
        int dtx;
        int dty;
        size_t dist;
        bool visible;
    };

    void Init();
    size_t Read();

    const Blob& operator[](size_t n) const {
        return m_blobs[n];
    }

private:
    unsigned char m_buf[BUF_SIZE];
    Blob m_blobs[NBLOBS];

    void WriteBytes(unsigned char, unsigned char);
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif

