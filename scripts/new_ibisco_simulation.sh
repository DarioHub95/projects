#!/bin/bash

# Comandi di DEBUG
set -x
# trap 'sleep 3' DEBUG        # Imposta un rallentamento generale di 1 secondo prima di ogni comando

# Check se ci sono altri job e in che stato sono
./scripts/check.sh


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
# cpu_idle=$(sinfo -o "%C" | tail -n 1 | awk -F "/" '{print $2}');
total_tasks=$(($1*$2))
job_name="${4}_${3}"

#----------------------------------------------------------------------------------------------------------------
########################################### ACQUISIZIONE DATI ###################################################
#----------------------------------------------------------------------------------------------------------------

# Pulizia dei file output esistenti
if [ "$(ls Dati_$3 | wc -l)" -gt 2 ]; then
rm Dati_$3/output*
fi

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [ ! -f "Dati_${3}/a.out" ]; then
    ./scripts/notify_errors.sh 110 "[parallel.sh] Il file 'a.out' non esiste. Simulazione interrotta."
    screen -X quit
fi
#-----------------------------------------------------------------

cd Dati_$3/

./../scripts/notify_ok.sh "J" "${job_name}" "Richiesta presa in carico alle ore $(TZ='Europe/Rome' date '+%H:%M:%S'): $1 Job per '${job_name}' con $2 task ciascuno."

count=0
for ((i=1; i<=$1; i++)); do
    num_tasks="$2"
    while :; do

        srun --job-name="${job_name}_J${i}" -p parallel -n $num_tasks a.out > srun.log 2>&1 &
        sleep 1
        while [[ $(squeue -u $USER -n "${job_name}_J${i}" -o "%i" -h | head -n 1) -eq "" ]]; do 
            ((num_tasks -= 1))
            srun --job-name="${job_name}_J${i}" -p parallel -n $num_tasks a.out > srun.log 2>&1 &
            sleep 1
        done   
        sleep 5

        # Acquisizione dei dati del job i-esimo
        job_id=$(squeue -u $USER -n "${job_name}_J${i}" -o "%i" -h | head -n 1)
        # job_status=$(squeue -j $job_id -o "%t" -h)

        # Verifica se lo status è R o PD
        case $(squeue -j $job_id -o "%t" -h) in
            "R")
                #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                if [ "$nstep" -gt 5000 ]; then
                    echo "Il job ${job_name}_J${i} partito!"
                    ./../scripts/notify_ok.sh "J" "${job_name}_J${i}" "Job ${job_name}_J${i} lanciato con $num_tasks task! "
                fi
                #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------

                echo "Allocate le risorse per il job ${job_name}_J${i}. Esecuzione..."
                wait $!
                rename_output_files
                esito+=("Eseguito dopo ${count} tentativi") 
                tasks_per_job+=($num_tasks)
                count=0
                
                #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                if [ "$nstep" -gt 5000 ]; then
                    ./../scripts/notify_ok.sh "J" "${job_name}_J${i}" "Dati acquisiti! Job ${job_name}_J${i} completato con $num_tasks task! "
                fi
                #----------------RICHIAMA LO SCRIPT NOTIFY_OK------------------------------------------
                break  # Esci dal ciclo se il job è stato eseguito
                ;;
            "PD")
                # Verifica se la reason è Resources, Priority o altro
                if [[ "$(squeue -j $job_id -o "%R" -h)" == *"Resources"* ]]; then
                    # Verifica se il job_id è il primo nella lista di priorità
                    if [ "$job_id" == "$(sprio -S '-Y' | awk 'NR==2 {print $1}')" ]; then

                        echo "Il job ${job_name}_J${i} ha priorità massima ma non ha le risorse necessarie."
                        scancel $job_id
                        echo "Riduzione del numero di task di 5 e rilancio del job ${job_name}_J${i}..."
                        ((num_tasks -= 5))

                        if (( $num_tasks < 1 )); then
                            echo "Raggiunto il minimo numero di task (-n 2) per job. Cancellazione del job ${job_name}_J${i}..."
                            scancel $job_id
                            ((i--))
                            ((count++))
                            # esito+=("Cancellato (assenza di risorse)")
                            # tasks_per_job+=(0)
                            break
                        fi
                    else
                        echo "Il Job ${job_name}_J${i} ha una priority troppo bassa. Cancellazione del job ${job_name}_J${i}..."
                        scancel $job_id
                        ((i--))
                        ((count++))
                        break
                    fi

                # Controlla se il job è in priority
                elif [[ "$(squeue -j $job_id -o "%R" -h)" == *"Priority"* || "$(squeue -j $job_id -o "%R" -h)" == *"DOWN, DRAINED"* ]]; then
                    job_reason=$(squeue -j $job_id -o "%R" -h)
                    echo "Il job ${job_name}_J${i} con ID $job_id è in attesa (PD) con reason: $job_reason."
                    time=1800

                    # Verifica se il job_id è il primo nella lista di sprio
                    if [ "$job_id" == "$(sprio -S '-Y' | awk 'NR==2 {print $1}')" ]; then
                        echo "Il job con ID $job_id è il primo nella lista."
                        t=0
                        while (( $t < $time && $(squeue -j $job_id -o "%t" -h) != "R" )); do        # aspetta al massimo 30 min e ogni 1s controlla job_status==R
                            echo "Il job con ID $job_id è il primo nella lista."
                            sleep 1
                            ((t++))
                        done
                        ((time -= 600))
                        if (( $time == 0 )); then
                            echo "Eseguiti i 3 tentativi di attesa. Cancellazione del job ${job_name}_J${i}..."
                            scancel $job_id
                            ((i--))
                            ((count++))
                            # esito+=("Cancellato (attesa eccessiva)")
                            # tasks_per_job+=(0)
                            break
                        fi
                    
                    # Elimina il job_id se ha poca Priority
                    else
                        echo "Il Job ${job_name}_J${i} ha una priority troppo bassa. Cancellazione del job ${job_name}_J${i}..."
                        scancel $job_id
                        ((i--))
                        ((count++))
                        # esito+=("Cancellato (bassa Priority)")
                        # tasks_per_job+=(0)
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

