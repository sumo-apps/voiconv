#ifndef __COMPRESS__
#define __COMPRESS__

#include "core.h"

namespace TEN {

MemBuffer *lzma_compress( MemBuffer *from, bool deleteFrom=true );
MemBuffer *lzma_decompress( MemBuffer *from, bool deleteFrom=true );

}


#endif