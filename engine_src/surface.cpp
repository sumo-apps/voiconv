#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define QOI_IMPLEMENTATION
#include "qoi.h"

#include "compress.h"
#include "surface.h"

using namespace TEN;

#define STR_ENDS_WITH(S, E) (strcmp(S + strlen(S) - (sizeof(E) - 1), E) == 0)

Surface::Surface(int w0, int h0, int channels0) : ownData(true), pixels(0), width(0), height(0), channels(0)
{
    create(w0, h0, channels0);
}

Surface::Surface(int w, int h, int channels, void *data, int pitch0) : ownData(false), pixels(data), width(w), height(h), channels(0)
{
    pitch = (pitch0 > 0) ? pitch : w;
}

Surface::Surface(MemBuffer *encoded) : ownData(true), pixels(0), width(0), height(0), channels(0)
{
    loadFromMemory(encoded->data, encoded->size());
}

Surface::~Surface()
{
    release();
}

void Surface::create(int w0, int h0, int channels0)
{
    if (w0 * h0 * channels0 < 1)
        return;
    release();
    width = pitch = w0;
    height = h0;
    channels = channels0;
    ownData = true;
    pixels = malloc(width * height * channels);
}

void Surface::release()
{
    if (ownData)
    {
        if (pixels)
            free(pixels);
    }
    pixels = 0;
    width = height = pitch = channels = 0;
}

void Surface::changeNofChannels(int newChannels)
{
    if (pixels == 0 || width * height < 1)
        return; // no data
    if (newChannels == channels)
        return; // nothing to do


    unsigned char *newData = (unsigned char *)malloc(width * height * newChannels);
    for (int y = 0; y < height; y++)
    {
        unsigned char *olddata = (unsigned char *)pixels + y * pitch * channels;
        unsigned char *newdata = newData + y * pitch * newChannels;
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < newChannels; c++)
                newdata[c] = olddata[c];
            olddata += channels;
            newdata += newChannels;
        }
    }
    // release old data
    if (ownData)
    {
        if (pixels)
            free(pixels);
    }
    pixels = newData;
    ownData = true;
    channels = newChannels;
}

bool Surface::loadFromMemory(const void *data, const size_t size, int request_channels)
{
    if (size < sizeof(qoi_desc))
    {
        return 0;
    }
    if (size < 20)
    {
        return 0;
    }

    TEN::MemBuffer buf((void *)data, size, false);
    TEN::MemBuffer *decomp = lzma_decompress(&buf, false);
    if (decomp)
    { // is lzma buffer. now decoded
        if (loadFromMemory(decomp->data, decomp->size(), request_channels))
        {
            printf("Succeeded\n");
        }
        else
            printf("failed\n");
        delete decomp;
        return true;
    }

    release();
    qoi_desc desc;
    pixels = qoi_decode(data, size, &desc, request_channels);
    if (pixels)
    {
        if (request_channels != 0)
            channels = request_channels;
        else
            channels = desc.channels;
        width = pitch = desc.width;
        height = desc.height;
        ownData = true;
        printf("Surface: loadmem qoi: ");
    }
    else // wasnt qoi. load with stb_image
    {
        int channelsInFile;
        pixels = stbi_load_from_memory((const unsigned char *)data, size, &width, &height, &channelsInFile, request_channels);
        if (request_channels != 0)
            channels = request_channels;
        else
            channels = channelsInFile;
        printf("Surface: loadmem stbi: ");
    }

    printf("%d,%d (channels %d)\n", width, height, channels);
    return true;
}

Surface *Surface::load(const char *filePath, int request_channels)
{
    TEN::MemBuffer *b = new TEN::MemBuffer(filePath);
    if (!b) return 0;
    Surface *rval = Surface::load( b->data, b->size(), request_channels );
    delete b;
    return rval;
}

Surface *Surface::load(const void *data, const size_t size, int request_channels)
{
    Surface *rval = new Surface;
    if (rval->loadFromMemory(data, size, request_channels) == false)
    {

    }
    return rval;
}

Surface *Surface::load(const MemBuffer *b, int request_channels)
{
    return load(b->data, b->size(), request_channels);
}

static void stbi_write_to_membuffer(void *context, void *data, int size)
{
    TEN::MemBuffer *db = (TEN::MemBuffer *)context;
    db->write((unsigned char *)data, size);
}

TEN::MemBuffer *Surface::savePngToMemory()
{
    TEN::MemBuffer *rval = new TEN::MemBuffer;
    int encoderval = stbi_write_png_to_func(stbi_write_to_membuffer, rval, width, height, channels, pixels, width * channels);
    return rval;
}

TEN::MemBuffer *Surface::saveJpgToMemory(int q)
{
    TEN::MemBuffer *rval = new TEN::MemBuffer;
    int encoderval = stbi_write_jpg_to_func(stbi_write_to_membuffer, rval, width, height, channels, pixels, q);
    return rval;
}

TEN::MemBuffer *Surface::saveTgaToMemory()
{
    TEN::MemBuffer *rval = new TEN::MemBuffer;
    int encoderval = stbi_write_tga_to_func(stbi_write_to_membuffer, rval, width, height, channels, pixels);
    return rval;
}

TEN::MemBuffer *Surface::saveQoiToMemory()
{
    int len;
    qoi_desc desc = (qoi_desc){
        .width = (unsigned int)width,
        .height = (unsigned int)height,
        .channels = channels,
        .colorspace = QOI_SRGB};
    void *encoded = qoi_encode(pixels, &desc, &len);
    return new TEN::MemBuffer(encoded, (size_t)len, true);
}

TEN::MemBuffer *Surface::saveQoiLZMAToMemory()
{
    return TEN::lzma_compress(saveQoiToMemory());
}