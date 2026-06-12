# Full-Stack Graph Algorithm Engine & Traversal Simulator

A high-performance algorithmic benchmarking and visualization suite designed to analyze graph traversal algorithms. This project bridges a highly optimized, compiled C++17 backend with a responsive, canvas-based JavaScript frontend. 

The core objective of this project is to move beyond standard console-based output, providing a real-time, interactive environment to compare the time complexity, spatial exploration, and operational efficiency of Dijkstra’s Algorithm, A* Search, Breadth-First Search (BFS), and Depth-First Search (DFS) on massive datasets.

## ⚙️ System Architecture

The project enforces a strict separation of concerns, decoupling heavy mathematical computations from visual rendering.

* **Backend (C++17):** Responsible for data structure management, random graph generation, algorithmic execution, and high-precision timing. Compiled via CMake and MinGW.
* **Data Bridge (JSON):** The backend serializes graph topology and algorithm step-data into a custom `results.json` payload, acting as a language-agnostic bridge.
* **Frontend (HTML5, CSS3, JS):** Reads the JSON payload, dynamically scales the coordinate space to the user's viewport (preventing native canvas stretching), and orchestrates the step-by-step animation using HTML5 Canvas.

## 🧠 Core Data Structures & Logic

Standard library wrappers were bypassed in favor of custom implementations to maximize performance and demonstrate a fundamental understanding of systems logic.

* **Adjacency List Graph:** Optimizes memory for sparse graphs while allowing O(1) access to neighboring nodes.
* **Custom Min-Heap:** A priority queue built entirely from scratch to drive Dijkstra's algorithm. It utilizes a zero-indexed array representation with custom `heapifyUp` and `heapifyDown` operations, ensuring strict O(log N) extraction times.
* **Algorithmic Implementations:**
  * **Dijkstra's:** Baseline for weighted shortest-path discovery using the custom Min-Heap.
  * **A* Search:** Built upon Dijkstra but introduces a Euclidean distance heuristic function `std::sqrt(dx*dx + dy*dy)` to heavily prune the search space.
  * **BFS & DFS:** Implemented to demonstrate the visual and computational contrast between radiating uniformly outward (BFS) and deep, backtracking exploration (DFS).

## ⏱️ Microsecond Benchmarking Engine

Because C++ `-O2` release optimizations execute these algorithms in fractions of a millisecond, standard OS timers fail to capture execution time accurately. 

To solve this, the engine implements a **Loop Averaging Strategy**. Each algorithm is forced to execute 1,000 times back-to-back inside the `std::chrono::high_resolution_clock` boundary. The total time is then divided by 1,000, yielding a hyper-accurate, microsecond-level (µs) performance metric.

## 🚀 Build and Run Instructions

### Prerequisites
* **C++ Compiler:** MinGW (GCC) installed and added to PATH.
* **CMake:** Version 3.14 or higher.
* **Python 3:** For hosting the local web server.

### 1. Compile the Backend
Open your terminal in the root directory and run:
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```
### 2. Generate Data
Return to the root directory and run the executable to generate a graph and calculate algorithms. (Example: 30 nodes)
```bash
cd ..
.\build\pathfind.exe --generate 30 --src 0 --dest 29 --algo all --out output\results.json
```
### 3. Launch the Visualizer
Start a local web server from the root directory to allow the JavaScript to fetch the JSON payload:
```bash
python -m http.server 8000
```
Open your web browser and navigate to http://localhost:8000.