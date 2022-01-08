#include <stdio.h>
#include "core.h"

using namespace TEN;

MemBuffer::MemBuffer(const char *filePath)
{
    data = 0;
    pos = allocatedSize = readpos = 0;
    ownData = true;

    FILE *fhandle = fopen(filePath, "rb");
    if (!fhandle) return;
    fseek(fhandle, 0, SEEK_END);
    size_t siz = ftell(fhandle);
    fseek(fhandle, 0, SEEK_SET);

    data = (unsigned char*)malloc(siz);
    pos=allocatedSize=siz;
    if (fread(data,1,siz,fhandle)!=siz) {
        printf("Membuffer from file. Something went wrong\n");
    }
    fclose(fhandle);
    
}

size_t MemBuffer::write(const void *from, size_t bytes)
{
    if (pos + bytes >= allocatedSize)
    { // reallocate
        int newSize = allocatedSize * 2;
        if (newSize < 4096)
            allocatedSize = 4096;
        if (newSize < bytes + pos)
            newSize = pos + bytes + 4096;
        unsigned char *oldData = data;
        data = (unsigned char *)malloc(newSize);
        allocatedSize = newSize;
        // copy the previous content
        memcpy(data, oldData, pos);
        if (oldData)
            free(oldData);
    }
    memcpy(data + pos, from, bytes);
    pos += bytes;
    return bytes;
}

size_t MemBuffer::read(void *to, size_t bytes)
{
    int toread = bytes;
    if (toread + readpos >= pos)
    {
        toread = pos - readpos;
    }
    memcpy(to, (unsigned char *)data + readpos, toread);
    return (size_t)toread;
}
