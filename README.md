

# Educore

High level editor for development of educational 3D games. 

## Table of Contents
- [Educore](#educore)
  - [Table of Contents](#table-of-contents)
  - [Development Installation](#development-installation)
  - [Build and Run](#build-and-run)
  - [Pre-built binaries](#pre-built-binaries)
  - [Contributing](#contributing)
    - [Commit message convention](#commit-message-convention)
## Development Installation

```
git clone --recurse-submodules https://github.com/skejeton/ducation-tool
```

If you cloned without the submodules, you can run:
```
git submodule init
git submodule update
```

We also use [git-lfs](https://git-lfs.com/)
```
git lfs install
git lfs pull
```

More system dependencies
- cmake
- python3
- some x11 libs on linux
  - on ubuntu: libx11-dev libxi-dev libxcursor-dev libgl-dev

## Build and Run
```
python build.py build
python build.py run
```

## Pre-built binaries
You can find pre-built binaries for windows in the [releases](https://github.com/skejeton/education-tool/releases) page.

There's also weekly build for WASM: https://ske.land/ci/educore

## Contributing
To contribute, configure your editor:
- To use clang-format with the .clang-format file in the root of the project.
- To obey the .editorconfig file in the root of the project.

To create a pull request:
1. Fork the repository (if not already)
2. Create a new branch with the name of the PR you want to add. [*DO NOT WORK DIRECTLY ON MASTER*](https://stackoverflow.com/questions/60631439/why-is-it-bad-practice-to-submit-pull-request-from-master-branch)
3. Make your changes, commit and push them to the branch.
    - Make sure to follow the [commit message convention](#commit-message-convention).
4. Create a pull request to the main repository.
5. Rinse and repeat the review process until the PR is merged or closed.



### Commit message convention

`Keyword: Message` or `Keyword #issue: Message`.

| Keyword  | Description |
|----------|-------------|
|`Change`  | Change in existing feature |
|`Code`    | Code change/refactor (no functional change) |
|`Docs`    | Documentation change |
|`Feature` | New feature |
|`Fix`     | Bug fix |
|`Optimize`| Performance improvement |
|`Remove`  | Remove feature |
|`Test`    | Test change |


Examples:
- `Docs: Update README`
- `Feature #2401: Add second player`
- `Fix: Crash when loading level`
- `Change: Improve performance`
- `Optimize: Reduce memory usage`
- `Code: Remove deprecated function`
 