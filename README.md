# Caching Simulator Project

## Overview

This project consists of an assembler and a caching simulator that work together to simulate a simple processor with a cache system. The primary purpose of this project is to demonstrate and analyze the behavior of a cache memory system in a simplified computer architecture.

### Educational Objectives

This project serves as an educational tool for understanding:

1. Basic computer architecture concepts
2. Assembly language programming
3. Cache memory operations and their impact on performance
4. Memory hierarchy and its effects on program execution

### Key Learning Objectives

- Understanding the relationship between assembly language and machine code
- Observing cache hit and miss patterns
- Analyzing the effects of different cache configurations on performance
- Identifying potential performance bottlenecks in code due to memory access patterns
- Gaining insights into optimizing code for better cache utilization

## Cache Configurations and Performance

### Supported Configurations

The simulator supports various cache configurations, denoted by X-Y, where X is the number of cache blocks and Y is the block size in words:

- 1-8: 1 cache block with 8 words per block
- 4-2: 4 cache blocks with 2 words per block
- 4-8: 4 cache blocks with 8 words per block
- 8-1: 8 cache blocks with 1 word per block
- 8-8: 8 cache blocks with 8 words per block

### Performance Impact

These configurations can significantly impact cache performance:

1. 1-8: Good for programs with high temporal locality but may suffer from conflict misses.
2. 4-2: Better for programs accessing multiple memory locations but with less spatial locality.
3. 4-8: Balanced configuration, suitable for many general-purpose applications.
4. 8-1: Effective for programs with high temporal locality accessing many different locations.
5. 8-8: Largest cache, generally good for most programs but may be excessive for small applications.

## Configuring Cache Settings

To make different cache configurations, modify the following variables in `caching.cpp`:

- `NUM_CACHE_BLOCKS`: The number of cache blocks (X)
- `BLOCK_SIZE`: The block size in words (Y)

### LRU Policy Implementation

The Least Recently Used (LRU) policy is implemented using a reference count system:

1. Each cache block has an associated reference count (ref_count in the CACHE_ENTRY structure).
2. A global counter (current_ref_count) is incremented each time a cache block is accessed.
3. When a block is accessed, its ref_count is updated to the current global counter value.
4. When a new block needs to be brought into a full cache:
   - The removeLRU() function is called to find the block with the lowest ref_count.
   - This block is considered the least recently used and is selected for replacement.
   - The selected block is written back to main memory if it's dirty (modified).
   - The new block is then loaded into the cache in its place.

## Components

1. Assembler (assembler.cpp)
2. Caching Simulator (caching.cpp)

### Assembler

The assembler takes assembly code as input and produces object code for the caching simulator.

#### Usage

```bash
./assembler <input_file.asm>
```

#### Features

- Supports basic arithmetic, logical, move, shift, and branch instructions
- Handles labels for branching
- Generates binary output compatible with the simulator

### Caching Simulator

The caching simulator emulates a processor with a cache, executing the object code produced by the assembler.

#### Usage

```bash
./caching <object_file> <data_file>
```

#### Features

- Configurable cache size and block size
- Implements a Least Recently Used (LRU) cache replacement policy
- Simulates memory reads and writes through the cache
- Provides statistics on cache hits and misses

#### Caching Simulation Process

1. Maintains a cache structure with configurable blocks and block sizes.
2. Implements an LRU replacement policy using a reference count.
3. Simulates cache reads and writes.
4. Tracks and reports cache hit/miss statistics.

## Memory Organization

- Code memory: 1024 words
- Data memory: 1024 words
- Word size: 2 bytes
- Cache: Configurable number of blocks and block size

## Building the Project

To compile the project, use the following commands:

```bash
g++ -std=c++11 -o assembler assembler.cpp
g++ -std=c++11 -o caching caching.cpp
```

## Running a Simulation

1. Write your assembly code in a file (e.g., test1.asm)
2. Assemble the code:

   ```bash
   ./assembler test1.asm
   ```

3. Prepare a data file (e.g., test1.dat)

   The data file should contain the initial values for the data memory. Each value should be represented in hexadecimal format. For example, the content of `test1.dat` could be:

   ```bash
   010100052386F4E42CABFF5D0096FFFF
   FFFF0096FF5D2CABF4E4238600050032
   003200052386F4E42CABFF5D0096FFFF
   ```

   This represents the initial values to be loaded into the data memory before the simulation starts. Ensure that the data file is properly formatted and contains the necessary values for your simulation.

   The data file should be written in a way that aligns with the assembly code and caching simulation. Each line in the data file corresponds to a memory address in the data memory. The values should be in hexadecimal format and should match the expected memory layout for the simulation. For example, if your assembly code expects certain values at specific memory addresses, ensure that the data file provides those values in the correct order.

   Additionally, the data file should be structured to work with the cache configuration. If the cache is set up with a specific block size, the data file should be organized to fit within those blocks. This ensures that the cache simulation accurately reflects the memory accesses and cache hits/misses during the simulation.

   When creating a .dat file, keep in mind the following constraints from the assembler and caching simulator:

   - WORD_SIZE: 2 bytes
   - DATA_SIZE: 1024 words (2048 bytes)
   - BLOCK_SIZE: Configurable, default is 8 words
   - LINE_LENGTH: 32 bytes (16 words) for caching simulator

   Ensure that your .dat file doesn't exceed the DATA_SIZE limit of 1024 words (2048 bytes). The caching simulator will use this data to initialize its memory blocks.

4. Run the simulation:

   ```bash
   ./caching test1.o test1.dat
   ```

## Output

The simulator provides detailed output of each instruction's execution, including:

- Instruction fetch and decode information
- ALU operations
- Memory accesses
- Cache hits and misses

At the end of the simulation, it displays:

- Reason for simulation termination (successful completion, illegal opcode, etc.)
- Cache statistics (hit rate)
- Final state of data memory
