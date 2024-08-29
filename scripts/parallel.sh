#!/bin/bash
set -x

rename_output_files() {
    for file in output-*; do
        mv "$file" "output_J${i}_${file#output-}"
    done
}

tasks_per_job=()
esito=()
jobs=()
ids=()


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
    mv a.out Dati_$3/
    echo "Il file 'a.out' è stato spostato nella directory 'Dati_$3'."
fi

cd Dati_$3/

#LANCIO DEI JOB-----------------------------------

for ((i=1; i<=$1; i++)); do
    num_tasks="$2"
    count=0
    while [ $count -eq 0 ]; do
        srun --job-name="${4}_${3}_J${i}" -p parallel -n $num_tasks a.out > srun.log 2>&1 &
        sleep 2

        # Verifica dello stato del job i-esimo
        job_id=$(squeue -u $USER -n "${4}_${3}_J${i}" -o "%i" -h | head -n 1)
        job_status=$(squeue -j $job_id -o "%t" -h)
        job_reason=$(squeue -j $job_id -o "%R" -h)

        # Controlla se il job è in attesa di risorse
        if [[ "$job_status" == "PD" ]]; then
            echo "Il job ${4}_${3}_J${i} non è riuscito a partire poichè in pending..."
            echo "Cancellazione del job..."
            scancel $job_id
            echo "Riduzione del numero di task di 10."
            ((num_tasks -= 10))
            if ((num_tasks < 50)); then
                echo "Il numero di task è inferiore a 100. Cancellazione del job ${4}_${3}_J${i}..."
                scancel $job_id
                esito+=("cancellato a causa di: ${job_reason}")
                tasks_per_job+=(0)
            fi
        else
            echo "Allocate le risorse per il job ${4}_${3}_J${i}. Esecuzione..."
            ((count++))
            job_pid=$!
            wait $job_pid
            rename_output_files
            esito+=("eseguito") 
            tasks_per_job+=($num_tasks)
        fi
    done
    jobs+=("${4}_${3}_J${i}")
    ids+=("${job_id}")
done

# Verifica del numero di tasks eseguiti dai jobs
sum=0
for value in "${tasks_per_job[@]}"; do
    sum=$((sum + value))
done

if [ "$sum" -eq 0 ]; then
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
    ./scripts/notify_errors.sh 250 "[parallel.sh] Superato il limite inferiore di 100 task per tutti i job. Interruzione della simulazione."
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
else
    echo "La somma delle componenti dell'array non è 0. La somma è $sum."
    #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
    ./scripts/notify_ok.sh "J" "$2" "$sum" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}" "${ids[@]}"
    #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
fi


#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------

# # Verifica se esistono file che iniziano con output-*
# if ! ls output-* 1> /dev/null 2>&1; then
#     echo "Nessun file che inizia con 'output-*' trovato. Uscita dallo screen..."
# fi


#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------


screen -X quit