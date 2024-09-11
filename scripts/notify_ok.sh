#!/bin/bash
set -x

echo ""
echo "Generazione del file $output_file per email di notifica..."
echo ""

if [ "$1" == "S" ]; then

#Formattazione
#./scripts/notify_ok.sh "S" "<file>" $start_time $total_tasks

output_file="scripts/body.txt"
git_message="Simulazione eseguita con successo!"

# Estrai i valori dalle parti del nome del file
basename="${2%.txt}"
IFS='_' read -r -a components <<< "$basename"

modello="${components[0]}"        # Ising

if [ ${#components[@]} -eq 6 ]; then
    osservabile="${components[1]}_${components[2]}"  # osservabile è composto da Corr_tw1
    L="${components[3]#L}"        # 8
    R="${components[4]#R}"        # 1590
else
    osservabile="${components[1]}"     # Energie
    L="${components[2]#L}"        # 12
    R="${components[3]#R}"        # 1590
fi

start_time=$3
end_time=$(date +%s)
time_diff=$((end_time - start_time))
hms=$(date -u -d @$time_diff +'%H:%M:%S')

# Assembla il contenuto del body.txt
cat <<EOF > $output_file

[   Dettagli della Simulazione  ]


Start Date: $(date -u -d @$start_time +"%d-%m-%Y %H:%M:%S")
End Date:   $(date -u -d @$end_time +"%d-%m-%Y %H:%M:%S")
Durata:     ${hms}

Modello selezionato: ${modello}
Osservabile scelto:  ${osservabile}

Parametri del Modello:
> L:     ${L}
> nstep: $(grep -oP '(?<=^nstep=)\d+' "${2}")
> Sz:    $(grep -oP '(?<=^Sz=)-?\d+' "${2}")
> Jxy:   1
> Jz:    $(grep -oP '(?<=^Jz=)\d+(\.\d+)?' "${2}")
> eps:   $(grep -oP '(?<=^eps=)\d+(\.\d+)?' "${2}")
> alpha: $(grep -oP '(?<=^alpha=)\d+(\.\d+)?' "${2}")

File delle medie:
${2}

N° di file output eliminati: $(($4 - R))
Questo è il numero di file esclusi dal calcolo della media poichè contenevano almeno il 20% di '-nan'.

EOF

elif [ "$1" == "JJ" ]; then

#Formattazione
# ./scripts/notify_ok.sh "JJ" "$2" "$sum" "${4}_${3}" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}" "${ids[@]}" 

output_file="scripts/body.txt"
git_message="Jobs lanciati con successo!"

# vars
x=$(( ( $# - 4 ) / 4 ))
tasks_per_job=("${@:5:x}") # I prossimi 3 argomenti sono gli elementi di tasks_per_job
esito=("${@:5+x:x}")         # I successivi 3 argomenti sono gli elementi di esito
jobs=("${@:5+x*2:x}")         # I successivi 3 argomenti sono gli elementi di jobs
ids=("${@:5+x*3:x}")          # I successivi 3 argomenti sono gli elementi di ids
basename="${4%.txt}"
IFS='_' read -r -a components <<< "$basename"
modello="${components[0]}"        # Ising
osservabile="${components[1]}"     # Energie
max_len=0

# Itera su tutte le componenti di tutti gli array e trova la lunghezza massima
for element in "${jobs[@]}" "${ids[@]}" "${tasks_per_job[@]}" "${esito[@]}"; do
    length=${#element}
    if [ $length -gt $max_len ]; then
        max_len=$length
    fi
    # Stampa la lunghezza corrente per il debug (opzionale)
    echo "Lunghezza corrente: $length"
done


# Assembla il contenuto del body.txt
cat <<EOF > "$output_file"

[   Jobs per ${4}   ]


Modello selezionato: ${modello}
Osservabile scelto:  ${osservabile}

N° di Task richiesti: $((${2}*${#jobs[@]}))
N° di Task eseguiti:  ${3}

N° di Job richiesti: $((${#jobs[@]} - 1))

Dettagli Job Lanciati:

EOF

for ((i=0; i<x; i++)); do
    job_id=${ids[$i]:-""}
    task_per_job=${tasks_per_job[$i]:-""}
    esito_val=${esito[$i]:-""}
    job_name=${jobs[$i]:-""}
    
    # Stampa della riga formattata
    printf "| %-*s | %-*s | %-*s | %-*s |\n" "$max_len" "$job_name" "$max_len" "$job_id" "$max_len" "$task_per_job" "$max_len" "$esito_val" >> $output_file
done

# EOF

elif [ "$1" == "J" ]; then

#Formattazione
# ./../scripts/notify_ok.sh "J" "${4}_${3}_J${i}" "Job ${4}_${3}_J${i} lanciato alle ore $(date '+%H:%M:%S') con $num_tasks task! "
# ./../scripts/notify_ok.sh "J" "${4}_${3}_J${i}" "Dati acquisiti! Job ${4}_${3}_J${i} completato con $num_tasks task! "

output_file="../scripts/body.txt"
git_message="Job info"

# Assembla il contenuto del body.txt
cat <<EOF > "$output_file"

[   Info sul Job ${2}   ]


${3}

EOF

fi

#-------------------GITHUB-------------------------------------#
echo ""
echo "Eseguo il commit e il push..."
echo ""

# Esegui il push del commit al repository remoto
# git pull
git add -A .
git commit -a -m "LXGRIV11: ${git_message}"
git push origin lxgriv11
echo "Commit e push completati."
#-------------------GITHUB-------------------------------------#


