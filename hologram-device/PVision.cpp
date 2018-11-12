// Example of using the PVision library for interaction with the Pixart sensor on a WiiMote
// This work was derived from Kako's excellent Japanese website
// http://www.kako.com/neta/2007-001/2007-001.html

// Steve Hobley 2009 - www.stephenhobley.com

#include "PVision.h"

#include <Arduino.h>
#include <Wire.h>

#define SENSOR_ADDR 0xB0
#define SLAVE_ADDR (SENSOR_ADDR >> 1)
#define MAX_BLOB_DIST 15

void PVision::Init() {
    Wire.begin();
    WriteBytes(0x30, 0x01);
    delay(10);
    WriteBytes(0x30, 0x08);
    delay(10);
    WriteBytes(0x06, 0x90);
    delay(10);
    WriteBytes(0x08, 0xC0);
    delay(10);
    WriteBytes(0x1A, 0x40);
    delay(10);
    WriteBytes(0x33, 0x33);
    delay(10);
    delay(100);
}

size_t PVision::Read() {
    size_t blobCount = 0;

    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(0x36);
    Wire.endTransmission();

    Wire.requestFrom(SLAVE_ADDR, BUF_SIZE);

    size_t i;
    for (i = 0; i < BUF_SIZE; i++) {
        m_buf[i] = 0;
    }
    i = 0;
    while (Wire.available() && i < BUF_SIZE) {
        m_buf[i++] = Wire.read();
    }

    unsigned char s;
    int prevX, prevY;
    for (i = 0; i < NBLOBS; i++) {
        s = m_buf[i * 3 + 3];
        m_blobs[i].dist = s & 0x0F;

        if (m_blobs[i].dist < MAX_BLOB_DIST) {
            m_blobs[i].visible = true;
            blobCount++;

            prevX = m_blobs[i].x;
            prevY = m_blobs[i].y;
            m_blobs[i].x = m_buf[i * 3 + 1] + ((s & 0x30) << 4);
            m_blobs[i].y = m_buf[i * 3 + 2] + ((s & 0xC0) << 2);
            m_blobs[i].dtx = m_blobs[i].x - prevX;
            m_blobs[i].dty = m_blobs[i].y - prevY;
        } else {
            m_blobs[i].visible = false;
        }
    }

    return blobCount;
}

void PVision::WriteBytes(unsigned char a, unsigned char b) {
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(a);
    Wire.write(b);
    Wire.endTransmission();
}

