# FPS Lock

An Unreal Engine 5 editor plugin that adds a button to the Level Editor toolbar for capping the editor frame rate.

| Button | Meaning |
|---|---|
| Yellow, lock icon, **30** | Editor (and Play In Editor) capped at 30 fps |
| Green, lock icon, **60** | Capped at 60 fps |
| Unlock icon, **Off** | No limit |

Click the button to cycle 30 → 60 → Off. It sets the `t.MaxFPS` console variable. The choice is saved per user and per project (`EditorPerProjectUserSettings.ini`, section `[FpsLock]`) and restored when the editor starts.

The plugin is editor-only: it is never included in packaged games.

## Install

1. Download `FpsLock-<version>-Installer.zip` from [Releases](../../releases) and unzip it.
2. Run `Install.bat`.
3. Tick the engines to install into (every project using that engine gets the plugin), or pick a single `.uproject`. If an engine isn't listed (moved, copied or source-built), click **Add engine folder...** and choose its folder.
4. Click **Install**, then restart Unreal Editor.

If an engine is installed under `Program Files`, right-click `Install.bat` and choose **Run as administrator**.

From a console:

```bat
Install.bat -Engine 5.6
Install.bat -Engine "D:\Engines\UE_5.6"
Install.bat -Project "D:\MyGame\MyGame.uproject"
Install.bat -Engine 5.6 -Uninstall
```

Engine installs go to `<Engine>\Engine\Plugins\Marketplace\FpsLock`; project installs go to `<Project>\Plugins\FpsLock`.

## Compatibility

Supports UE 5.0 and later. The installer includes prebuilt binaries for the engine versions listed in each release. For any other 5.x version, install into a C++ project and the editor compiles the plugin when the project opens.

## Building from source

```powershell
.\Scripts\Build.ps1                  # every UE5 engine installed on this machine
.\Scripts\Build.ps1 -Engines 5.4,5.6 # only these
```

The script builds the plugin for each engine into `Build\UE<ver>\FpsLock` (logs in `Build\Logs`) and packages `Dist\FpsLock-<version>-Installer.zip`. It needs Visual Studio 2022 with the C++ workload. UE 5.0–5.3 also need the MSVC 14.36 toolchain ("MSVC v143 - VS 2022 C++ x64/x86 build tools (v14.36)" in the Visual Studio Installer), which the script selects automatically.

## Releasing

1. Bump `Version` and `VersionName` in `FpsLock/FpsLock.uplugin` and add a line to `CHANGELOG.md`.
2. Run `.\Scripts\Build.ps1`.
3. Tag and upload:

```powershell
git tag v1.0.0; git push --tags
gh release create v1.0.0 Dist\FpsLock-1.0.0-Installer.zip --notes-file CHANGELOG.md
```
