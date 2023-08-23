
Welcome to Educat. Low level editor for development of educational 3D interactive games and workflows.  

We support multiplayer out of the box. 

## Current Keymap
(add editor shortcuts here)

## Clone the repository

Note: the repository is dependent on several submodules

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

More system dependencies
- cmake
- python3

Run app
```
python build.py build
```
