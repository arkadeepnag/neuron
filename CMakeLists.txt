cmake_minimum_required(VERSION 3.16)

project(neuron LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)


find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/core/include
    ${PROJECT_SOURCE_DIR}/db
    ${PROJECT_SOURCE_DIR}/cli
    ${PROJECT_SOURCE_DIR}/third_party
)


add_library(neuron-core STATIC
    core/src/object.cpp
    core/src/index.cpp
    core/src/tree.cpp
    core/src/commit.cpp
    core/src/refs.cpp
    core/src/repository.cpp

    core/src/graph.cpp
    core/src/negotiate.cpp
    core/src/packfile.cpp
    core/src/walker.cpp

    core/src/mmap.cpp
    core/src/pack_index.cpp
)

target_link_libraries(neuron-core
    OpenSSL::Crypto
    Threads::Threads
)

add_library(neuron-db STATIC
    db/database.cpp
)

target_link_libraries(neuron-db
    SQLite::SQLite3
)

add_executable(neuron
    cli/neuron.cpp
    cli/push.cpp
    cli/pull.cpp
    cli/clone.cpp
    cli/refs_remote.cpp
)



target_link_libraries(neuron
    neuron-core
    neuron-db
)


add_executable(neuron-ssh-handler
    ssh/neuron_ssh_handler.cpp
    ssh/upload_pack.cpp
    ssh/receive_pack.cpp
    ssh/ls_refs.cpp
)

target_link_libraries(neuron-ssh-handler
    neuron-core
    neuron-db
    OpenSSL::Crypto
    SQLite::SQLite3
    Threads::Threads
)


add_executable(neuron-http
    http/http_server.cpp
)

target_link_libraries(neuron-http
    neuron-core
    neuron-db
    OpenSSL::Crypto
    SQLite::SQLite3
    Threads::Threads
)

