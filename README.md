# Search Engine in C++

## Description
This project is a console-based search engine that processes search queries using an inverted index built from text documents.

## Project Structure
- **`ConvertJson.cpp`** – Handles loading and saving data in JSON format.
- **`InvertedIndex.cpp`** – Builds the inverted index for documents.
- **`SearchServer.cpp`** – Processes search queries and ranks results.
- **`main.cpp`** – The main program file that integrates all components.

## Requirements
- C++ Compiler (e.g., `g++` or `clang`)
- `CMake` for building
- [nlohmann/json](https://github.com/nlohmann/json) library for JSON handling

## Installation and Running
### 1. Clone the Repository
```sh
git clone https://github.com/Elsudeia/SimpleSearchEngineProject.git
cd yourrepository
```

### 2. Build the Project
```sh
mkdir build && cd build
cmake ..
make
```

### 3. Run the Program
```sh
./simple_search_engine
```

## Configuration
Before running, ensure that the following files are in the project root directory:
- **`config.json`** – List of document files.
- **`requests.json`** – List of search queries.

You can change the file paths in ConvertJSON.h.
If they are missing, the program will generate sample versions automatically.

## Output
Search results are saved in `answers.json` in the following format:
```json
{
    "answers": {
        "request1": {"result": "true", "relevance": [{"docid": 0, "rank": 1.0}]},
        "request2": {"result": "false"}
    }
}
```

## License
This project is distributed under the MIT License.
