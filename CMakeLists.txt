cmake_minimum_required(VERSION 2.6)

project(secret-spice-flat)

include_directories(flat/src)

file(
    GLOB_RECURSE
    src
    src/*
    flat/src/*
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Werror")

add_executable(
    secret-spice-flat
    ${src}
)

target_link_libraries(
    secret-spice-flat
    lua5.2
    SDL2
    SDL2_image
    SDL2_mixer
    SDL_ttf
    GL
    GLU
    GLEW
    essentia
    avcodec
    avformat
    avutil
    yaml
    fftw3f
    tag
    samplerate
)
