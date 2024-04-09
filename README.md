## Introduction
App to manage books in library. Done in C with gtk library.
To compile, use command below.
```
gcc `pkg-config --cflags gtk+-3.0` -o a main.c `pkg-config --libs gtk+-3.0`
```
## Features
- Viewing, searching, adding and removing books.
- Good looking UI.
- Writing database to file.
- Reading database from file.

## Notes
Requires gtk library. \
Works only on Linux.