if [ "$sum" -eq 0 ]; then
    #-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
    ./scripts/notify_errors.sh 250 "[parallel.sh] Interruzione della simulazione per $job_name: non sono state allocate risorse per i job. Eliminazione directory per i dati."
    rm -rf "Dati_$3"
    screen -X quit
    #-----------------------------------------------------------------
else
    #----------------RICHIAMA_LO_SCRIPT_NOTIFY_OK------------------------------------------
    echo "La somma delle componenti dell'array non è 0. La somma è $sum."
    ./scripts/notify_ok.sh "JJ" "$2" "$sum" "$job_name" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}" "${ids[@]}"    # $2 ---> input_tasks (R)
    #-----------------------------------------------------------------
fi

#----------------------------------------------------------------------------------------------------------------
############################################### CALCOLO MEDIE ###################################################
#----------------------------------------------------------------------------------------------------------------

#-------------RICHIAMA LO SCRIPT NOTIFY_ERRORS--------------------
if [ "$sum" -ne 0 ] && [ "$(ls Dati_$3 | wc -l)" -eq 2 ]; then       # Se la cartella contiene solo 2 file 
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

# PULIZIA DATI - Controlla se il numero di righe è esatto (operazione delicata)
t_w=grep -oP '(?<=tw=)\d+' $(find "Dati_$3" -maxdepth 1 -type f -name "output*" | head -n 1)
for file in "Dati_$3"/output*; do
    num_righe=$(wc -l < "$file")
    if [ "$num_righe" -eq $((16 + nstep - t_w + 1)) ]; then
        echo "Il numero di righe in $file è corretto: $num_righe"
    else
        echo "Il numero di righe in $file non corrisponde. Trovate: $num_righe"
        echo "Eliminando file: $file"
        rm "$file"
        file_count_lines=$((file_count_lines + 1))
    fi
done

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
MEDIA="${4}_${3}_R${R_tot}_$(TZ='Europe/Rome' date -u -d @$start_time +'%H.%M.%S').txt"
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
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=2; i++) for(j=1; j<R; j++) $i+=$(i+j*2); printf "\t%20.15g\t%20.15g\n", $1/R, $2/R}' > "Medie/temp_output_${3}.txt"
fi

# Calcolo delle medie a 3 colonne (ENERGIE / COMPONENTI SPIN SU L)
if [[ "$Oss" -eq 4 || "$Oss" -eq 6 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=4; i++) for(j=1; j<R; j++) $i+=$(i+j*4); printf "\t%20.15g\t%20.15g\t%20.15g\t%20.15g\n", $1/R, $2/R, $3/R, $4/R}' > "Medie/temp_output_${3}.txt"
fi

# Calcolo delle medie a 8 colonne (CORRELAZIONE PER L=8)
if [[ "$Oss" -eq 5 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$3"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=9; i++) for(j=1; j<R; j++) $i+=$(i+j*9); printf "\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\t%20.15g\n", $1/R, $2/R, $3/R, $4/R, $5/R, $6/R, $7/R, $8/R, $9/R}' > "Medie/temp_output_${3}.txt"
fi

# Inserisci l'output dopo la 16esima riga
{
    head -n 16 "Medie/${MEDIA}"
    cat "Medie/temp_output_${3}.txt"
} > "${MEDIA}.tmp"

mv "${MEDIA}.tmp" "Medie/${MEDIA}"
rm "Medie/temp_output_${3}.txt"

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
