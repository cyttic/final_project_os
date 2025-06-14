#Operating Systems 2025 Project - Restaurant Simulation


#Project Overview

This project implements a simulation of a fast food restaurant in Tel Aviv, as part of the Operating Systems 2025 course. The program simulates a restaurant with an interactive menu, multiple waiters, and customers, using separate processes for each entity. The menu and orders board are managed in shared memory, with synchronization handled via semaphores to address the readers-writers problem. The simulation tracks customer orders, waiter activities, and generates a final report on dish counts and total income.

#Files Included

main.c: Main program file containing the manager process, menu setup, and simulation logic.

sim.h: Header file defining structures, constants, and function prototypes.

sim.c: Implementation of customer and waiters process behavior (reading menu, placing orders).

timer.h: Header file defining structures, and function prototypes for program timer

timer.c: Implementation of program timer


README.md: This file, providing project details and instructions.

Compilation Instructions

The project is designed to compile on a fully updated 64-bit Ubuntu 22.04.2 LTS distribution. Use the following command to compile:

gcc src/sim.c src/main.c src/timer.c -o restaurant -pthread

Ensure you have the necessary permissions and dependencies installed. The program should compile without errors or warnings.

Execution Instructions

Run the program with the following command-line arguments:

Number of different dishes (5-7)

Number of waiters (up to 3)

Number of customers (up to 10)

Total simulation time (in seconds, less than 30)

Example:

./restaurant 15 6 5 2

This runs the simulation with 6 dishes, 2 waiters, 5 customers, and a 15-second duration.

Program Behavior

Main Process (Manager): Initializes the menu (5-7 dishes with ID, Name, Price, TotalOrdered), creates the orders board, and spawns waiter and customer processes. At the end, it prints a summary of total orders per dish and total income.
Customer Process: Reads the menu, waits randomly (3-6 seconds), and with 50% probability places an order (random item, 1-4 amount) or skips. Orders are written to the shared orders board.
Waiter Process: Checks the orders board every 1-2 seconds, processes pending orders, updates the menu's total orders, and marks orders as done.
Synchronization: Uses semaphores to ensure exclusive access to shared memory for writes and allow multiple readers, implementing the readers-writers algorithm.

Output: Each process prints actions (e.g., reading menu, placing orders) to stdout with timestamps, process IDs, and indices. Errors are printed to stderr.

Contact

For questions about the project, contact to me)

