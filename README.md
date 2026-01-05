# Simple Shell

## Description
Simple Shell (hsh) is a minimal UNIX command interpreter written in C.
It behaves like the standard shell `/bin/sh` and supports both **interactive** and **non-interactive** modes.

The shell :
- Reads commands from the user or from a file/pipe.
- Searches for the executable in the PATH.
- Creates a child process for execution using `fork`.
- Executes the command with `execve`.
- Waits for the process to finish with `wait`.
- Handles errors and built-in commands like `exit`.
- Ignores empty lines or lines containing only spaces or tabs.
- Resolves commands using the PATH environment variable if the command is not an absolute path.
- Supports built-in commands `exit` and `env`.
- Returns appropriate exit status (0 on success, 127 if command not found, or child process status).

---

## Compilation
To compile the shell on **Ubuntu 20.04**:

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

---

## ✅ Implemented Features

### :small_blue_diamond: Simple Shell 0.0
- Displays a prompt `($)`
- Reads a command line from standard input
- Executes simple commands without arguments

### :small_blue_diamond: Simple Shell 0.1 - Handle command lines with arguments
- Supports commands with arguments
- Example :
```bash
ls -l /tmp
```

### :small_blue_diamond: Simple Shell 0.2 - Handle the PATH
- Searches for commands in directories listed in the PATH variable
- `fork()` is not called if the command does not exist
- Correct handling of an empty PATH
UNIX-compliant error message:
```bash
/hsh: 1: ls: not found
```
Exit status :
- `127` → command not found

### :small_blue_diamond: Simple Shell 0.3 - Built-in exit
- Implements the built-in command
`exit`
- Exits the shell using the last command's exit status
- No argument handling required

### :small_blue_diamond: Simple Shell 0.4 - Built-in env
- Prints the current environment variables:
`env`




## Usage
Interactive mode:
- Run `./hsh` to start the shell.
- The prompt `($)` is displayed when waiting for user input.

## Non-interactive mode:
- Run commands from a file or pipe: `echo "/bin/ls" | ./hsh`.

## Built-in commands:
- `exit` to terminate the shell.
- `env` to print the current environment variables.

---

## Technical Details
- The shell reads user input using `getline`.
- Commands are tokenized using spaces and tabs.
- Forks a child process to execute external commands using `execve`.
- Waits for child process completion using `wait` or `waitpid`.
- Resolves commands via PATH if not provided as absolute path.

---

### 🚫 Constraints Respected
### ❌ No forbidden functions used
### ❌ No system()
### ❌ No getenv()
### ❌ No dprintf()
### ❌ No printf() for error messages
### ✅ Only allowed system calls and functions
### ✅ Compatible with GNU89
### ✅ Betty style compliant
### ✅ No memory leaks (checked with Valgrind)

---

## AUTHORS
**Maxime RÉGNIER and James ROUSSEL**
