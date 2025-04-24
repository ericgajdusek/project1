# Process and Resource Management Simulator

This C++ program simulates basic process and resource management commands in an operating system's environment. It reads commands from input and prints the result of each action.

## Files

- `main.cpp` — Contains the main logic of the program and input handling.
- `main.h` — Contains data structures and function declarations used in the program.

## Compiling and Running

### 1. Compile

 - Open a terminal and navigate to the directory containing `main.cpp` and `main.h`, then run:

```bash
g++ main.cpp -o main
```
### 2. Run

You can either enter commands manually or provide them from a file:

```bash
./main
```
or

```bash
./main < input.txt
```
> Replace input.txt with the path to your command file.

### Supported Commands

| Command       | Description                                      |
|---------------|--------------------------------------------------|
| `in`          | Initialize the system.                           |
| `cr <p>`      | Create a process with priority `p` (0–2).        |
| `de <pid>`    | Destroy process with ID `pid` and its children.  |
| `rq <r> <n>`  | Request `n` units of resource `r`.               |
| `rl <r> <n>`  | Release `n` units of resource `r`.               |
| `to`          | Timeout: move current process to end of queue.   |

📋 Example Input
```bash
in
cr 1
rq 1 1
to
rl 1 1
de 1
```
> This example initializes the system, creates a process with priority 1, requests and releases a resource, and finally destroys the process.


