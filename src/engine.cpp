#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include our custom modules
#include "mutator.h"
#include "r2session.h"

#ifdef _WIN32
#include <windows.h>
#endif

// Helper: Standard file copy for stability
bool copy_file(const char* src, const char* dst) {
#ifdef _WIN32
  return CopyFileA(src, dst, FALSE);
#else
  FILE* fsrc = fopen(src, "rb");
  FILE* fdst = fopen(dst, "wb");
  if (!fsrc || !fdst) return false;
  char buf[8192];
  size_t n;
  while ((n = fread(buf, 1, sizeof(buf), fsrc)) > 0) fwrite(buf, 1, n, fdst);
  fclose(fsrc);
  fclose(fdst);
  return true;
#endif
}

int mutate_binary(const char* input_path, const char* output_path,
                  bool do_analysis, bool debug, bool force,
                  bool* user_techniques, bool safe_mode, bool enable_advanced) {
  // 1. Setup Technique Flags
  bool active_techniques[TECH_COUNT] = {false};
  if (user_techniques == NULL) {
    if (safe_mode) {
      active_techniques[TECH_INSTRUCTION_SUBSTITUTION] = true;
    } else {
      active_techniques[TECH_INSTRUCTION_SUBSTITUTION] = true;
      active_techniques[TECH_DEAD_CODE_INSERTION] = true;
      active_techniques[TECH_JUNK_INSERTION] = true;
      // Enable Data Obfuscation (constant splitting/XOR) by default
      active_techniques[TECH_DATA_OBFUSCATION] = true;

      if (enable_advanced) {
        active_techniques[TECH_REGISTER_RENAMING] = true;
        active_techniques[TECH_INSTRUCTION_REORDERING] = true;
        active_techniques[TECH_CONTROL_FLOW_OBFUSCATION] = true;
        active_techniques[TECH_CODE_BLOCK_PERMUTATION] = true;
      }
    }
  } else {
    for (int i = 0; i < TECH_COUNT; i++)
      active_techniques[i] = user_techniques[i];
  }

  // 2. Prepare Output File
  if (debug) printf("[INFO] Creating work copy: %s\n", output_path);
  if (!copy_file(input_path, output_path)) {
    fprintf(stderr, "[ERROR] Failed to copy binary.\n");
    return -1;
  }

  // 3. Initialize Radare2 Session
  R2Session* r2 = r2_open(output_path, do_analysis, true);
  if (!r2) {
    fprintf(stderr, "[ERROR] Radare2 failed to initialize.\n");
    return -1;
  }

  // 4. Initialize Mutator (Keystone)
  Mutator* mut = mut_init(r2->bits, debug, active_techniques);
  if (!mut) {
    r2_close(r2);
    return -1;
  }

  int total_instructions_changed = 0;

  // 5. Processing Loop: Function-by-Function
  cJSON* functions = r2_get_functions(r2);
  cJSON* fcn = NULL;

  if (debug) printf("[INFO] Starting mutation engine...\n");

  cJSON_ArrayForEach(fcn, functions) {
    cJSON* name_item = cJSON_GetObjectItem(fcn, "name");
    if (!name_item || !name_item->valuestring) continue;

    const char* fcn_name = name_item->valuestring;

    // Skip imported functions or stubs to avoid breaking the PLT/IAT
    if (strstr(fcn_name, "sym.imp") || strstr(fcn_name, "reloc.")) continue;

    cJSON* fcn_ctx = r2_disasm_function(r2, fcn_name);
    if (!fcn_ctx) continue;

    // Generate replacements based on active techniques (including Data
    // Obfuscation)
    cJSON* replacements = mut_generate_final(mut, fcn_ctx);

    // Apply the entire array of patches via the R2 session
    total_instructions_changed += r2_write_patches(r2, replacements);

    cJSON_Delete(fcn_ctx);
    cJSON_Delete(replacements);
  }

  // After the main processing loop, handle post-processing techniques
  if (active_techniques[TECH_STEALTH_PACKING]) {
    if (debug)
      printf("[INFO] Applying stealth packing to output resource...\n");

    // Integration for PE resource hiding would occur here
  }

  // 6. Cleanup
  if (debug) printf("[INFO] Cleaning up sessions...\n");
  cJSON_Delete(functions);
  mut_free(mut);
  r2_close(r2);

  if (debug)
    printf("[INFO] Total instructions changed: %d\n",
           total_instructions_changed);
  return total_instructions_changed;
}

// Bypass for MSVC-compiled static libraries in MinGW
#ifdef __GNUC__
void __GSHandlerCheck() {
}
void __security_check_cookie(uintptr_t _StackCookie) {
}
uintptr_t __security_cookie = 0xBB40E64E;
#endif