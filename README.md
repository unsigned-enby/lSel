## Description
Simple list selection menu for use in scripts. The menu is displayed on 
stderr so as to allow for output in a pipeline or to a file. Arguments (i.e list items)
may be read from stdin, supplied on the command-line, or obtained from the items in a
directory (when using `--list [dir]`).

## Compiling
Simply run
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```
from the project's root directory.
Optionally, you may use: 
   - `-DSYSTEM_FTXUI=1` to use prebuilt/installed ftxui libraries.
   - `-DLSEL_STATIC=1` to create a static binary. However, please note that 
     the TUI library FTXUI will be statically compiled either way.

## Install
Once compiled, just place the `lSel` binary anywhere in your PATH.

## Usage
The full list of options may be seen with `lSel --help`. Along with the options
described there, lSel supports reading args from both the command-line and stdin
(however/at this time, it can not do both at once). Please note that if you use
the `--file` option, that the resulting selections will be sent to both the given
file and to stdout. So, if you want to simply output to a file,
use your shell's redirection operators (e.g `>/>>` for bash).
- Selections are made by either pressing `Enter` or `Space`.
- Once you have made your selection, you may use either `F1`, `Control-c`, or `y` to continue.

## Notes
I believe this should work on both windows and Linux, but I have, as of yet, only tested it
on Linux. PLEASE NOTE I DO NOT GUARANTEE THIS WILL OPERATE CORRECTLY ON YOUR SYSTEM. 
So before sending output to a file you do not want accidentally overridden, please test
it out to make sure it works as you expect. Should you notice any issues, you can
(and are encouraged to) let me know and I'll fix it as soon as I am able.
