#!/bin/bash

# Comandi di DEBUG
set -x
# trap 'sleep 3' DEBUG        # Imposta un rallentamento generale di 1 secondo prima di ogni comando

#vars simulazione
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
Oss=$(grep -oP 'int\s+Oss\s*=\s*\K\d+' main.c)
L=$(grep -oP '(?<=int L=)\d+' main.c)
num_tasks="$2"
total_tasks=$(($1*$2))
job_name="${4}_${3}"

#----------------------------------------------------------------------------------------------------------------
########################################### ACQUISIZIONE DATI ###################################################
#----------------------------------------------------------------------------------------------------------------

# # Pulizia dei file output esistenti
# if [ "$(ls Dati_$3 | wc -l)" -gt 2 ]; then
# rm Dati_$3/output*
# fi

# #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
# if [ ! -f "Dati_${3}/a.out" ]; then
#     ./scripts/notify_errors.sh 110 "[parallel.sh] Il file 'a.out' non esiste. Simulazione interrotta."
#     screen -X quit
# fi
# #-----------------------------------------------------------------

# cd Dati_$3/

# #----------------RICHIAMA_LO_SCRIPT_NOTIFY_OK---------------------
# ./../scripts/notify_ok.sh "J" "${job_name}" "Richiesta presa in carico alle ore $(TZ='Europe/Rome' date '+%H:%M:%S'): $1 Job per '${job_name}' con $num_tasks task ciascuno."
# #-----------------------------------------------------------------

# for ((i=1; i<=$1; i++)); do
#     mpirun -np $num_tasks ./a.out > mpirun.log 2>&1 &
#     sleep 1

#     echo "Allocate le risorse per il job ${job_name}_J${i}. Esecuzione..."
#     wait $!
#     rename_output_files
#     esito+=("Eseguito") 
#     tasks_per_job+=($num_tasks)
#     #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
#     ./../scripts/notify_ok.sh "J" "${job_name}_J${i}" "Dati acquisiti! Job ${job_name}_J${i} completato alle ore $(TZ='Europe/Rome' date '+%H:%M:%S') con $num_tasks task! "
#     #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
#     jobs+=("${job_name}_J${i}")
# done
# cd ../

# #----------------RICHIAMA_LO_SCRIPT_NOTIFY_OK------------------------------------------
# ./scripts/notify_ok.sh "JJ" "$2" "$sum" "$job_name" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}"    # $2 ---> input_tasks (R)
# #-----------------------------------------------------------------

#----------------------------------------------------------------------------------------------------------------
############################################### CALCOLO MEDIE ###################################################
#----------------------------------------------------------------------------------------------------------------

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [ "$(ls Dati_$3 | wc -l)" -eq 2 ]; then       # Se la cartella contiene solo 2 file 
    ./scripts/notify_errors.sh 100 "[media.sh] I Job sono stati eseguiti ma la cartella Dati_$3 non contiene i dati di output. Uscita dallo screen ${4}_${3}..." 
    screen -X quit
fi
#-----------------------------------------------------------------

# PULIZIA DATI - Tolleranza al 5% per il numero di -nan nei file di dati
max_nan_count=0
file_count_nan=0
file_count_lines=0
for file in "Dati_$3"/output*.txt; do
    nan_count=$(grep -c "\-nan" "$file")    

    if [ $(echo "scale=2; $nan_count / $nstep > 0.04" | bc) -eq 1 ]; then
        echo "La soglia del 4% è superata. Eliminazione del file $file..."
        rm "$file"
        file_count_nan=$((file_count_nan + 1))
    else
        echo "La soglia non è superata. Nan count: $nan_count"
        if (( nan_count > max_nan_count )); then
            max_nan_count=$nan_count
        fi
    fi
done
echo "Il massimo numero di '-nan' è ${max_nan_count:-0}."

# PULIZIA DATI - Se tutte le colonne di dati contengono solo zeri, elimina il file
for file in "Dati_$3"/output*; do
    if awk '{for (i=2; i<=NF; i++) if ($i != 0) exit 1}' "$file"; then
        rm "$file"
        echo "File $file eliminato perché tutte le colonne contengono solo zeri."
        file_count_lines=$((file_count_lines + 1))
    fi
done

# PULIZIA DATI - Controlla se il primo numero dell'ultima riga è diverso da nstep
for file in "Dati_$3"/output*; do
    last_line=$(tail -n 1 "$file")
    first_number=$(echo "$last_line" | awk '{print $1}')
    if [[ "$first_number" != $nstep ]]; then
        echo "Eliminando file: $file"
        rm "$file"
        file_count_lines=$((file_count_lines + 1))
    fi
done
echo "Il numero di file con righe sbagliate è $file_count_lines"

# Conta il numero di file rimasti in Data
R_tot=$(ls -1 "Dati_$3"/output* 2>/dev/null | wc -l)

