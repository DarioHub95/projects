#!/bin/bash
set -x

#vars
rename_output_files() {
    for file in output-*; do
        mv "$file" "output_J${i}_${file#output-}"
    done
}
tasks_per_job=("Tasks")
esito=("Esito")
jobs=("Job Name")
ids=("Job ID")
nstep=$(grep -oP 'int\s+nstep\s*=\s*\K\d+' main.c)
cpu_idle=$(sinfo -o "%C" | tail -n 1 | awk -F "/" '{print $2}');

# Pulizia dei file output esistenti
if [ "$(ls Dati_$3 | wc -l)" -gt 2 ]; then
rm Dati_$3/output*
fi

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [ ! -f "Dati_${3}/a.out" ]; then
    scancel $job_id
    ./scripts/notify_errors.sh 110 "[parallel.sh] Il file 'a.out' non esiste." 
fi
#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------

cd Dati_$3/
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

        # se la diff è di 450 tasks con le cpu, per una sola volta aspetta 10 min
        if (( $((cpu_idle - num_tasks)) < 400 && $((cpu_idle - num_tasks)) >= 390 && $nstep == 10000 )); then
        echo "Attendo 10 min che il job ${4}_${3}_J${i} parta..."
        sleep 600
        job_status=$(squeue -j $job_id -o "%t" -h)
        #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
            if [[ "$job_status" == "R" ]]; then
            echo "Il job ${4}_${3}_J${i} partito!"
            ./../scripts/notify_ok.sh "J" "${4}_${3}_J${i}" "Job ${4}_${3}_J${i} lanciato con $num_tasks task! "
            fi
        #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
        fi

        # Controlla se il job è in attesa di risorse
        if [[ "$job_status" == "PD" ]]; then
            echo "Il job ${4}_${3}_J${i} non è riuscito a partire poichè in pending..."
            echo "Cancellazione del job..."
            scancel $job_id
            echo "Riduzione del numero di task di 10."
            ((num_tasks -= 10))
            if (( $num_tasks < 50 || $num_tasks < 0 )); then
                echo "Il numero di task è inferiore a 50 o <0. Cancellazione del job ${4}_${3}_J${i}..."
                ((count++))
                scancel $job_id
                esito+=("Cancellato a causa di: ${job_reason}")
                tasks_per_job+=(0)
            fi
        else
            echo "Allocate le risorse per il job ${4}_${3}_J${i} in stato ${job_status}. Esecuzione..."
            job_pid=$!
            wait $job_pid
            rename_output_files
            ((count++))
            esito+=("Eseguito") 
            tasks_per_job+=($num_tasks)
            #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
            if [ "$nstep" -eq 10000 ]; then
                ./../scripts/notify_ok.sh "J" "${4}_${3}_J${i}" "Dati acquisiti! Job ${4}_${3}_J${i} completato con $num_tasks task! "
            fi
            #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
        fi
    done
    jobs+=("${4}_${3}_J${i}")
    ids+=("${job_id}")
done
cd ../

# Verifica del numero di tasks eseguiti dai jobs
sum=0
for value in "${tasks_per_job[@]}"; do
    sum=$((sum + value))
done

if [ "$sum" -eq 0 ]; then
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
    ./scripts/notify_errors.sh 250 "[parallel.sh] Interruzione della simulazione per ${4}_${3}: superato il limite inferiore di 50 task per tutti i job."
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
else
    echo "La somma delle componenti dell'array non è 0. La somma è $sum."
    #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
    ./scripts/notify_ok.sh "JJ" "$2" "$sum" "${4}_${3}" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}" "${ids[@]}"    # $2 ---> input_tasks (R)
    #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
fi

screen -X quit