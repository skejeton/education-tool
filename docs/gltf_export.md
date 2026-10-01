# Exporting to glTF

The editor can export the world as a binary glTF file (`.glb`) to use in
Blender, Godot, Unity, three.js or any other tool that reads glTF 2.0.

## Usage

Click the **Export** button in the editor toolbar (the tray with an up arrow),
or press **Ctrl+E**. The world is written to `assets/world.glb` and a popup
says whether the export succeeded.

## What's in the file

- **Scenes.** The outdoor world is the default scene (scene 0). Every interior
  gets a scene of its own, named after its building, e.g.
  `building interior (8, -4)`.
- **Nodes.** Each scene has a `ground` node and one node per placed object,
  named after its type (`building`, `road`, `tree`, ...) or, for props, after
  the prop ID (`barrel`, `bench`, ...). The object node holds the object's
  position; its children are the boxes and models it's drawn with.
- **Extras.** Object nodes carry the editor properties in `extras`, so the
  game logic can be rebuilt in another engine:

  ```json
  {"type": "building", "x": 8, "y": -4, "floors": 3, "interior_scene": 1}
  {"type": "prop", "x": 2, "y": 5, "floors": 0, "id": "barrel"}
  ```

  `x` and `y` are the editor's grid coordinates. `interior_scene` is the index
  of the scene holding the building's interior. Blender shows extras as custom
  properties, and Godot imports them as node metadata.
- **Meshes and materials.** Geometry is shared: every box is one cube mesh
  instanced with a transform, and each source model is stored once. Textures
  are embedded as PNGs. Object colors become `baseColorFactor`, converted from
  sRGB to linear as glTF requires.

## Coordinate system

The engine's world is left-handed (looking down +Z, +X is to the right), while
glTF is right-handed. The exporter mirrors the Z axis and flips the triangle
winding, so the exported scene looks the same as in the editor, not mirrored.
A tile at editor position `(x, y)` ends up at glTF `(x, height, -y)`.

## Implementation

The exporter (`catedu/scene/gltf_export.cpp`) doesn't reimplement how objects
look. Rendering goes through `GenSink` (`catedu/genobj/render.hpp`): the
functions that describe objects emit `(model, matrix, color)` draws, and the
renderer draws them while the exporter records them as nodes. New object types
show up in the export without changes to the exporter.

Source geometry and textures are read from the model's glTF file, found with
`find_model_source` (`catedu/resources/resources.hpp`). No GPU data is read
back, which is why the unit tests in `tests/unit/test_gltf_export.inl` can
export without a graphics context.

## Limits

- **Fixed output path.** The file always goes to `assets/world.glb` and
  overwrites the previous export. There's no file dialog.
- **Web build.** In the WASM build the file is written to the browser's
  in-memory filesystem and can't be downloaded yet.
- **Interiors in other tools.** Many importers (Godot, Unity, most web viewers)
  load only the default scene, so interiors are left out there. Blender imports
  every scene.
- **Lighting and sky.** The editor's time-of-day lighting, fog and skybox
  aren't exported, only the geometry and materials. Exported scenes look
  flatter or brighter depending on the target's lighting.
- **Behaviour isn't exported.** Scripts, dialogue, NPC logic and the inventory
  stay in `world.dat`. Only the object properties in `extras` travel with the
  file.
- **Textures must be PNG.** Other image formats are skipped with a warning on
  stderr; the model is still exported, untextured.
- **One primitive per model.** Like the renderer, the exporter only reads the
  first primitive of the first mesh of each source model.

## Next steps

1. **Download in the web build.** Offer the `.glb` as a browser download
   instead of writing it to the in-memory filesystem.
2. **Choose the output path.** A save dialog on desktop, or at least a
   timestamped file name so exports don't overwrite each other.
3. **Godot and Unity import helpers.** A small import script per engine that
   reads `extras` and swaps object nodes for engine prefabs (spawn points for
   `player`, NPCs with their dialogue, doors that load `interior_scene`).
4. **Interiors for single-scene importers.** An option to place interiors in
   the default scene, offset away from the world or hidden, so Godot and Unity
   users get them too.
5. **Export the light.** Add a `KHR_lights_punctual` directional light matching
   the world's time of day.
6. **Merge static geometry.** An option to merge each object's boxes into one
   mesh per material, for engines that handle many small nodes poorly.
7. **Export scripts and dialogue.** Write the script graph to JSON next to the
   `.glb`, or into the root's extras, so behaviour can be rebuilt too.
