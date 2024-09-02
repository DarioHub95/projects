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
job_name="${4}_${3}"

# # Pulizia dei file output esistenti
# if [ "$(ls Dati_$3 | wc -l)" -gt 2 ]; then
# rm Dati_$3/output*
# fi



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
        srun --job-name="${job_name}_J${i}" -p parallel -n $num_tasks a.out > srun.log 2>&1 &
        sleep 10

        # Verifica dello stato del job i-esimo
        job_id=$(squeue -u $USER -n "${job_name}_J${i}" -o "%i" -h | head -n 1)
        job_status=$(squeue -j $job_id -o "%t" -h)
        job_reason=$(squeue -j $job_id -o "%R" -h)

        case $job_status in
            "R")
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                # if [ "$nstep" -eq 10000 ]; then
                #     echo "Il job ${job_name}_J${i} partito!"
                #     ./../scripts/notify_ok.sh "J" "${job_name}_J${i}" "Job ${job_name}_J${i} lanciato con $num_tasks task! "
                # fi
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------

                echo "Allocate le risorse per il job ${job_name}_J${i}. Esecuzione..."
                job_pid=$!
                wait $job_pid
                rename_output_files
                esito+=("Eseguito") 
                tasks_per_job+=($num_tasks)
                
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                # if [ "$nstep" -eq 10000 ]; then
                #     ./../scripts/notify_ok.sh "J" "${job_name}_J${i}" "Dati acquisiti! Job ${job_name}_J${i} completato con $num_tasks task! "
                # fi
                # #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                break  # Esci dal ciclo se il job è in esecuzione
                ;;
            "PD")
                # Controlla se il job è in attesa di risorse
                if [[ "$job_reason" == *"Resources"* ]]; then

                    echo "Il job ${job_name}_J${i} non ha le risorse necessarie."
                    scancel $job_id
                    echo "Riduzione del numero di task di 10 e rilancio del job ${job_name}_J${i}..."
                    ((num_tasks -= 10))

                    if (( $num_tasks < 2 )); then
                        echo "Raggiunto il minimo numero di task (-n 2) per job. Cancellazione del job ${job_name}_J${i}..."
                        scancel $job_id
                        esito+=("Cancellato (assenza di risorse)")
                        tasks_per_job+=(0)
                        break
                    fi

                # Controlla se il job non è ancora running ma in priority
                elif ! echo "$(squeue -j $job_id -o "%R" -h)" | grep -q "ibisco"; then 
                    echo "Il job con ID $job_id è ancora in attesa (PD) ma con $job_reason."
                    time=1800

                    # Verifica se il job_id è il primo nella lista di sprio
                    if [ "$job_id" == "$(sprio -S '-Y' | awk 'NR==2 {print $1}')" ]; then
                        echo "Il job con ID $job_id è il primo nella lista."
                        i=0
                        while (( $i < $time && $(squeue -j $job_id -o "%t" -h) != "R" )); do        # aspetta al massimo 30 min e ogni 1s controlla job_status==R
                            echo "Il job con ID $job_id è il primo nella lista."
                            sleep 1
                            ((i++))
                        done
                        ((time -= 600))
                        if (( $time == 0 )); then
                            echo "Eseguiti i 3 tentativi di attesa ma il job non è partito. Cancellazione del job ${job_name}_J${i}..."
                            scancel $job_id
                            esito+=("Cancellato (attesa eccessiva)")
                            tasks_per_job+=(0)
                            break
                        fi

                    # Verifica se il job_id è il secondo nella lista di sprio
                    elif [ "$job_id" == "$(sprio -S '-Y' | awk 'NR==3 {print $1}')" ]; then
                        echo "Il job con ID $job_id NON è il primo nella lista, ma il SECONDO."
                        sprio -S '-Y' --long
                        scancel $job_id
                        echo "Riduzione del numero di task di 10 e rilancio del job ${job_name}_J${i}..."
                        ((num_tasks -= 10))

                        if (( $num_tasks < 2 )); then
                            echo "Raggiunto il minimo numero di task (-n 2) per job. Cancellazione del job ${job_name}_J${i}..."
                            scancel $job_id
                            esito+=("Cancellato (bassa Priority)")
                            tasks_per_job+=(0)
                            break
                        fi
                    
                    # Elimina il job_id se ha poca Priority
                    else
                        echo "Il Job ${job_name}_J${i} ha una priority troppo bassa. Cancellazione del job ${job_name}_J${i}..."
                        scancel $job_id
                        esito+=("Cancellato (bassa Priority)")
                        tasks_per_job+=(0)
                        break
                    fi
                fi
                ;;
        esac
    done
    jobs+=("${job_name}_J${i}")
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