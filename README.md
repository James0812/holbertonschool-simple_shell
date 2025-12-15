# Simple Shell

## Description
Simple Shell (hsh) is a minimal UNIX command interpreter written in C.
It behaves like the standard shell `/bin/sh` and supports both **interactive** and **non-interactive** modes.

The shell:
- Reads commands from the user or from a file/pipe.
- Searches for the executable in the PATH.
- Creates a child process for execution using `fork`.
- Executes the command with `execve`.
- Waits for the process to finish with `wait`.
- Handles errors and built-in commands like `exit`.

---

## Compilation
To compile the shell on **Ubuntu 20.04**:

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

