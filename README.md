
# Project Overview

This project is managed using a Makefile, supporting simple commands to compile and run the program.

## Compile the Project

To compile the entire project, use the following command:

```bash
make
```

## Run the Program

To run the program, you can pass parameters via the `make run` command. The detailed usage of the parameters is as follows:

```bash
make run ARGS="param1 param2 param3 param4 param5 param6 param7"
```

### Parameter Details

- **param1:** Dataset name
  - **Type:** string
  - **Example:** Testdataset
- **param2:** Community detection algorithm name
  - **Type:** string
  - **Range:** LPA, KL, Louvain, GM 
  - **Example:** LPA
- **param3:** k
  - **Type:** int
  - **Example:** 3
- **param4:** Number of searches
  - **Type:** int
  - **Example:** 10
- **param5:** Number of nodes in a single search
  - **Type:** int
  - **Example:** 1
- **param6:** Whether to construct a query set
  - **Type:** bool
  - **Example:** true
- **param7:** Whether to reconstruct the index
  - **Type:** bool
  - **Example:** true

#### Example Command

```bash
make run ARGS="Testdataset LPA 3 10 1 true true"
```

The default parameters are `Testdataset LPA 3 10 1 true true`, so you can directly run:

```bash
make run
```

## Data Management Approach

- **Dataset:** `data/name/name.txt`
- **Index File:** `data/name/nameIndex.txt`
- **Query File:** `data/name/nameQuery.txt`
- **Historical Dynamic Maintenance Operations File:** `data/name/nameDynamic.txt`
- **Result File:** `data/name/nameResult.txt`

### Example:

- `data/Testdataset/Testdataset.txt`
- `data/Testdataset/TestdatasetIndex.txt`
- `data/Testdataset/TestdatasetQuery.txt`
- `data/Testdataset/TestdatasetDynamic.txt`
- `data/Testdataset/TestdatasetResult.txt`

## Environment Requirements

- **C++ Version:** C++17
- **Operating System:** Debian GNU/Linux 11
- **Compiler:** g++ (Debian 10.2.1-6) 10.2.1 20210110
- **Python Version:** Python 3.9.2
- **Dependencies on External Libraries:** 
  - NetworkX 
  - python-louvain
