# Assignment 3: MPI Programming

Course: Selected Topics in Parallel and Distributed Computing
Author: eng.sahar ismail

This repository contains the MPI source files, Slurm job scripts, result logs and analysis scripts for Assignment 3, together with the PDF report.

## Environment

- Windows with WSL2 (Ubuntu)
- MPI C compiler (`mpicc`) and `mpirun` (Open MPI)
- Slurm, installed locally inside WSL2 (single machine, not a multi-node cluster)
- Python 3 with `pandas` and `matplotlib` (only for the scaling analysis)

Install the Python packages on Ubuntu/WSL with:

```bash
sudo apt update
sudo apt install python3-pandas python3-matplotlib
```

## Directory layout

```
Assignment3_MPI/
├── hello_mpi.c                 Task 1: Hello World
├── pingpong.c                  Task 2: ping-pong (blocking and nonblocking)
├── matmul_mpi.c                Task 3: distributed matrix multiplication
├── deadlock.c                  Task 4: deadlock (MPI_Ssend on both ranks)
├── deadlock_reorder.c          Task 4: fix by reordering send/receive
├── deadlock_sendrecv.c         Task 4: fix with MPI_Sendrecv
├── deadlock_nonblocking.c      Task 4: fix with MPI_Isend / MPI_Irecv / MPI_Waitall
├── slurm/
│   ├── hello_mpi.slurm         Slurm script, 4 tasks
│   ├── pingpong.slurm          Slurm script, 2 tasks
│   └── matmul.slurm            Slurm script for matrix multiplication
├── result/                     Slurm outputs and logs (hello_*, pingpong_*, matmul_*)
├── result_reorder.txt          Output of deadlock_reorder
├── result_sendrecv.txt         Output of deadlock_sendrecv
├── result_nonblocking.txt      Output of deadlock_nonblocking
├── scaling_results.csv         Measured runtimes used for Task 5
├── mpi_scaling_analysis/       Scaling analysis script and generated plots
│   ├── analyze_scaling.py
│   ├── scaling_results.csv
│   └── analysis_output/        speedup, efficiency and communication overhead plots
└── Task5_Report_Section.md     Draft text of the Task 5 report section
```

## Build

Run from the project directory:

```bash
mpicc -o hello_mpi hello_mpi.c
mpicc -o pingpong pingpong.c
mpicc -o matmul_mpi matmul_mpi.c -lm
mpicc -o deadlock deadlock.c
mpicc -o deadlock_reorder deadlock_reorder.c
mpicc -o deadlock_sendrecv deadlock_sendrecv.c
mpicc -o deadlock_nonblocking deadlock_nonblocking.c
```

## Task 1: Hello World

```bash
mpirun -np 4 ./hello_mpi
sbatch slurm/hello_mpi.slurm      # requests 4 tasks; output goes to result/hello_<jobid>.out
```

Expected output: one line `Hello from rank r of 4` per rank (order may vary).

## Task 2: Ping-pong

The program needs exactly 2 processes. It measures 1 KB, 64 KB, 1 MB and 4 MB messages, 100 iterations each, first with `MPI_Send`/`MPI_Recv` and then with `MPI_Isend`/`MPI_Irecv`.

```bash
mpirun -np 2 ./pingpong
sbatch slurm/pingpong.slurm       # output: result/pingpong_<jobid>.out
```

Output format: message size, average round-trip time (seconds) and effective bandwidth `size / (RTT/2)` in MB/s (1 MB = 1,048,576 bytes).

## Task 3: Distributed matrix multiplication

N = 2048, row-block decomposition (`MPI_Bcast` for B, `MPI_Scatter` for A, `MPI_Gather` for C). N must be divisible by the number of processes. The program verifies the result against a serial reference and prints runtime, compute time, communication time and `Verification: PASSED/FAILED`.

```bash
mpirun -np 4 ./matmul_mpi
```

With Slurm, set the number of processes with `--ntasks` and submit:

```bash
sbatch --ntasks=1 slurm/matmul.slurm
sbatch --ntasks=2 slurm/matmul.slurm
sbatch --ntasks=4 slurm/matmul.slurm
sbatch --ntasks=8 slurm/matmul.slurm
```

Note: `slurm/matmul.slurm` and `slurm/pingpong.slurm` contain `cd /mnt/d/Assignment3_MPI`. Change this path to the location of the project on your machine. The `result/` directory must exist before submitting jobs.

## Task 4: Deadlock

`deadlock.c` uses `MPI_Ssend` on both ranks before either posts a receive, so both ranks wait for each other and the program hangs. Run it with a timeout:

```bash
timeout 10 mpirun -np 2 ./deadlock
```

The three fixed versions complete and print `Rank <r> received: 100` for both ranks:

```bash
mpirun -np 2 ./deadlock_reorder
mpirun -np 2 ./deadlock_sendrecv
mpirun -np 2 ./deadlock_nonblocking
```

## Task 5: Scaling analysis

`scaling_results.csv` holds the measured runtimes for 1, 2, 4 and 8 processes. The script recomputes speedup `S(p) = T(1)/T(p)`, efficiency `E(p) = S(p)/p` and communication overhead `T_comm/T_total * 100`, and generates the plots:

```bash
cd mpi_scaling_analysis
python3 analyze_scaling.py scaling_results.csv --outdir analysis_output
```

Outputs in `analysis_output/`: `scaling_analysis.csv`, `speedup_vs_processes.png`, `efficiency_vs_processes.png`, `communication_overhead.png`.

## Summary of results (N = 2048)

| Processes | Runtime (s) | Speedup | Efficiency | Communication overhead |
| --- | --- | --- | --- | --- |
| 1 | 4.430283 | 1.0000 | 100.00% | 0.505% |
| 2 | 4.049786 | 1.0940 | 54.70% | 1.143% |
| 4 | 3.801076 | 1.1655 | 29.14% | 1.509% |
| 8 | 4.079504 | 1.0860 | 13.57% | 1.744% |

All configurations passed verification against the serial reference.

## Limitations

- The 16-process measurement requested in Task 5 was not obtained: the Slurm job stayed pending and was never executed. No value is reported for it.
- All experiments ran on one local WSL2 machine, so the results describe that environment and not a multi-node cluster.
- Runtimes vary between runs by roughly 10%, which is comparable to the measured speedups.
