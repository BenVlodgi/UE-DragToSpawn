# Drag to Spawn
Unreal Engine Editor Plugin.

Supports UE 4.27 - UE 5.7+


Drag Blueprint Objects or Data Assets into your level wrapped with whatever actor you want.

Configure and register a handler class which will setup the actor with your dragged data.

## Use
There are two ways.

### Simple: Mapping
1. Open **Project Settings > Plugins > DragToSpawn**
2. Add an *Asset Mapping* from the asset type to the actor to spawn, with the variable name on the actor which the asset will be set on.

### Advanced: Handler
1. Create a handler  Blueprint child of `UDragToSpawnHandler`.
2. Add the draggable type to `SupportedAssetClasses`.
3. Set `ActorClassToSpawn` to the actor class to be spawned on drag.
4. In the **Class Defaults**, click the big green `Register` Button.

#### No-Actor Mode
If `ActorClassToSpawn` is left null, no actor is spawned. `DragStart` and `DropFinished` still fire, so you can use a handler to just *do* something.

#### Handler Events
Handlers receive events during the spawn sequence. Override any of these in Blueprint or C++:

| Event | Description |
|---|---|
| `CanHandleAsset` | Before anything happens. Return false to abort. Do extra validation here. Don't want to spawn characters with Name == "Bob" here's your chance. |
| `DragStart` | Before the actor is spawned. Receives drop location and rotation.  |
| `ActorSpawnedPreConstruction` | After `SpawnActor()`, before Construction Script. Set variables the Construction Script needs here. |
| `ActorSpawnedPostConstruction` | After `FinishSpawning()`. Construction Script has run. |
| `DropFinished` | Always runs at the end even if no actor was spawned. |

For Blueprint assets, the handler receives the blueprint class's **CDO** as the Asset parameter.
