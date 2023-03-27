// note: -D_7ZIP_ST is required when compiling on non-Windows platforms
// g++ -o lzma_sample -std=c++14 -D_7ZIP_ST lzma_sample.cpp LzmaDec.c LzmaEnc.c LzFind.c

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <memory>
#include <cstdlib>
#include "LzmaEnc.h"
#include "LzmaDec.h"

static void *_lzmaAlloc(ISzAllocPtr, size_t size)
{
    // return new uint8_t[size];
    return malloc(size);
}
static void _lzmaFree(ISzAllocPtr, void *addr)
{
    if (!addr)
        return;

    // delete[] reinterpret_cast<uint8_t *>(addr);
    free(addr);
}

static ISzAlloc _allocFuncs = {
    _lzmaAlloc, _lzmaFree};

const char *lzma_sumomagig = "SUMOLZMA";

#include "compress.h"

using namespace TEN;

MemBuffer *TEN::lzma_compress(MemBuffer *from, bool deleteFrom)
{
    printf("lzma_compress: original %ld bytes. %f.2 MB\n", from->size(), (float)from->size() / (1024 * 1024));
    //  set up properties
    SizeT inputSize = from->size();
    CLzmaEncProps props;
    LzmaEncProps_Init(&props);

    props.dictSize = inputSize;
    
    /*
		note, NOT heavily tested which attributes work the best in which case. These values are just good guesses
    if (props.dictSize > (1 << 20))
        props.dictSize = (1 << 20);
        */
    if (props.dictSize < (1 << 12))
        props.dictSize = (1 << 12);

    props.fb = 40;

    // prepare space for the encoded properties
    SizeT propsSize = 5;
    uint8_t propsEncoded[5];

    // allocate some space for the compression output
    // this is way more than necessary in most cases...
    // but better safe than sorry
    //   (a smarter implementation would use a growing buffer,
    //    but this requires a bunch of fuckery that is out of
    ///   scope for this simple example)
    SizeT outputSize64 = inputSize * 1.25;
    if (outputSize64 < 1024)
        outputSize64 = 1024;
    Byte *outputData = (Byte *)malloc(outputSize64);

    int lzmaStatus = LzmaEncode(
        outputData, &outputSize64, from->data, inputSize,
        &props, propsEncoded, &propsSize, 0,
        NULL,
        &_allocFuncs, &_allocFuncs);

    printf("LZMA compression finished: %d bytes (%.2f Mb)\n", (int)outputSize64, (float)outputSize64 / (1024 * 1024));
    // destroy the inputbuffer
    if (deleteFrom)
        delete from;

    if (lzmaStatus == SZ_OK)
    {
        size_t outputSize = outputSize64 + 13 + 8;
        // tricky: we have to generate the LZMA header
        // 5 bytes properties + 8 byte uncompressed size
        // result = std::make_unique<uint8_t[]>(outputSize64 + 13);
        Byte *result = (Byte *)malloc(outputSize);
        memcpy(result, lzma_sumomagig, 8);

        memcpy(result + 8, propsEncoded, 5);
        UInt64 saveSize = (UInt64)inputSize;
        // for (int i = 0; i < 8; i++)
        // result[5 + 8 + i] = ((saveSize >> (i * 8)) & 0xFF);
        memcpy(result + 5 + 8, &saveSize, 8);
        memcpy(result + 13 + 8, outputData, outputSize64);
        free(outputData);
        return new MemBuffer(result, outputSize, true);
    }
    else
    {
        free(outputData);
        printf("lzma_compress did not succeed.\n");
    }
    return 0;
}

MemBuffer *TEN::lzma_decompress(MemBuffer *from, bool deleteFrom)
{
    if (from->size() < 21)
        return NULL; // invalid header!
    // has magig
    bool hasMagig = true;
    for (int f = 0; f < 8; f++)
        if (((unsigned char *)from->data)[f] != lzma_sumomagig[f])
            hasMagig = false;
    if (!hasMagig)
        return 0;
    printf("HAS magig\n");

    Byte *input = (Byte *)from->data + 8;
    SizeT inputSize = (SizeT)from->size() - 8;

    if (inputSize < 13)
        return NULL; // invalid header!

    // extract the size from the header
    UInt64 size = 0;
    memcpy(&size, input + 5, 8);

    printf("Original size is: %zu (%.2f). Target size is %llu bytes (%.2f Mb)\n", inputSize, (float)inputSize/(1024*1024), size, (float)size / (1024 * 1024));
    if (size <= (256 * 1024 * 1024))
    {
        // auto blob = std::make_unique<uint8_t[]>(size);
        uint8_t *blob = (uint8_t *)malloc(size);

        ELzmaStatus lzmaStatus;
        SizeT procOutSize = size;
        SizeT procInSize = inputSize - 13;
        int status = LzmaDecode(blob, &procOutSize, &input[13], &procInSize, input, 5, LZMA_FINISH_END, &lzmaStatus, &_allocFuncs);
        if (deleteFrom)
            delete from;

        if (status == SZ_OK && procOutSize == size)
        {
            printf("LZMA decode succeeded\n");
            return new MemBuffer(blob, size, true);
        } else {
            printf("LZMA decode FAILED\n");
        }
    }
    if (deleteFrom)
        delete from;
    return NULL;
}
