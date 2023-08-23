
Welcome to Educat. Low level editor for development of educational 3D interactive games and workflows.  

We support multiplayer out of the box. 

## Current Keymap
(add editor shortcuts here)

## Development Installation

Clone the repository
```
# if you don't care about contributing, you can use https version
# https://github.com/skejeton/education-tool.git
git clone --recurse-submodules git@github.com:skejeton/education-tool.git
```

if you clonned without the submodules, you can run
```
git submodule init
git submodule update
```

We also use [git-lfs](https://git-lfs.com/)
```
git lfs fetch --all
```

More system dependencies
- cmake
- python3
- some x11 libs on linux

## Build and Run
```
python build.py build
python build.py run
```
