# Plan: Build Aya Server + Client (Linux, Ninja)

## Goal
Build the Aya server (`client/server`) and player (`client/player`) on Linux using Ninja, then run a local server and connect a client.

## Context
- CMake project `Aya`, C++20, outputs to `build/<preset>/bin`.
- Dependencies via vcpkg (`vcpkg.json`); Qt6 required by presets; CEF optional (web-helper).
- CEF submodule now populated (`third-party/cef`).
- Linux presets: `x64-linux-dbg` / `x64-linux-rel` (clang + Ninja + vcpkg toolchain).

## Decisions
- Use `x64-linux-rel` (Release) for speed/size; switch to dbg if debugging needed.
- Enable CEF (`-DENABLE_CHROMIUM_FRAMES=ON`, default). cef-cmake will download CEF binaries for version `140.1.14+...` automatically on configure (requires network). Builds `web-helper` target too.
- Keep `NO_QT=OFF` (Qt needed by UI); install Qt6 via system packages or aqt.
- Do not build studio/thumbnail-handler unless needed (targets optional).

## Ordered Steps

1. Verify/populate remaining submodules:
   - `third-party/BulletPhysics`, `RakNet`, `fmod`, `QtitanRibbon` (check non-empty; fetch if empty).
2. Install vcpkg:
   - `git clone https://github.com/microsoft/vcpkg` into repo root `vcpkg/`.
   - `./vcpkg/bootstrap-vcpkg.sh`
   - `./vcpkg/vcpkg install` (uses `vcpkg.json`; long).
3. Install Qt6 (Linux): `sudo apt install qt6-base-dev qt6-tools-dev` or aqt install-qt; set `CMAKE_PREFIX_PATH`/`Qt6_DIR` to actual path (not macOS path).
4. Fix preset paths: edit `CMakePresets.json` Linux entries `CMAKE_PREFIX_PATH` and `Qt6_DIR` to local Qt6 (or pass via `-D`).
5. Configure:
   ```
   cmake --preset x64-linux-rel
   ```
6. Build server + player with Ninja:
   ```
   cmake --build build/x64-linux-rel --target Server Player -j$(nproc)
   ```
   (target names TBD; inspect `client/server/CMakeLists.txt` and `client/player/CMakeLists.txt`.)
7. Run server:
   ```
   ./build/x64-linux-rel/bin/Aya.Server --localServer --insecure --port 64989 --localServerPort 53640 --localServerPlace rbxasset://test.rbxl
   ```
   (confirm CLI flags from server main.cpp).
8. Run client and connect to `127.0.0.1:53640` (or configured port).
9. Open firewall/port if hosting remotely.

## Risks / Open Items
- vcpkg build is heavy (boost, cgal) and may fail on missing system libs (X11, GL, audio).
- Qt6 path mismatch in presets must be corrected.
- Server/client CLI args and port defaults need verification from source.
- FMOD/RakNet submodules may be empty → build failures.
- CEF enabled: configure step downloads large CEF binary (~hundreds MB) and may fail on network/proxy; ensure connectivity.

## Validation
- `cmake --build` succeeds with zero errors.
- Server binary starts and listens on configured port.
- Player connects and joins a place.

## Out of Scope
- Studio build, tests, packaging, Windows/macOS.

## Execution log (2026-10-21) — DONE (build), client connect deferred

### Environment
- vcpkg cloned to `vcpkg/` at baseline `29ff5b8131d0c6c8fcb8fbaef35992f0d507cd7c`, bootstrapped, `vcpkg install` finished (153 ports, 55 min).
  `gperf` had to be fetched manually from `ftp.gnu.org` (ftpmirror.gnu.org timed out) into `vcpkg/downloads/`.
- Qt: **6.8.3 at `/home/admin/Qt/6.8.3/gcc_64`** (system Qt 6.4.2 lacks the `Qt6Designer` cmake config, which QtitanRibbon needs).
- CEF `140.1.14+geb1c06e+chromium-140.0.7339.185` linux64 pre-downloaded + extracted into `third-party/cef/`.

### Repo fixes required to build
| File | Fix |
|---|---|
| `CMakePresets.json` | Linux presets → real Qt6 path (`/home/admin/Qt/6.8.3/gcc_64`) |
| `CMakeLists.txt` | global `include_directories()` for engine/app, engine/core, engine/gfx, engine/network, RakNet, BulletPhysics (usage reqs are NOT propagated through `$<TARGET_OBJECTS:>`) |
| `CMakeLists.txt` | link `${QT_LIBRARIES}` into `Player` (it was only linked into `Studio`) |
| `engine/app/CMakeLists.txt` | `engine/app` → `engine/app/src` include dir; added `Linux/FileSystem.cpp`, `Linux/MemoryStats.cpp`, `Linux/MachineIdUploader.cpp`; DiscordIntegration only for `AppPlayer`; CEF link uses plain signature |
| `engine/core/CMakeLists.txt` | `src/AyaAssert.*` → `src/RbxAssert.*`, `src/AyaFormat.*` → `src/RbxFormat.*` (files do not exist) |
| `engine/network/src/GameConfigurer.cpp` | guard `SharedLauncher.hpp` include (Windows-only header) |
| `client/studio/CMakeLists.txt` | `target_sources(Player ...)` → `target_sources(Studio ...)` (Player not defined yet at that point) |
| `client/core-script-compiler/CMakeLists.txt` | add `ContextGLSDL2.cpp`; fix `LuaGenCS.inl` output path |
| `client/player/CMakeLists.txt` / `Application.hpp` / `Document.cpp` | drop `SharedLauncher.hpp` include (Windows-only, unused) |
| `client/server/CMakeLists.txt` | new `src/ServerMain.cpp`, include dirs, absolute path for `test.lua` copy |
| `client/server/src/ThumbnailGenerator.cpp` | `RBX_REGISTER_CLASS(ThumbnailGenerator);` |
| `client/server/src/ServerMain.cpp` (new) | native `main()` — the original RCCService `main.cpp`/`server.cpp` need gSOAP generated stubs that are not in the repo |

### Result
- `build/x64-linux-rel/bin/`: `Aya.Server`, `Aya.Player`, `Aya.CoreScriptCompiler`, `Aya.WebHelper`.
- Server smoke test OK:
  `./Aya.Server --localServer --insecure --localServerPort 53640 --localServerPlace rbxasset://avatar.rbxl`
  → `Started network server 127.0.0.1|53640` / `== Aya: Started place ID 1818 on port 53640 ==`
  (there is no `test.rbxl` in `client/resources/content`; `avatar.rbxl` is the only place shipped).
- Player needs a display + GPU; this box is headless so it only fails at GL/CEF init.
  Run it from the `bin` dir (it resolves `content/` relative to cwd) and with a real X display.

## Update: original RCCService restored (gSOAP)

The user provided the gSOAP bundle (`gSOAP/` at repo root: `RCCService.wsdl`, `generated/`, `stdsoap2.h/.cpp`, `import/`, `generate.bat`).

### What changed
- `client/server/src/generated/` populated from `gSOAP/generated/` (soapC.cpp, soapRCCServiceSoapService.cpp/.h, soapH.h, soapStub.h, prototypes.h, RCCServiceSoap.nsmap)
- `client/server/src/stdsoap2.h` + `stdsoap2.cpp` copied
- `client/server/src/Windows/Information.h` — empty Linux stub (only included, no symbols used)
- `client/server/CMakeLists.txt`: uncommented `src/main.cpp` + `src/server.cpp`; removed `src/ServerMain.cpp` from build; added `stdsoap2.cpp` + `generated/soapC.cpp` + `generated/soapRCCServiceSoapService.cpp`; added `VERSION_MAJOR_MINOR_PATCH_STR`/`VERSION_FULL_STR` compile defs; compiled the 3 gSOAP `.cpp` as C++14 (`-std=gnu++14 -D_GNU_SOURCE`)
- Patched `stdsoap2.h`: `xlocale.h` → `<locale.h>` on glibc ≥ 2.26
- Patched `generated/soapRCCServiceSoapService.h`: reverted second param to pointer (`*`), matching the generated stubs
- `server.cpp`: changed method signatures `&` → `*` and field accesses `.` → `->` for the response param (18 methods)
- `main.cpp`: `Execute`/`OpenJob` calls pass `&response`
- `ThumbnailGenerator.cpp`: removed duplicate `RBX_REGISTER_CLASS` (already in `server.cpp`)

### Smoke test
```
cd build/x64-linux-rel && ./bin/Aya.Server
→ Starting Aya.Server v1.0.0...
→ Started Aya.Server!
→ Now listening for incoming SOAP requests on port TCP/64989
```
SOAP/RCCService is up. The `ServerMain.cpp` shim remains in the tree (not compiled) as a fallback.
