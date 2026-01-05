Neuron
======

Neuron is a small command-line tool for managing a content-addressable object storage and basic repository operations. It implements core features such as hashing, indexing, commits, and a simple object store, focused on performance and clarity for local development and testing.

Key features
- Content-addressable object storage backed by local files
- Commit and tree operations similar to lightweight version control primitives
- Pluggable blockstore implementations for memory, filesystem, and chain storage
- Test suite for blockstore implementations and basic push/pull integration

Getting started
1. Install dependencies: ensure a C++17-capable compiler and CMake are installed.
2. Build:

   mkdir -p build
   cd build
   cmake ..
   make

3. Run tests:

   cd build
   ctest --output-on-failure

Development notes
- Source files are located in `src/` and tests in `tests/`.
- The project uses CMake for build configuration.
- Keep binary artifacts and build directories out of version control (see `.gitignore`).

Contributing
- Open issues for bugs or feature requests.
- Fork, make changes on a feature branch, and submit a pull request with a clear description.

License
- This repository does not include a license file. Add one if you plan to make the project public.
