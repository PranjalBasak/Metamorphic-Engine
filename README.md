# Metamorphic Engine

## ⚠️ CRITICAL WARNING ⚠️

**THIS PROJECT MUST ONLY BE RUN ON AN ISOLATED VIRTUAL MACHINE WITH NO INTERNET ACCESS.**

**DO NOT download or execute this software on your regular system. Running this engine on a production machine or any system with network connectivity may cause severe damage to your system, compromise system integrity, and potentially corrupt executables. This tool modifies binary code at runtime and is intended STRICTLY for security research and educational purposes in controlled environments.**

**YOU HAVE BEEN WARNED. The authors are not responsible for any damage caused by misuse of this software.**

---

## Overview

The Metamorphic Engine is an advanced binary mutation framework that applies various code obfuscation and metamorphic techniques to Windows PE executables. It uses Radare2 for binary analysis and the Keystone assembler engine to generate functionally equivalent but structurally different code variants.

This tool demonstrates how malware can evade signature-based detection systems by continuously morphing its code while maintaining the same functionality.

## Features

The engine implements the following metamorphic techniques:

- **Instruction Substitution**: Replace instructions with functionally equivalent alternatives
- **Dead Code Insertion**: Insert code that doesn't affect program execution
- **Junk Insertion**: Insert semantically neutral instructions
- **Instruction Reordering**: Reorder independent instructions within basic blocks
- **Control Flow Obfuscation**: Add opaque predicates and control flow complexity
- **Code Block Permutation**: Permute code blocks while preserving execution order
- **Register Renaming**: Exchange registers throughout function scope
- **Stealth Packing**: Hide encrypted payload within PE resources (Icons/Data)
- **Data Obfuscation**: Obfuscate constants by splitting 64-bit values or XOR-encoding 32-bit values

## Architecture

```
├── src/
│   ├── main.cpp          # Command-line interface
│   ├── engine.cpp        # Main mutation engine
│   ├── mutator.cpp       # Mutation techniques implementation
│   ├── r2session.cpp     # Radare2 integration
│   ├── cJSON.cpp         # JSON parsing library
│   └── getopt.cpp        # Command-line argument parsing
├── include/              # Header files
├── lib/                  # Static libraries
├── bin/                  # Output binaries
├── output/               # Mutated binaries
├── test/                 # Test binaries
├── build.bat             # Build script
├── cpp_build.bat         # C++ build script
└── modern_run.ps1        # PowerShell execution script
```

## Requirements

- **Windows Operating System** (tested on Windows 10/11)
- **Radare2** - Binary analysis framework
- **Keystone Engine** - Assembler framework
- **MinGW-w64** or **MSVC** - C/C++ compiler
- **Administrator privileges** - Required for binary modification

## Building

### Method 1: Standard Build
```batch
build.bat
```

### Method 2: C++ Build
```batch
cpp_build.bat
```

The compiled binary will be output to the `bin/` directory.

## Usage

### Basic Usage
```bash
metamorphic-engine.exe --input <input.exe> --output <output.exe>
```

### Safe Mode (Conservative Mutations)
```bash
metamorphic-engine.exe --input <input.exe> --output <output.exe> --safe-mode
```

### Advanced Mode (All Techniques)
```bash
metamorphic-engine.exe --input <input.exe> --output <output.exe> --enable-advanced
```

### Specific Techniques
```bash
metamorphic-engine.exe --input <input.exe> --output <output.exe> -t instruction_substitution -t dead_code_insertion
```

### List Available Techniques
```bash
metamorphic-engine.exe --list-techniques
```

## Command-Line Options

| Option | Description |
|--------|-------------|
| `-i, --input` | Input PE executable (required) |
| `-o, --output` | Output mutated executable (required) |
| `-n, --no-analysis` | Skip binary analysis phase |
| `-f, --force` | Force mutation even if warnings |
| `-d, --debug` | Enable debug output |
| `-s, --safe-mode` | Use only safe mutation techniques |
| `-e, --enable-advanced` | Enable advanced obfuscation techniques |
| `-t, --techniques` | Specify specific techniques to apply |
| `-H, --stealth` | Enable stealth packing |
| `-l, --list-techniques` | List all available techniques |

## Running Scripts

### PowerShell (Modern)
```powershell
.\modern_run.ps1
```

### Batch (Legacy)
```batch
legacy_run.bat
```

### Run All Tests
```powershell
.\all_modern_run.ps1
```

## Safety Considerations

1. **Isolated Environment**: Always run in a VM with snapshots
2. **No Network Access**: Disconnect all network adapters
3. **Backup Everything**: Keep original binaries safe
4. **Test Thoroughly**: Verify mutated binaries work correctly
5. **Legal Use Only**: Only use on binaries you own or have permission to modify

## Educational Purpose

This project is designed for:
- Security researchers studying malware evasion techniques
- Antivirus developers testing detection capabilities
- Academic research on code obfuscation
- Understanding metamorphic code generation

## Legal Disclaimer

This tool is provided for educational and research purposes only. Using this software to create malware, evade antivirus detection for malicious purposes, or modify software you don't own is illegal and unethical.

The authors and contributors:
- Do NOT condone malicious use of this software
- Are NOT responsible for any damage or illegal activity
- Provide this tool AS-IS with no warranties
- Assume you have proper authorization for any binaries you modify

## Technical Details

### Mutation Process

1. **Binary Analysis**: Radare2 analyzes the input PE file
2. **Function Enumeration**: All functions are identified (excluding imports)
3. **Disassembly**: Each function is disassembled to assembly instructions
4. **Mutation**: Selected techniques are applied to instructions
5. **Reassembly**: Keystone reassembles modified instructions
6. **Patching**: Modified bytes are written back to the binary
7. **Verification**: Output binary is created with mutations applied

### Dependencies

The engine requires:
- Radare2 for PE parsing and disassembly
- Keystone for x86/x64 assembly
- cJSON for configuration parsing

## Contributing

This is a research project. If you have improvements or new techniques to add, ensure they are for legitimate research purposes only.

## License

MIT License

## Acknowledgments

- Radare2 project for binary analysis capabilities
- Keystone Engine for assembler support
- Security research community

---

**Remember: With great power comes great responsibility. Use this tool ethically and legally.**