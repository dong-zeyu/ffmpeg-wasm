# ffmpeg-wasm
A wasm built for FFMpeg

## How to build

First follow the [instruction](https://emscripten.org/docs/getting_started/index.html) to setup emscripten environment. 

Clone the repository and update the ffmpeg module

```bash
git clone <url>
git submodule update --init
```

Build the source

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## How to test

Put `test.mp4` in `build` folder.

Inside build folder, run

```bash
python ../run.py  # You may need to install rangehttpserver
```
