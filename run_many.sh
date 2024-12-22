#!/bin/bash
# Here you should provide the sbatch arguments to be used in all jobs in this serial farm
#SBATCH --account=def-houghten
#SBATCH --time=24:00:00
#SBATCH --mem=512M
#SBATCH --cpus-per-task=1
#SBATCH --output=./Output/cout-%j.out
#SBATCH --mail-user=ko19af@brocku.ca
#SBATCH --mail-type=ALL

# Case number is given by $SLURM_ARRAY_TASK_ID environment variable:
i=$SLURM_ARRAY_TASK_ID

# Extracing the $i-th line from file $TABLE:
LINE=`sed -n ${i}p "$TABLE"`
# Echoing the command (optional), with the case number prepended:
echo "$i; $LINE"
# Executing the command:
eval "$LINE"
