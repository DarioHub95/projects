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
# job_name="${4}_${3}_J${i}"
job_name="test"


# # Pulizia dei file output esistenti
# if [ "$(ls Dati_$3 | wc -l)" -gt 2 ]; then
# rm scripts/output*
# fi
rm scripts/output*



# #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
# if [ ! -f "Dati_${3}/a.out" ]; then
#     scancel $job_id
#     ./scripts/notify_errors.sh 110 "[parallel.sh] Il file 'a.out' non esiste." 
# fi
# #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------

cd scripts/
for ((i=1; i<=$1; i++)); do
    num_tasks="$2"
    count=0
    while :; do
        srun --job-name="$job_name" -p parallel -n $num_tasks a.out > srun.log 2>&1 &
        if (( $count == 0 )); then sleep 60; ((count++)); else sleep 5; fi

        # Verifica dello stato del job i-esimo
        job_id=$(squeue -u $USER -n "$job_name" -o "%i" -h | head -n 1)
        job_status=$(squeue -j $job_id -o "%t" -h)
        job_reason=$(squeue -j $job_id -o "%R" -h)


        case $job_status in
            "R")
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                # if [ "$nstep" -eq 10000 ]; then
                #     echo "Il job $job_name partito!"
                #     ./../scripts/notify_ok.sh "J" "$job_name" "Job $job_name lanciato con $num_tasks task! "
                # fi
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------

                echo "Allocate le risorse per il job $job_name. Esecuzione..."
                job_pid=$!
                wait $job_pid
                rename_output_files
                esito+=("Eseguito") 
                tasks_per_job+=($num_tasks)
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                # if [ "$nstep" -eq 10000 ]; then
                #     ./../scripts/notify_ok.sh "J" "$job_name" "Dati acquisiti! Job $job_name completato con $num_tasks task! "
                # fi
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                break  # Esci dal ciclo se il job è in esecuzione
                ;;
            "PD")
                echo "Il job con ID $job_id è ancora in attesa (PD)."
                if (( elapsed >= timeout1 )); then
                    echo "Sono trascorsi 20 minuti e il job è ancora in attesa. Passo alla fase di verifica."
                    job_status="CHECK"  # Cambia lo stato per eseguire un'azione specifica
                fi
                ;;
        esac
















        else


        fi
    done
    jobs+=("$job_name")
    ids+=("${job_id}")
done
cd ../

# Verifica del numero di tasks eseguiti dai jobs
sum=0
for value in "${tasks_per_job[@]}"; do
    sum=$((sum + value))
done

# if [ "$sum" -eq 0 ]; then
#     #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
#     ./scripts/notify_errors.sh 250 "[parallel.sh] Interruzione della simulazione per ${4}_${3}: superato il limite inferiore di 50 task per tutti i job."
#     #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
# else
#     echo "La somma delle componenti dell'array non è 0. La somma è $sum."
#     #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
#     ./scripts/notify_ok.sh "JJ" "$2" "$sum" "${4}_${3}" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}" "${ids[@]}"    # $2 ---> input_tasks (R)
#     #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
# fi

echo "La somma delle componenti dell'array non è 0. La somma è $sum."


set +x
# screen -X quit