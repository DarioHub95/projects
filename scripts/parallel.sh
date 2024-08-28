#!/bin/bash
set -x

rename_output_files() {
    for file in output-*; do
        mv "$file" "output_J${i}_${file#output-}"
    done
}

# Pulizia dei file output esistenti
if [ "$(ls Dati_$3 | wc -l)" -gt 2 ]; then
rm Dati_$3/output*
fi

if [ ! -f "a.out" ]; then
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
    scancel $job_id
    ./scripts/notify_errors.sh 110 "[parallel.sh] Il file 'a.out' non esiste." 
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
else
    # Se il file a.out esiste
    mv a.out Dati_$3/
    echo "Il file 'a.out' è stato spostato nella directory 'Dati_$3'."
fi

mv a.out Dati_$3/ #110
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
        job_status=$(squeue -j $job_id -o "%t" -h)
        job_reason=$(squeue -j $job_id -o "%R" -h)

        # Controlla se il job è in attesa di risorse
        if [[ "$job_status" == *"PD"* || "$job_status" == *"Priority"* ]]; then
            echo "Il job ${4}_${3}_J${i} non è riuscito a partire poichè in pending..."
            echo "Cancellazione del job..."
            scancel $job_id
            echo "Riduzione del numero di task di 10."
            ((num_tasks -= 10))
            if ((num_tasks < 100)); then
                #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
                scancel $job_id
                ./scripts/notify_errors.sh 250 "[parallel.sh] Il numero di task è inferiore a 100. Cancellazione del job ${4}_${3}_J${i} a causa di ${job_reason}" 
                #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
            fi
        else
            echo "Allocate le risorse per il job ${4}_${3}_J${i}. Esecuzione..."
            ((count++))
            job_id=$!
            wait $job_id
            rename_output_files
        fi
    done
done

# # Verifica se esistono file che iniziano con output-*
# if ! ls output-* 1> /dev/null 2>&1; then
#     echo "Nessun file che inizia con 'output-*' trovato. Uscita dallo screen..."
#     screen -X quit
# fi

# #USCITA DI EMERGENZA-----------------------------------------------------------------------

# job_names=$(squeue -u $USER -o "%j" | tail -n +2)

# # Trova il job associato ai file, puoi adattare questo comando per ottenere l'ID del job
# job_id=$(squeue -u $USER -n "${job_names}" -o "%i" -h | head -n 1)

# # Controlla se il job è ancora in esecuzione e verifica l'ultima modifica del file
# if [ "$(squeue -j "$job_id" -o "%t" -h)" = "R" ]; then
#     echo "Il job $job_id è ancora in esecuzione."

#     # Controlla se i file sono stati modificati negli ultimi 5 secondi
#     if [ "$(find output-* -type f -exec stat -c %Y {} + | awk -v threshold=$(date +%s) -v sec=5 '{
#         if (threshold - $1 >= sec) exit 1
#     }' || echo "1")" = "1" ]; then
#         echo "Il file non è stato modificato negli ultimi 5 secondi."
#         echo "Cancellazione del job $job_id..."
#         scancel "$job_id"
#     else
#         echo "Il file è stato modificato recentemente. Il job non verrà cancellato."
#     fi
# else
#     echo "Il job $job_id non è in esecuzione o non esiste."
# fi




screen -X quit