# Setup and Execution Guide (WSL Ubuntu)

## 1. Prerequisites

The compiler front end is developed and tested for **Ubuntu on WSL (ARM64 / x86_64)**.

Ensure the necessary packages are installed:

```bash
sudo apt update
sudo apt install -y build-essential flex bison make git
```

Verify your toolchain:
```bash
gcc --version
flex --version
bison --version
make --version
```

---

## 2. Opening the Project in VS Code

From your Windows terminal or WSL terminal, navigate to the project directory:

```bash
cd /mnt/e/switchcasecompiler
code .
```

---

## 3. Building the Compiler

Compile the scanner, parser, and C source files using `make`:

```bash
make clean
make
```

This will:
1. Generate `build/parser.tab.c` and `build/parser.tab.h` using Bison.
2. Generate `build/lex.yy.c` using Flex.
3. Compile all C sources into `build/switch_compiler`.

---

## 4. Running the Compiler

### Run with default input (`input.txt`):
```bash
make run
```
Or run the binary directly:
```bash
./build/switch_compiler input.txt
```

### Run with standard input:
```bash
./build/switch_compiler
```
(Type code, then press `Ctrl+D` to submit).

---

## 5. Running the Automated Test Suite

Execute all valid and error tests:

```bash
make test
```
Or directly:
```bash
bash run_tests.sh
```

---

## 6. Troubleshooting

- **Permission Denied for test script**:
  Run:
  ```bash
  chmod +x run_tests.sh
  ```
- **Line Ending Issues (`\r\n` vs `\n`)**:
  If Windows line endings cause issues in bash scripts:
  ```bash
  sed -i 's/\r$//' run_tests.sh
  ```
