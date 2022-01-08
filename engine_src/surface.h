/*
 * Surface 2d image with N channels
 *
 */

#ifndef __SURFACE__
#define __SURFACE__

#include "core.h"

namespace TEN
{
    class Surface
    {
    public:
        Surface( MemBuffer *encoded );
        Surface(int w = 0, int h = 0, int channels = 4);
        Surface(int w, int h, int channels, void *data, int pitch=-1);            // Create as reference to existing decoded data
        virtual ~Surface();

        bool loadFromMemory(const void *data, const size_t size, int request_channels = 0);

        void create(int w0, int h0, int channels0);
        void release();
        void changeNofChannels(int newChannels);


            // Works with qoi or anything supported by stb_image
        static Surface *load(const char *filePath, int request_channels = 0);
        static Surface *load(const void *data, const size_t size, int request_channels = 0);
        static Surface *load(const MemBuffer *b, int request_channels = 0);

        TEN::MemBuffer *savePngToMemory();
        TEN::MemBuffer *saveJpgToMemory(int q=90);
        TEN::MemBuffer *saveTgaToMemory();
        TEN::MemBuffer *saveQoiToMemory();
        TEN::MemBuffer *saveQoiLZMAToMemory();

        void *pixels;
        int width;
        int height;
        int pitch;                      // PITCH in pixels. not in bytes!
        unsigned char channels;


    protected:
        bool ownData;
    };
}

#endif