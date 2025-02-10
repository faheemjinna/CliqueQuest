# **CliqueQuest: Optimizing Test Data Compression through Graph-Based Dictionary Selection**

## **Overview**
This project focuses on **dictionary-based test data compression** in System-on-Chip (SoC) design. The goal is to select entries for a dictionary by modeling the test set as a graph and applying a heuristic procedure to solve a **clique partitioning problem**. The project involves implementing a program in **C++** to partition the graph into cliques and generate dictionary entries based on the provided test data.

---

## **Project Description**
### **Problem Statement**
- **Input**: A test set represented as a graph, where two words are compatible (i.e., there is an edge between them) if for any position, the bits are either equal or at least one is a don’t-care bit (`x`).
- **Objective**: Partition the graph into cliques using a heuristic procedure and generate dictionary entries for the given test data.

### **Heuristic Procedure**
1. Copy the graph `G` to a temporary data structure `G'`.
2. Find the vertex `v` with the maximum degree in `G'`.
3. Establish a subgraph consisting of all vertices connected to `v`. Copy this subgraph to `G'` and add `v` to a set `C`.
4. If `G'` is not empty, repeat from Step 2. Otherwise, a clique `C` has been formed.
5. Remove the vertices in the clique `C` from `G` and copy `G - C` to `G'`. Repeat until `D` cliques are found.

---

## **Requirements**
### **Input**
- A test file containing test vectors (e.g., `s15850f.test`).
- The number of dictionary entries (e.g., `4`).
- The length of each test vector (e.g., `32` bits).
- The output file name (e.g., `s15850f_dict`).

### **Output**
- A dictionary file (e.g., `s15850f_dict`) containing the selected dictionary entries.
- If the number of dictionary entries requested exceeds the number of unique cliques possible, the program should output:  
  `Only X dictionary entries are possible`, where `X` is the number of unique cliques.

### **Error Handling**
- If the input file name is incorrect, the program should output `Wrong file name` and terminate.
- If the number of command-line arguments is incorrect, the program should output `Incorrect number of arguments` and terminate.

---

## **How to Run the Program**
1. **Compile the Code**:
   - Ensure your code is in a zipped folder with a `Makefile`.
   - Compile the code using:  
     ```bash
     make
     ```
   - The executable output must be named `dict`.

2. **Run the Program**:
   - Run the program with the following command:  
     ```bash
     ./dict s15850f.test 4 32 s15850f_dict
     ```
   - Replace `s15850f.test` with the input file, `4` with the number of dictionary entries, `32` with the test vector length, and `s15850f_dict` with the output file name.

---

## **Test Cases**
Four test cases have been provided. A sample output file (`sample.dict`) is also included for reference.
