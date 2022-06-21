# ffmpeg-wasm
A wasm built for FFMpeg

## How to build

First clone/update and cd to the root of the source.

Update the ffmpeg module

```bash
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
