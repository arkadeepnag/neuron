cmake_minimum_required(VERSION 3.16)
project(neuron)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)

add_executable(neuron
    src/main.cpp
    src/repo.cpp
    src/refs.cpp
    src/checkout.cpp
    src/merge.cpp
    src/index.cpp
    src/hash.cpp
    src/commit.cpp
    src/tree.cpp
    src/object_store.cpp
    src/blockstore_fs.cpp
    src/blockstore_mem.cpp
    src/blockstore_chain.cpp
    src/utils.cpp
    src/remote.cpp
)

add_executable(blockstore_test
    tests/blockstore_test.cpp
    src/blockstore_fs.cpp
    src/blockstore_mem.cpp
    src/blockstore_chain.cpp
    src/utils.cpp
)
target_link_libraries(blockstore_test ZLIB::ZLIB)

add_executable(mem_blockstore_test
    tests/mem_blockstore_test.cpp
    src/blockstore_mem.cpp
)
target_link_libraries(mem_blockstore_test ZLIB::ZLIB)

target_include_directories(neuron PRIVATE src)
find_package(OpenSSL REQUIRED)

target_link_libraries(neuron ZLIB::ZLIB OpenSSL::Crypto)
