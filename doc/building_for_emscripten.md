# Building for Emscripten (WebAssembly)

This document describes the process to build Ocean for WebAssembly using Emscripten.

Ocean's Emscripten support covers its core computer-vision stack. It is single-threaded and does not use WebAssembly SIMD; see [Supported modules and limitations](#6-supported-modules-and-limitations) before starting.

## 1 Prerequisites

* [General prerequisites listed on the main page](../README.md)
* Python 3.10 or higher, plus the build scripts' dependencies: `pip install -r build/python/requirements.txt`
* The [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html), with the `EMSDK` environment variable set:
  ```bash
  source /path/to/emsdk/emsdk_env.sh
  echo "$EMSDK"        # must print the SDK path
  emcc --version
  ```
* Node.js, to run the test executables. The emsdk ships one under `$EMSDK/node/`.

`EMSDK` is the only thing the build system looks for. It is set by `emsdk_env.sh` and by the official Docker image alike, so nothing below depends on which one you use.

> **Note:** Verified with Emscripten 4.0.23. Ocean requires CMake 3.26 or higher, which is newer than the CMake bundled with some emsdk distributions — check with `cmake --version`.

### Building inside Docker instead

If you would rather not install the emsdk on your machine, the official image provides the same environment. This is a convenience, not a requirement.

The image ships CMake 3.22, which is older than Ocean requires, so install a newer one in the same command:

```bash
docker run --rm -v "$PWD":/src -w /src emscripten/emsdk:4.0.23 bash -c '
    pip install --quiet "cmake==3.31.6" pyyaml &&
    python3 build/python/build_ocean_3rdparty.py --target emscripten_wasm32 \
        --library eigen --config release --link static'
```

Every command in the sections below works the same way: wrap it in the same
`docker run ... bash -c '...'`, keeping the `pip install` prefix. The directories
and output paths are identical either way, since the repository is mounted at the
same place the build writes to.

CMake is pinned to 3.31 rather than 4.x because CMake 4 removed support for `cmake_minimum_required(VERSION <3.5)`, which several third-party libraries still declare.

## 2 Building the third-party libraries

Only Eigen is needed for the modules Ocean currently builds for Emscripten, and it is header-only.

```bash
cd /path/to/ocean

python build/python/build_ocean_3rdparty.py --target emscripten_wasm32 \
    --library eigen --config release --link static
```

The installed library appears under `ocean_3rdparty/install/emscripten_wasm32_static/eigen/`, following the same layout as every other platform.

Third-party libraries do not build for Emscripten unless they say so. `platforms: all` in `build/python/dependencies.yaml` means every platform Ocean builds natively; a library that genuinely supports Emscripten has to name it. Most of Ocean's dependencies need dedicated work to build under `emcc` at all, and enrolling them by default would turn that into a cascade of unrelated link errors. See `OPT_IN_PLATFORMS` in `build/python/lib/manifest.py`.

## 3 Building Ocean

```bash
cd /path/to/ocean

python build/python/build_ocean.py --target emscripten_wasm32 \
    --config release --link static \
    --third-party-dir ./ocean_3rdparty/install
```

The compiled libraries can be found in `ocean_install/emscripten_wasm32_static/lib`.

> **Note:** CMake reads `CMAKE_TOOLCHAIN_FILE` only when it first configures a build directory. If a configure fails and you re-run it after a fix, delete the build directory first, or CMake will silently reuse the previous configuration and report `OCEAN_TARGET_SYSTEM_NAME = linux`.

### Using CMake Directly

`emcmake` sets `CMAKE_TOOLCHAIN_FILE` and then calls CMake, so the two forms below are equivalent:

```bash
emcmake cmake -S . -B build_emscripten \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install

cmake --build build_emscripten -j
```

A successful configure prints:

```
-- OCEAN_TARGET_SYSTEM_NAME = emscripten
-- OCEAN_TARGET_PROCESSOR_TYPE = wasm32
```

If it prints `linux` instead, the toolchain file was not applied — see the note above.

### Link settings applied automatically

These are set for every Emscripten executable, in the root `CMakeLists.txt`:

| Setting | Why |
|---------|-----|
| `-sALLOW_MEMORY_GROWTH=1` | Emscripten's default heap is a fixed 16 MB. Ocean's test executables exhaust it and abort with `Cannot enlarge memory arrays`. |
| `-sSTACK_SIZE=8MB` | The default is 64 KB, against the 8 MB a Linux main thread gets. Release builds do not check for overflow, so exceeding it surfaces as `RuntimeError: memory access out of bounds` somewhere unrelated. |
| `-O3` on the link line | `emcc` runs Binaryen's `wasm-opt` over the whole module at link time and takes its level from there, while CMake puts the release flags on compile lines only. |

