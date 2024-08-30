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
osservabile="${components[1]}"     # Energie
L="${components[2]#L}"            # 12
R="${components[3]#R}"            # 1590
start_time=$3
end_time=$(date +%s)
time_diff=$((end_time - start_time))
hms=$(date -u -d @$time_diff +'%H:%M:%S')

# Assembla il contenuto del body.txt
cat <<EOF > $output_file
--------------------------------------
    [Dettagli della Simulazione]
--------------------------------------
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
# ./scripts/notify_ok.sh "JJ" "$2" "$sum" "${tasks_per_job[@]}" "${esito[@]}" "${jobs[@]}" "${ids[@]}"

output_file="scripts/body.txt"
git_message="Jobs lanciati con successo!"

# vars
# Calcolo del numero massimo di righe tra gli array
max_len=${#tasks_per_job[@]}
if [ ${#esito[@]} -gt $max_len ]; then max_len=${#esito[@]}; fi
if [ ${#jobs[@]} -gt $max_len ]; then max_len=${#jobs[@]}; fi
if [ ${#ids[@]} -gt $max_len ]; then max_len=${#ids[@]}; fi

# Assembla il contenuto del body.txt
cat <<EOF > "$output_file"
--------------------------------------
       [Dettagli Job Lanciati]
--------------------------------------

N° di Task impostati: "${2}"
N° di Task eseguiti:   ${sum}

N° di Job totali: ${#jobs[@]}
N° di Job eseguiti: ${#jobs[@]}


| Job Name | Job ID | Task per Job | Esito   |
EOF

for ((i=0; i<max_len; i++)); do
    job_id=${ids[$i]:-""}
    task_per_job=${tasks_per_job[$i]:-""}
    esito_val=${esito[$i]:-""}
    job_name=${jobs[$i]:-""}
    
    # Stampa della riga formattata
    printf "| %-8s | %-6s | %-12s | %-7s |\n" "$3" "$job_id" "$task_per_job" "$esito_val" >> $output_file
done

# EOF

elif [ "$1" == "J" ]; then

output_file="../scripts/body.txt"
git_message="Job info"

# Assembla il contenuto del body.txt
cat <<EOF > "$output_file"
--------------------------------------
[Info sul Job "${2}"]
--------------------------------------

"${3}"

EOF

fi

# #-------------------GITHUB-------------------------------------#
# echo ""
# echo "Eseguo il commit e il push..."
# echo ""

# # Esegui il push del commit al repository remoto
# # git pull
# git add -A .
# git commit -a -m "IBiSco: ${git_message}"
# git push origin master
# echo "Commit e push completati."
# #-------------------GITHUB-------------------------------------#


