# Secure Fragment Vault

Secure Fragment Vault is a C-based file storage project that splits a file into smaller fragments and stores those fragments inside a password-protected vault.

The project was developed to practice C programming, Linux system calls, file handling, directory operations, signals, and basic password protection.

## Features

- Create a password-protected vault
- Split files into 1024-byte fragments
- Store password information using a simple hash
- Unlock a vault using a password
- List available vaults
- Extract the original file from stored fragments
- Delete a vault after password verification
- Check fragment sizes using `lseek()`
- Handle `SIGINT` using signal handling
- Basic error handling
- Use Linux file and directory system calls
- Build the project using a Makefile

## Technologies Used

- C
- GCC
- Linux / WSL
- Linux system calls
- Make

## System Calls and Concepts Used

This project demonstrates several Linux system programming concepts:

- `open()` - Open files
- `read()` - Read file data
- `write()` - Write file data
- `close()` - Close file descriptors
- `lseek()` - Determine file/fragment size
- `mkdir()` - Create directories
- `opendir()` - Open directories
- `readdir()` - Read directory entries
- `stat()` - Obtain file information
- `unlink()` - Delete files
- `rmdir()` - Remove directories
- `signal()` - Handle signals

## Project Structure

    C_Project/
    |
    +-- src/
    |   +-- main.c
    |   +-- vault.c
    |   +-- auth.c
    |
    +-- include/
    |   +-- vault.h
    |   +-- auth.h
    |
    +-- .gitignore
    +-- Makefile
    +-- README.md
    +-- LICENSE

## How It Works

The project stores files by dividing them into smaller fragments.

For example, if a file is 1296 bytes:

    Original file
         |
         v
    Fragmentation
         |
         +-- fragment_001 -> 1024 bytes
         |
         +-- fragment_002 -> 272 bytes

The fragments are stored inside the selected vault.

When the file is extracted, the fragments are read in order and written back into a new output file.

## Password Protection

When creating a vault, the user provides a password.

The project does not directly store the password. Instead, it calculates a simple hash value using `simple_hash()` and stores that hash in:

    access.dat

When the user attempts to unlock or delete the vault, the entered password is hashed again and compared with the stored hash.

> Note: The hashing method used in this educational project is a simple demonstration and is not intended to provide production-level password security.

## Requirements

You need:

- Linux or WSL
- GCC
- Make

For Ubuntu/WSL, GCC and Make can be installed using:

    sudo apt update
    sudo apt install gcc make

## Compilation

Clone the repository and enter the project directory.

Build the project using:

    make

The executable will be created at:

    bin/vault

## Running the Program

Run:

    ./bin/vault

The program provides the following options:

    1. Create Vault
    2. List Vaults
    3. Unlock Vault
    4. Extract File
    5. Delete Vault
    6. Check Fragment Size
    7. Exit

## Example Usage

Suppose you have a file named:

    secret.txt

and choose to create a vault named:

    secret

The program creates a vault similar to:

    vaults/
    +-- secret/
        +-- access.dat
        +-- fragment_001
        +-- fragment_002

If the original file is 1296 bytes, the fragments may be:

    fragment_001 -> 1024 bytes
    fragment_002 -> 272 bytes

The original file can later be reconstructed using the extraction option.

## Checking Fragment Size

The project also demonstrates the use of `lseek()` to determine the size of a fragment.

For example:

    Fragment_001 size: 1024 bytes

This demonstrates how a file descriptor can be used to move to the end of a file and determine its size.

## Signal Handling

The project handles `SIGINT`, which is normally generated when the user presses:

    Ctrl + C

The signal handler provides controlled program behavior instead of allowing the program to terminate without handling the signal.

## Error Handling

The project includes basic error handling for operations such as:

- Opening files
- Creating files
- Reading files
- Writing files
- Creating directories
- Opening vaults
- Deleting vault contents
- Reading password information

Errors are reported to the user and the program attempts to close open file descriptors before returning from failed operations.

## Makefile Commands

Build the project:

    make

Remove the compiled executable:

    make clean

After running `make`, start the program using:

    ./bin/vault

## Git and Generated Files

Compiled executables and generated vault data are not stored in the Git repository.

The `.gitignore` file prevents files such as:

    bin/
    *.o
    *.obj
    *.out
    *.exe
    vaults/*

from being committed accidentally.

This keeps the GitHub repository focused on the source code and project documentation.

## Educational Purpose

This project was created as a C/Linux systems programming project to demonstrate practical use of:

- File descriptors
- File I/O
- System calls
- Directory operations
- File fragmentation
- File reconstruction
- Password hashing
- Signals
- `lseek()`
- Error handling
- Modular C programming
- Makefiles

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
