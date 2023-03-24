#include <stdio.h>
#include <cstring>
#include "surface.h"

#define STR_ENDS_WITH(S, E) (strcmp(S + strlen(S) - (sizeof(E) - 1), E) == 0)

int main( int argc, char **arg) {
    printf("*** Voi/Qoi converter. (c) Sumo Ltd. 2022\n");
    if (argc<3) {
        printf("Usage: sourceimage targetimage.[qoi,voi,png]\n");
        return 0;
    }
    char *sourceFile = arg[1];
    char *targetFile = arg[2];

    printf("Converting from [%s] to [%s]\n", sourceFile, targetFile );


    TEN::Surface *s = TEN::Surface::load( sourceFile );

    TEN::MemBuffer *tbuf = 0;
    if (STR_ENDS_WITH(targetFile,".qoi")) {
        tbuf = s->saveQoiToMemory();
    } else if (STR_ENDS_WITH(targetFile,".voi") || STR_ENDS_WITH(targetFile,".sumo")) {
        tbuf = s->saveQoiLZMAToMemory();
    } else if (STR_ENDS_WITH(targetFile,".png")) {
        tbuf = s->savePngToMemory();
    }
    delete s;

    FILE *tfile = fopen(targetFile, "wb");
    fwrite(tbuf->data, tbuf->size(), 1, tfile);
    fclose(tfile);
    delete tbuf;
    printf("Done.\n");
    return 0;
}
