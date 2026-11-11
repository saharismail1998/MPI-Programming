#!/bin/bash
#SBATCH --job-name=test_slurm
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=00:01:00
#SBATCH --output=test_%j.out

echo "Slurm Job Started"
echo "Hostname: $(hostname)"
echo "Date: $(date)"
echo "Job ID: $SLURM_JOB_ID"
echo "Slurm Job Finished"
