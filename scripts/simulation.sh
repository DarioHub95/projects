#!/bin/bash
set -x

#vars simulazione
# Primo istante di tempo
start_time=$(date +%s)
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
Oss=$(grep -oP '(?<=int P = )\d+' main.c)
L=$(grep -oP '(?<=int L=)\d+' main.c)
cpu_idle=$(sinfo -o "%C" | tail -n 1 | awk -F "/" '{print $2}');
total_tasks=$(($1*$2))

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

        # se la diff è di circa 400 tasks con le cpu, aspetta 15 min
        if (( $((cpu_idle - num_tasks)) < 400 && $((cpu_idle - num_tasks)) >= 390 && $nstep == 10000 )); then
        echo "Attendo 15 min che il job ${4}_${3}_J${i} parta..."
        sleep 900
        job_status=$(squeue -j $job_id -o "%t" -h)
        #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
            if [[ "$job_status" == "R" ]]; then
            echo "Il job ${4}_${3}_J${i} partito!"
            ./../scripts/notify_ok.sh "J" "${4}_${3}_J${i}" "Job ${4}_${3}_J${i} lanciato alle ore $(date '+%H:%M:%S') con $num_tasks task! "
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
                ./../scripts/notify_ok.sh "J" "${4}_${3}_J${i}" "Dati acquisiti! Job ${4}_${3}_J${i} completato alle ore $(date '+%H:%M:%S') con $num_tasks task! "
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
    ./scripts/notify_errors.sh 250 "[parallel.sh] Interruzione della simulazione per ${4}_${3}: superato il limite inferiore di 50 task per tutti i job. Eliminazione directory per i dati."
    rm -rf "Dati_$3"
    # screen -X quit
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
else
    echo "La somma delle componenti dell'array non è 0. La somma è $sum."
    #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
    ./scripts/notify_ok.sh "JJ" "$2" "$sum" "${4}_${3}" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}" "${ids[@]}"    # $2 ---> input_tasks (R)
    #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
fi

#----------------------------------------------------------------------------------------------------------------
                                        #CALCOLO MEDIE
#----------------------------------------------------------------------------------------------------------------

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [ "$sum" -ne 0 ] && [ "$(ls Dati_$3 | wc -l)" -eq 2 ]; then       # Se la cartella contiene solo 2 file 
    ./scripts/notify_errors.sh 100 "[media.sh] Alcuni Job sono stati eseguiti ma la cartella Dati_$3 non contiene i dati di output. Uscita dallo screen media_$3..." 
    screen -X quit
fi
#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------


# Tolleranza al 20% per il numero di -nan nei file di dati
max_nan_count=0
for file in "Dati_$3"/output*.txt; do
    nan_count=$(grep -c "\-nan" "$file")    

    if [ $(echo "scale=2; $nan_count / $nstep > 0.2" | bc) -eq 1 ]; then
        echo "La soglia del 20% è superata. Eliminazione del file $file..."
        rm "$file"
    else
        echo "La soglia non è superata. Nan count: $nan_count"
        if (( nan_count > max_nan_count )); then
            max_nan_count=$nan_count
        fi
    fi
done
echo "Il massimo numero di '-nan' è ${max_nan_count:-0}."

# Conta il numero di file rimasti in Data e salva le prime 16 righe del primo file in media totale
R_tot=$(ls -1 "Dati_$3"/output* 2>/dev/null | wc -l)
MEDIA="${4}_${3}_L${L}_R${R_tot}_$(date -u -d @$start_time +'%H.%M.%S').txt"
output_file=$(find "Dati_$3" -maxdepth 1 -type f -name "output*" | head -n 1)
head -n 16 "$output_file" > "${MEDIA}"

# Rimuovi in ogni file il numero di righe pari al massimo numero di -nan trovati 
echo "Rimuovi ${max_nan_count:-0} righe non sommabili da ogni file di output..."
for file in "Dati_$3"/output*.txt; do
    tail -n +$((max_nan_count + 16 + 1)) "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
done
echo ""

# Verifica se R_tot è maggiore del limite corrente di file aperti
if [[ $R_tot -gt $(ulimit -n) ]]; then
    ulimit -n $((R_tot + 10))  # Aumenta il limite di file aperti di R_tot + 10
    echo "Il limite dei file aperti è stato aumentato a $((R_tot + 10))"
fi

# Calcolo delle medie a 1 colonna (OPERATORE SINGOLO)
if [ "$Oss" -eq 2 ] || [ "$Oss" -eq 3 ] || [ "$Oss" -eq 10 ] || [ "$Oss" -eq 12 ]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=2; i++) for(j=1; j<R; j++) $i+=$(i+j*2); printf "\t%20.15g\t%20.15g\n", $1/R, $2/R}' > "temp_output.txt"
fi

# Calcolo delle medie a 3 colonne (ENERGIE / COMPONENTI SPIN SU L)
if [[ "$Oss" -eq 4 || "$Oss" -eq 6 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=4; i++) for(j=1; j<R; j++) $i+=$(i+j*4); printf "\t%20.15g\t%20.15g\t%20.15g\t%20.15g\n", $1/R, $2/R, $3/R, $4/R}' > "temp_output.txt"
fi

# Calcolo delle medie a 8 colonne (CORRELAZIONE PER L=8)
if [[ "$Oss" -eq 5 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=9; i++) for(j=1; j<R; j++) $i+=$(i+j*9); printf "\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\n", $1/R, $2/R, $3/R, $4/R, $5/R, $6/R, $7/R, $8/R, $9/R}' > "temp_output.txt"
fi

# Inserisci l'output dopo la 16esima riga
{
    head -n 16 "${MEDIA}"
    cat "temp_output.txt"
} > "${MEDIA}.tmp"

mv "${MEDIA}.tmp" "${MEDIA}"
rm temp_*.txt
echo ""

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [ $(wc -l < "${MEDIA}") -le 20 ]; then
    ./scripts/notify_errors.sh 350 "[media.sh] Il file '${MEDIA}' non contiene nessun valore medio. Uscita dallo screen media_$3..." 
    screen -X quit
if [ ! -f "${MEDIA}" ]; then
    ./scripts/notify_errors.sh 200 "[media.sh] Il file '${MEDIA}' non esiste. Uscita dallo screen media_$3..." 
    screen -X quit
fi
#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------

# Inserisci riga di Data e ora e di tasks nel file di media totale
sed -i "1i Tasks: ${R_tot}" "${MEDIA}"
sed -i "1i Date: $(date '+%Y-%m-%d %H:%M:%S')" "${MEDIA}"
sed -i '/seed/d' "${MEDIA}"

#----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
./scripts/notify_ok.sh "S" "${MEDIA}" $start_time $total_tasks
#----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------

# Processa i file di output nella directory
echo "Sostituzione punti con virgole nel file delle medie..."
sed -i 's/\./,/g' "$MEDIA"

# screen -X quit
