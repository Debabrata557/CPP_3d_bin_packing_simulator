**3D Bin Packing Simulator**

A C++ simulator for the 3D Bin Packing Problem (3D-BPP) — packing a stream of variable-sized 3D boxes into the minimum number of fixed-size bins.

Implements and benchmarks multiple packing heuristics (First Fit, Floor Building, Smart variants) alongside a Python-based evolutionary strategy training pipeline for learning-based approaches.

Note: This project depends on ROS (Robot Operating System) and cannot be run standalone without a ROS environment set up.

**Tech Stack**

C++ — simulation core, packing algorithms, collision detection
Python — evolutionary strategy trainer and policy training scripts
Make — build system
