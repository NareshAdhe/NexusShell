# NexusShell

A minimal Unix shell written in C++ using core POSIX system calls (`fork`, `exec`, `pipe`, `dup2`, `wait`).

## Features

- **Command Execution** — runs any program available in `$PATH` via `execvp`
- **Piping** — supports multi-stage pipelines (`cmd1 | cmd2 | cmd3 | ...`)
- **Background Processes** — append `&` to run commands without blocking the shell
- **Built-in `cd`** — changes the working directory within the shell process
- **`exit`** — cleanly exits the shell

## Build & Run

```bash
g++ -o main main.cpp
./main
```

## How It Works

1. **Read** — reads a line of input from stdin
2. **Parse** — tokenizes the input and detects pipes (`|`) and background (`&`) operators
3. **Execute** — forks child processes, wires pipes with `dup2`, and calls `execvp`
4. **Wait** — parent waits for foreground processes or continues immediately for background ones

## Usage

```
NexusShell> ls -la
NexusShell> cat file.txt | grep error | wc -l
NexusShell> sleep 10 &
NexusShell> cd /tmp
NexusShell> exit
```

## Tech

- **Language:** C++
- **System Calls:** `fork`, `execvp`, `pipe`, `dup2`, `wait`, `chdir`
- **Platform:** Linux / macOS
