cmake_minimum_required(VERSION 3.1)

project (kn5toac VERSION 0.1 LANGUAGES CXX)

configure_file(config.h.in config.h)

add_executable(kn5toac kn5toac.cpp kn5.h kn5.cpp ini.h ini.cpp lut.h lut.cpp)

target_compile_features(kn5toac PUBLIC cxx_std_17)
target_include_directories(kn5toac PUBLIC "${PROJECT_BINARY_DIR}")
target_compile_options(kn5toac PUBLIC $<$<CXX_COMPILER_ID:GNU>:-Wno-psabi -Wall -Wextra -Wpedantic>)
target_link_libraries(kn5toac PUBLIC $<$<CXX_COMPILER_ID:GNU>:stdc++fs>)

install(TARGETS kn5toac DESTINATION bin)
