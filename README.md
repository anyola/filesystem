# Hierarchical File System

A small, safety-focused simulation of a hierarchical file system, built to
demonstrate modern C++ design: RAII ownership, polymorphism, and
const-correctness — verified with unit tests, static analysis, and sanitizers.

## Design

- `fsobject` — abstract base (name, parent link, path resolution)
- `file` — leaf node holding string content
- `directory` — owns children via `std::vector<std::unique_ptr<fsobject>>`
- `symlink` — stores a target path and resolves it through the tree,
  with cycle detection and a configurable max-depth limit

Deep copies go through a virtual `clone()`. Ownership is single and explicit:
a `directory` owns its children; moving an object between directories transfers
that `unique_ptr`, it never duplicates it.

## Features

- Find by name (recursive) or by path (segment-by-segment)
- Add / remove / rename with duplicate-name and invalid-name checks
- Copy (deep) and move, both guarding against self-references and
  moving/copying a directory into its own descendant
- Symlinks with chain resolution and cycle detection

## Testing & Analysis

- GoogleTest unit tests covering normal and edge cases
- Built with `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror`
- AddressSanitizer + UndefinedBehaviorSanitizer
- clang-tidy (`modernize-*`, `performance-*`, `bugprone-*`, `clang-analyzer-*`)

## Build & Run

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
./filesystem
ctest --output-on-failure
```