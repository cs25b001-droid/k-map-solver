# k-map-solver

# K-Map Minimizer

A C++ program to find all possible minimal Sum-of-Products (SOP) Boolean expressions from a Karnaugh Map (K-Map) provided in a text file.

---

## Features

- **Dynamic Grid Support:** Automatically parses standard 2-variable ($2 \times 2$), 3-variable ($2 \times 4$), and 4-variable ($4 \times 4$) K-maps.
- **Gray Code Coordinate Mapping:** Converts 2D matrix positions into accurate minterm decimal indices using standard Gray code sequences (`00`, `01`, `11`, `10`).
- **Boolean Hypercube Subcube Enumeration:** Identifies all valid power-of-two rectangular groupings to extract Maximal Prime Implicants without relying on manual visual grouping.
- **Branch-and-Bound Exact Cover:** Solves the minimum set-cover problem with reachability pruning to determine all equally optimal minimized expressions.

---

## Input File Format

The program reads a plain text file containing space-separated binary values (`0` and `1`). 

### Example (`kmap.txt` for 4 variables: $a, b, c, d$)
```text
0 1 0 1
1 0 0 0
1 0 0 1
0 0 0 1