## 4 Running the tests

Ocean's console test executables become a `.js` / `.wasm` pair and run under Node.js with no flags:

```bash
cd ocean_build/emscripten_wasm32_static/impl/application/ocean/test/math/testmath
node application_ocean_test_math_testmath.js
```

Each executable accepts `--duration` (`-d`) for the per-test duration in seconds and `--functions` (`-f`) for a comma-separated subset of suites. Pass `--help` for the full list.

The default duration is 2 seconds per test, which over a full suite runs for a long time under WebAssembly. `-d 0.1` exercises every test with less sampling and is usually what you want while developing:

```bash
node application_ocean_test_math_testmath.js -d 0.1
```

### Running in a browser

Each supported test application carries a `wasm/index.html` shell, the counterpart of the `android/` and `ios/` shells beside it. The build copies it next to the generated `.js`, so serving that directory is enough:

```bash
cd ocean_build/emscripten_wasm32_static/impl/application/ocean/test/math/testmath
python -m http.server 8000
# then open http://localhost:8000/
```

A `.wasm` cannot be instantiated from a `file://` origin, so it has to be served over HTTP.

The page reports `passed` or `FAILED` once the run completes. Note that `main()` executes synchronously and blocks the page's main thread, so the tab is unresponsive while the tests run; the shells therefore run a small subset at a reduced duration. **The browser run is a smoke test in a real engine, not a replacement for the Node.js run.**

### Test suites that fail by construction

`testbase` aborts in a single-threaded build. Six of its suites create threads and cannot pass:

`lock`, `signal`, `thread`, `threadpool`, `worker`, `workerpool`

They are deliberately left enabled rather than disabled, so the limitation stays visible. To run the rest:

```bash
node application_ocean_test_base_testbase.js \
    -f "binary,callback,caller,commandarguments,datatype,datetime,frame,hashmap,hashset,highperformancestatistic,highperformancetimer,inheritance,kdtree,median,memory,movebehavior,randomi,ringmap,scopedfunction,scopedobject,segmentunion,singleton,stackheapvector,staticbuffer,staticvector,stl,string,subset,timestamp,utilities,value"
```

### `testgeometry` is slow

Running the full geometry suite to completion under WebAssembly is impractical: its robust-estimator and bundle-adjustment tests take hours. Use a subset instead:

```bash
node application_ocean_test_geometry_testgeometry.js \
    -f "absolutetransformation,delaunay,epipolargeometry,error,homography,jacobian,octree,p3p,p4p,pnp,spatialdistribution,stereoscopicgeometry,utilities"
```

**Reducing `--duration` does not help.** Every test runs at least one full iteration regardless of the budget, and it is the iterations that are expensive — roughly 90 ms for a robust-estimator step and 400 ms for a bundle-adjustment one. Selecting suites with `--functions` is the only effective lever.

The cost traces to WebAssembly SIMD being disabled: native builds compile with `-msse4.1 -mavx2`, so Eigen's dense matrix paths run vectorised there and scalar here.

## 5 Timing measurements

`HighPerformanceTimer` values are correct in magnitude but **quantized to 1 ms**. Browsers coarsen timer resolution to blunt Spectre-style attacks, and Emscripten reports `CLOCK_REALTIME` accordingly. Individual measurements land on millisecond boundaries; averages over many iterations are still meaningful. Sub-millisecond benchmarking is not available on this platform.

## 6 Supported modules and limitations

| Module | Supported |
|--------|-----------|
| `base`, `math`, `geometry`, `system` | Yes |
| `cv`, including the QR code, barcode and bullseye detectors | Yes, except `cv/fonts` |
| `cv/fonts` | No — needs Freetype, HarfBuzz and PNG |
| `io`, `media` | No |
| `network` | No — POSIX sockets need a WebSocket proxy |
| `devices`, `platform`, `interaction`, `rendering`, `scenedescription`, `tracking` | No |

Unsupported modules are excluded by their CMake guards, so they are absent from the build rather than failing in it.

**Single-threaded.** Ocean is built without `-pthread`. `Worker` detects a single core and runs work inline on the calling thread, so code that takes an optional `Worker*` behaves correctly; code that creates threads directly does not. A pthreads build would require the hosting page to serve `Cross-Origin-Opener-Policy: same-origin` and `Cross-Origin-Embedder-Policy: require-corp`, which many deployment targets cannot.

**No WebAssembly SIMD.** `-msimd128` is not enabled. It would produce a `.wasm` that does not run in browsers lacking SIMD support, and that Node.js rejects without an explicit flag.
