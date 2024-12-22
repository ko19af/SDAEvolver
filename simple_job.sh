#!/bin/bash
#SBATCH --time=6:00:00
#SBATCH --account=def-houghten
#SBATCH --mem=512M
#SBATCH --cpus-per-task=1
#SBATCH --output=./Output/cout-%j.out
#SBATCH --mail-user=ko19af@brocku.ca
#SBATCH --mail-type=ALL
./a.out 6 2 50 3 1 10000 1 10 1 10 30 0 
