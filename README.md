# OpenAt

## Description

OpenAt is a command-line utility for Linux that allows you to open files in default applications or specified text editors. The program works with .desktop files and can handle MIME types of files.

## Compilation

To compile the program, run the following command:

```bash
gcc -o openat main.c
```

## Usage

To use the program, execute one of the following commands in the terminal:

```bash
./openat <path-to-file>
```

This command will open the file <path-to-file> in the default application for its MIME type.

You can also use the editor mode to open the file in a text editor:

```bash
./openat -e --nano|--vim|--pico|--vi <path-to-file>
```

If the editor key is not specified, the available editor will be used in the following order of preference: nano, vim, pico, vi.

To open a file in the default text/plain editor, use the -t argument
```bash
./openat -t <path-to-file>
```

## Features

- The program automatically determines the MIME type of the file.
- Allows you to specify the desired text editor from the command line.
- Supports placeholders %F and %u in .desktop files.

## Dependencies

For the correct operation of the program, the following packages must be installed in your system:

- file — to determine the MIME type of a file.
- Text editors (nano, vim, pico, vi) — to open files in edit mode.

## Licensing

The code is distributed under the MIT license

## Contact

For feedback, use GitHub Issues or send an email to the developer's email address: [eugene@anytalks.space](mailto:eugene@anytalks.space).
