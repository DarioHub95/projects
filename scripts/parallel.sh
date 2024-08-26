#!/bin/bash
set -x

rename_output_files() {
    for file in output-*; do
        mv "$file" "output_J${i}_${file#output-}"
    done
}

# Pulizia dei file output esistenti
if [ -d "Dati_$3" ] && [ "$(ls -A Dati_$3)" ]; then
rm Dati_$3/output*
fi

mv a.out Dati_$3/
cd Dati_$3/

#LANCIO DEI JOB-----------------------------------

for ((i=1; i<=$1; i++)); do
num_tasks="$2"
count=0
    while [ $count -eq 0 ]; do
        srun --job-name="${4}_${3}_J${i}" -p parallel -n $num_tasks a.out > srun.log 2>&1 &
        sleep 1

        # Verifica dello stato del job i-esimo
        job_id=$(squeue -u $USER -n "${4}_${3}_J${i}" -o "%i" -h | head -n 1)
        job_status=$(squeue -j $job_id -o "%R" -h)

        # Controlla se il job è in attesa di risorse
        if [[ "$job_status" == *"Resources"* || "$job_status" == *"Priority"* ]]; then
            echo "Il job ${4}_${3}_J${i} non è riuscito a partire poichè in attesa di risorse."
            echo "Cancellazione del job..."
            scancel $job_id
            echo "Riduzione del numero di task di 10."
            ((num_tasks -= 10))
            if ((num_tasks < 100)); then
                echo "Il numero di task è inferiore a 100. Uscita dallo screen."
                scancel $job_id
                cd ..
                touch "Job_${4}_${3}_J${i}_eliminato"
                rm -rf Dati_$3/
                screen -X quit
            fi
        else
            echo "Allocate le risorse per il job ${4}_${3}_J${i}. Esecuzione..."
            ((count++))
            wait $job_pid
            rename_output_files
        fi
    done
done

screen -X quit