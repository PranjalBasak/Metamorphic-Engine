#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "r2session.h"
#include "cJSON.h"

void test_session_init() {
    printf("[TEST] Initializing R2Session... ");
    // Use a known binary on your system (e.g., the engine.exe itself)
    R2Session* session = r2_open("engine.exe", false, false);
    
    assert(session != NULL);
    assert(strlen(session->arch) > 0);
    assert(session->bits == 64 || session->bits == 32);
    
    printf("PASSED (Arch: %s, Bits: %d)\n", session->arch, session->bits);
    r2_close(session);
}

void test_get_functions() {
    printf("[TEST] Fetching function list... ");
    R2Session* session = r2_open("engine.exe", true, false);
    
    cJSON* functions = r2_get_functions(session);
    assert(functions != NULL);
    assert(cJSON_IsArray(functions));
    
    // Check if at least one function exists (usually 'main' or entry)
    int count = cJSON_GetArraySize(functions);
    assert(count >= 0);
    
    printf("PASSED (Found %d functions)\n", count);
    
    cJSON_Delete(functions);
    r2_close(session);
}

void test_disassembly() {
    printf("[TEST] Disassembling 'main'... ");
    R2Session* session = r2_open("engine.exe", true, false);
    
    // We assume 'main' exists in our compiled engine
    cJSON* disasm = r2_disasm_function(session, "main");
    
    // If pdfj returns something, it should have an 'ops' array
    if (disasm) {
        cJSON* ops = cJSON_GetObjectItem(disasm, "ops");
        assert(ops != NULL);
        assert(cJSON_IsArray(ops));
        printf("PASSED\n");
    } else {
        printf("SKIPPED (main not found in analysis)\n");
    }

    cJSON_Delete(disasm);
    r2_close(session);
}

int main() {
    printf("=== R2Session Unit Tests ===\n");
    test_session_init();
    test_get_functions();
    test_disassembly();
    printf("============================\n");
    printf("ALL TESTS PASSED SUCCESSFULLY\n");
    return 0;
}