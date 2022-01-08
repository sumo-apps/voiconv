/**
 * @file core.h
 * @author Tuomo Hirvonen (tuomo.hirvonen@gmail.com)
 * @brief The main structures (for accessing memory etc.) for the engine.
 * @version 0.1
 * @date 2021-12-30
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef __TENCORE__
#define __TENCORE__

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace TEN
{
    class Stream
    {
    public:
        Stream() {}
        virtual ~Stream() {}
        virtual size_t write(const void *from, size_t bytes) = 0;
        virtual size_t read(void *to, size_t bytes) = 0;
    };

    class Buffer : public Stream
    {
    public:
        Buffer() {}
        virtual ~Buffer() {}

        virtual size_t size() const = 0;
    };

    class MemBuffer : public Buffer
    {
    public:
        virtual size_t write(const void *from, size_t bytes);
        virtual size_t read(void *to, size_t bytes);
        virtual size_t size() const { return pos; }

        MemBuffer( const char *filePath );
        MemBuffer(void *data0, size_t bytes0, bool takeData=false)
        { // create as reference
            data = (unsigned char *)data0;
            pos = allocatedSize = (int)bytes0;
            ownData = takeData;
            readpos = 0;
        }
        MemBuffer()
        {
            data = 0;
            pos = 0;
            allocatedSize = 0;
            ownData = true;
        }
        virtual ~MemBuffer()
        {
            if (data != 0 && ownData)
                free(data);
        }
        unsigned char *data;

    protected:
        bool ownData;
        int pos;
        int readpos;
        int allocatedSize;
    };
};

#endif