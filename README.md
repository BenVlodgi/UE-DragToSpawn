<br/>
<p align="center">
  <a href="https://github.com/BenVlodgi/UE-DragToSpawn">
    <img src="https://github.com/user-attachments/assets/d1dc7dde-090f-4bff-959e-367b4241160f" alt="Logo" width="640">
  </a>
</p>

# Drag to Spawn
Unreal Engine Editor Plugin.

Supports UE 4.27 - UE 5.7+


Drag Blueprint Objects or Data Assets into your level wrapped with whatever actor you want. Use a simple mapping, or an advanced blueprint handler to configure how to handle the dragged object.
| Configure | Drag |
|---|---|
| <a href=""><img src="https://github.com/user-attachments/assets/0678f0bc-32a3-497d-847d-55fd7668f3eb" alt="Configuration" height="300"></a> | <a href=""><img src="https://github.com/user-attachments/assets/128e21de-11ba-4558-92a6-c317c727353a" alt="Drag Demonstration" height="300"></a> |


## Use
There are two ways.

### Simple: Mapping
1. Open **Project Settings > Plugins > DragToSpawn**
2. Add an *Asset Mapping* from the asset type to the actor to spawn, with the variable name on the actor which the asset will be set on.

<img height="100" alt="Simple Mapping Setup" src="https://github.com/user-attachments/assets/dd48093b-053a-4784-83b1-746f29ac5461" />


### Advanced: Handler
1. Create a handler  Blueprint child of `UDragToSpawnHandler`.
2. Add the draggable type to `SupportedAssetClasses`.
3. Set `ActorClassToSpawn` to the actor class to be spawned on drag.
4. In the **Class Defaults**, click the big green `Register` Button.

<img height="180" alt="Advanced Mapping Setup" src="https://github.com/user-attachments/assets/4f6863af-d8aa-4127-9392-33bd0c841754" />


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
