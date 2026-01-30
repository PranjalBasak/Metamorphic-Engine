#ifndef R2SESSION_H
#define R2SESSION_H

#include <stdbool.h>
#include <stdio.h>
#include "cJSON.h" // You need to add this file to your project

typedef struct {
    char arch[16];
    int bits;
    FILE* pipe;
    char filename[256];
} R2Session;



R2Session* r2_open(const char* filename, bool do_analysis, bool write);
cJSON* r2_get_functions(R2Session* session);
cJSON* r2_disasm_function(R2Session* session, const char* func_name);
int r2_write_patches(R2Session* session, cJSON* patches);
void r2_close(R2Session* session);

#endif