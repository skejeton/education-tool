# Educore

High level editor for development of educational 3D games.

## Table of Contents

-   [Educore](#educore)
    -   [Table of Contents](#table-of-contents)
    -   [Development Installation](#development-installation)
    -   [Build and Run](#build-and-run)
    -   [Pre-built binaries](#pre-built-binaries)
    -   [Exporting to glTF](#exporting-to-gltf)
    -   [Agent sandbox](#agent-sandbox)
    -   [Contributing](#contributing)
        -   [Commit message convention](#commit-message-convention)

## Development Installation

Install git and python3.8+.

```sh
$ git clone https://github.com/skejeton/education-tool
$ cd education-tool
$ python build.py up    # Pulls latest changes, submodules and initialize LFS
$ python build.py run
```

### System dependencies:

-   python3
-   some x11 libs on linux
    -   on ubuntu: libx11-dev libxi-dev libxcursor-dev libgl-dev

## Build and Run

```
python build.py build
python build.py run
```

## Pre-built binaries

You can find pre-built binaries for windows in the [releases](https://github.com/skejeton/education-tool/releases) page.

There's also weekly build for WASM: https://ske.land/ci/educore

## Exporting to glTF

The editor exports the world as a `.glb` file for Blender, Godot, Unity and other
tools: click **Export** in the toolbar or press **Ctrl+E**, and the world is
written to `assets/world.glb`. See [docs/gltf_export.md](docs/gltf_export.md)
for what the file contains, its limits and next steps.

## Agent sandbox

An experimental Python API runs the world headless for AI agents, with a demo
of Claude-driven residents spending a morning in a generated town:
`python agents/demo.py --offline` runs it without API calls. See
[agents/README.md](agents/README.md).

## Contributing

To contribute, configure your editor:

-   To use clang-format with the .clang-format file in the root of the project.
-   To obey the .editorconfig file in the root of the project.

To create a pull request:

1. Fork the repository (if not already)
2. Create a new branch with the name of the PR you want to add. [_DO NOT WORK DIRECTLY ON MASTER_](https://stackoverflow.com/questions/60631439/why-is-it-bad-practice-to-submit-pull-request-from-master-branch)
3. Make your changes, commit and push them to the branch.
4. Create a pull request to the main repository.
5. Rinse and repeat the review process until the PR is merged or closed.

### Commit message convention

It's not necessary to follow any particular convention, but please use any of the following:

```
message
scope: message
message (issue-id)
scope: message (issue-id)
```
