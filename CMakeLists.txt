cmake_minimum_required(VERSION 3.10)

project(camgraph)

set(CAMGRAPH_C_FILES src/graphics.c src/graphics/sdlg.c)

include_directories(../libvcol/src)

add_library(camgraph ${CAMGRAPH_C_FILES})

