#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mutator.h"

// 1. Technique Definitions
// NOTE: These arrays must match the 'Tech' enum in mutator.h exactly.
const char* technique_names[] = {
    "instruction_substitution", "dead_code_insertion",
    "junk_insertion",           "instruction_reordering",
    "control_flow_obfuscation", "code_block_permutation",
    "register_renaming",        "stealth_packing",
    "data_obfuscation"  // Added
};

const char* technique_descriptions[] = {
    "Replace instructions with functionally equivalent alternatives",
    "Insert code that doesn't affect program execution",
    "Insert semantically neutral instructions",
    "Reorder independent instructions within basic blocks",
    "Add opaque predicates and control flow complexity",
    "Permute code blocks while preserving execution order",
    "Exchange registers throughout function scope",
    "Hide encrypted payload within PE resources (Icons/Data)",
    "Obfuscate constants by splitting 64-bit values or XOR-encoding 32-bit "
    "values"  // Added
};

// 2. Prototypes
int mutate_binary(const char* input, const char* output, bool do_analysis,
                  bool debug, bool force, bool* techniques, bool safe_mode,
                  bool advanced);

void list_techniques() {
  printf("Available metamorphic techniques:\n");
  for (int i = 0; i < TECH_COUNT; i++) {
    printf("  - %s: %s\n", technique_names[i], technique_descriptions[i]);
  }
}

// 3. Command Line Logic
int main(int argc, char* argv[]) {
  char* input_path = NULL;
  char* output_path = NULL;
  bool no_analysis = false;
  bool force = false;
  bool debug = false;
  bool safe_mode = false;
  bool enable_advanced = false;
  bool selected_techniques[TECH_COUNT] = {false};
  bool any_technique_selected = false;

  // Configuration for long options
  static struct option long_options[] = {
      {"input", required_argument, 0, 'i'},
      {"output", required_argument, 0, 'o'},
      {"no-analysis", no_argument, 0, 'n'},
      {"force", no_argument, 0, 'f'},
      {"debug", no_argument, 0, 'd'},
      {"techniques", required_argument, 0, 't'},
      {"list-techniques", no_argument, 0, 'l'},
      {"safe-mode", no_argument, 0, 's'},
      {"enable-advanced", no_argument, 0, 'e'},
      {"stealth", no_argument, 0, 'H'},  // Added stealth flag
      {0, 0, 0, 0}};

  int opt;
  // Added 'H' to the short options string
  while ((opt = getopt_long(argc, argv, "i:o:nft:lseH", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'i':
        input_path = optarg;
        break;
      case 'o':
        output_path = optarg;
        break;
      case 'n':
        no_analysis = true;
        break;
      case 'f':
        force = true;
        break;
      case 'd':
        debug = true;
        break;
      case 's':
        safe_mode = true;
        break;
      case 'e':
        enable_advanced = true;
        break;
      case 'H':  // Handle stealth packing flag
        selected_techniques[TECH_STEALTH_PACKING] = true;
        any_technique_selected = true;
        break;
      case 'l':
        list_techniques();
        return 0;
      case 't': {
        any_technique_selected = true;
        bool found = false;
        for (int i = 0; i < TECH_COUNT; i++) {
          if (strcmp(optarg, technique_names[i]) == 0) {
            selected_techniques[i] = true;
            found = true;
            break;
          }
        }
        if (!found) printf("[WARN] Unknown technique: %s\n", optarg);
        break;
      }
      default:
        return 1;
    }
  }

  if (!input_path || !output_path) {
    fprintf(stderr, "Error: --input and --output are required.\n");
    return 1;
  }

  // Call the engine with the updated technique array
  int changed =
      mutate_binary(input_path, output_path, !no_analysis, debug, force,
                    any_technique_selected ? selected_techniques : NULL,
                    safe_mode, enable_advanced);

  printf("[INFO] Done, number of instructions changed: %d\n", changed);
  return 0;
}