# PULIZIA DATI - Verifica se R_tot è maggiore del limite corrente di file aperti (meno 6 file che non capisco quali siano)
if [[ $R_tot -lt 4090 ]]; then
    ulimit -n 4096
    echo "Il limite dei file aperti è stato impostato al massimo"
    ulimit -n
else
    files_to_move=$((R_tot - 4090))
    echo "R_tot supera 4096. Spostamento di $files_to_move file .txt dalla cartella Dati..."
    mkdir -p "Dati_${3}_extra"
    ls -tp "Dati_$3"/*.txt | tail -n "$files_to_move" | xargs -I {} mv -- "{}" "Dati_${3}_extra/"
    # ls -tp "Dati_$3"/*.txt | tail -n "$files_to_move" | xargs -I {} rm -- "{}"
    ulimit -n 4096
    echo "Il limite dei file aperti è stato impostato al massimo"
fi
R_tot=$(ls -1 "Dati_$3"/output* 2>/dev/null | wc -l)

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [[ $file_count_nan != 0 || $file_count_lines != 0 || $files_to_move != 0 ]]; then       
    ./scripts/notify_errors.sh 550 "N° di file con eccesso di '-nan': $file_count_nan" "N° di file incompleti: $file_count_lines" "N° di file che superano ulimit: ${files_to_move:-0}" "N° di file conformi: $R_tot"
fi
#-----------------------------------------------------------------

# Salva le prime 16 righe del primo file in media totale
MEDIA="${4}_${3}_L${L}_R${R_tot}_$(TZ='Europe/Rome' date -u -d @$start_time +'%H.%M.%S').txt"
output_file=$(find "Dati_$3" -maxdepth 1 -type f -name "output*" | head -n 1)
head -n 16 "$output_file" > "Medie/${MEDIA}"

# Rimuovi in ogni file il numero di righe pari al massimo numero di -nan trovati 
echo "Rimuovi ${max_nan_count:-0} righe non sommabili da ogni file di output..."
for file in "Dati_$3"/output*.txt; do
    if head -n 1 "$file" | grep -q "seed"; then
        tail -n +$((max_nan_count + 16 + 1)) "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
    fi
done
echo ""

# Calcolo delle medie a 1 colonna (OPERATORE SINGOLO)
if [ "$Oss" -eq 2 ] || [ "$Oss" -eq 3 ] || [ "$Oss" -eq 10 ] || [ "$Oss" -eq 12 ]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=2; i++) for(j=1; j<R; j++) $i+=$(i+j*2); printf "\t%20.15g\t%20.15g\n", $1/R, $2/R}' > "Medie/temp_output.txt"
fi

# Calcolo delle medie a 3 colonne (ENERGIE / COMPONENTI SPIN SU L)
if [[ "$Oss" -eq 4 || "$Oss" -eq 6 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=4; i++) for(j=1; j<R; j++) $i+=$(i+j*4); printf "\t%20.15g\t%20.15g\t%20.15g\t%20.15g\n", $1/R, $2/R, $3/R, $4/R}' > "Medie/temp_output.txt"
fi

# Calcolo delle medie a 8 colonne (CORRELAZIONE PER L=8)
if [[ "$Oss" -eq 5 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=9; i++) for(j=1; j<R; j++) $i+=$(i+j*9); printf "\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\n", $1/R, $2/R, $3/R, $4/R, $5/R, $6/R, $7/R, $8/R, $9/R}' > "Medie/temp_output.txt"
fi

# Inserisci l'output dopo la 16esima riga
{
    head -n 16 "Medie/${MEDIA}"
    cat "Medie/temp_output.txt"
} > "${MEDIA}.tmp"

mv "${MEDIA}.tmp" "Medie/${MEDIA}"
rm "Medie/temp_output.txt"

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [ $(wc -l < "Medie/${MEDIA}") -le 20 ]; then
    ./scripts/notify_errors.sh 350 "[media.sh] Il file '${MEDIA}' non contiene nessun valore medio. Uscita dallo screen ${4}_${3}..." 
    screen -X quit
elif [ ! -f "Medie/${MEDIA}" ]; then
    ./scripts/notify_errors.sh 200 "[media.sh] Il file '${MEDIA}' non esiste. Uscita dallo screen ${4}_${3}..." 
    screen -X quit
fi
#-----------------------------------------------------------------

# Inserisci riga di Data e ora e di tasks nel file di media totale
sed -i "1i Tasks: ${R_tot}" "Medie/${MEDIA}"
sed -i "1i Date: $(TZ='Europe/Rome' date '+%Y-%m-%d %H:%M:%S')" "Medie/${MEDIA}"
sed -i '/seed/d' "Medie/${MEDIA}"

#----------------RICHIAMA_LO_SCRIPT_NOTIFY_OK------------------------------------------
./scripts/notify_ok.sh "S" "${MEDIA}" $start_time $total_tasks $sum
#-----------------------------------------------------------------

# Processa i file di output nella directory
echo "Sostituzione punti con virgole nel file delle medie..."
sed -i 's/\./,/g' "Medie/${MEDIA}"

screen -X quit
