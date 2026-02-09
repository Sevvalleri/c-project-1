# CPU Scheduling Simulation using Binomial Heap (C)

This project is a **CPU scheduling simulator** written in **C**, implementing a **preemptive scheduling algorithm** using a **Binomial Heap** as the priority queue.

The simulator evaluates different **time quantum values** and selects the one that minimizes the **Average Waiting Time (AWT)**.

---

# Features

- Binomial Heap implementation from scratch
- Dynamic priority calculation based on remaining execution time
- Preemptive CPU scheduling with time quantum
- Automatic selection of the best quantum value
- Detailed execution trace (process arrival, execution, re-queuing, completion)
- Average Waiting Time (AWT) calculation

---

# Scheduling Logic

- Each process is assigned a priority.
- The process with the **minimum priority value** is selected from the binomial heap.
- If a process is not completed within the given quantum, it is **re-queued** with an updated priority.
- Priority is recalculated using an exponential weighting function.

---

# Input Format

The program reads process data from a file named **`Metin.txt`**.

Each line must be in the following format:

