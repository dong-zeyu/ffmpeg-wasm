cmake_minimum_required(VERSION 3.0.0)
project(FFmpeg-wasm VERSION 0.1.0)

set(CMAKE_C_COMPILER emcc)
set(CMAKE_CPP_COMPILER em++)
add_link_options(
    "SHELL:-s WASM=1"
    "SHELL:-s FETCH=1"
    "SHELL:-s USE_PTHREADS=1"
    "SHELL:-s PROXY_TO_PTHREAD=1"
    "SHELL:-s INITIAL_MEMORY=134217728"
    # "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s SAFE_HEAP=1"
    "-O3"
)
add_link_options()

include(ExternalProject)
include(ProcessorCount)

ProcessorCount(N)
if(NOT N EQUAL 0)
  set(MAKE_ARGS -j${N})
endif()

ExternalProject_Add(
    ffmpeg
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/FFmpeg"
    CONFIGURE_COMMAND 
        "emconfigure"
        "${CMAKE_SOURCE_DIR}/FFmpeg/configure"
        "--prefix=${CMAKE_BINARY_DIR}/FFmpeg"
        "--enable-gpl"
        "--enable-version3"
        "--enable-nonfree"
        "--disable-autodetect"
        "--disable-programs"
        "--disable-doc"
        "--disable-everything"
        "--enable-decoders"
        "--enable-demuxers"
        "--enable-cross-compile"
        "--nm=lvm-nm -g"
        "--ar=emar"
        "--cc=emcc"
        "--cxx=em++"
        "--objcc=emcc"
        "--dep-cc=emcc"
        "--ranlib=llvm-ranlib"
        "--extra-cflags=-O3"
        "--extra-cxxflags=-O3"
        "--disable-asm"
        "--disable-debug"
    BUILD_COMMAND "make" "${MAKE_ARGS}"
    INSTALL_COMMAND "make" "install"
)
set(FFMPEG_LIBS avcodec avfilter avutil swresample avdevice avformat postproc swscale)
include_directories("${CMAKE_BINARY_DIR}/FFmpeg/include")
link_directories("${CMAKE_BINARY_DIR}/FFmpeg/lib")

add_executable(test.html main.c wasm_io.c demuxing_decoding.c)
add_dependencies(test.html ffmpeg)
target_link_libraries(test.html ${FFMPEG_LIBS})
