#!/bin/bash

output_file="scripts/body.txt"

if [ "$1" != "J" ]; then
echo ""
echo "File ${1} trovato. Generazione del body per email di notifica..."
echo ""

# Estrai i valori dalle parti del nome del file
file=$1
basename="${file%.txt}"
IFS='_' read -r -a components <<< "$basename"

modello="${components[0]}"        # Ising
osservabile="${components[1]}"     # Energie
L="${components[2]#L}"            # 12
R="${components[3]#R}"            # 1590

end_time=$(date +%s)
time_diff=$((end_time - $2))
hms=$(date -u -d @$time_diff +'%H:%M:%S')

# Assembla il contenuto del body.txt
cat <<EOF > $output_file
--------------------------------------
[Dettagli della Simulazione]
--------------------------------------
Start Date: $(date -u -d @$2 +"%d-%m-%Y %H:%M:%S")
End Date:   $(date -u -d @$end_time +"%d-%m-%Y %H:%M:%S")
Durata:     ${hms}

Modello selezionato: ${modello}
Osservabile scelto:  ${osservabile}

Parametri del Modello:
> L:     ${L}
> nstep: $(grep -oP '(?<=^nstep=)\d+' "${1}")
> Sz:    $(grep -oP '(?<=^Sz=)-?\d+' "${1}")
> Jxy:   1
> Jz:    $(grep -oP '(?<=^Jz=)\d+(\.\d+)?' "${1}")
> eps:   $(grep -oP '(?<=^eps=)\d+(\.\d+)?' "${1}")
> alpha: $(grep -oP '(?<=^alpha=)\d+(\.\d+)?' "${1}")

EOF

echo "Il file $output_file è stato generato con successo."
echo ""

#-------------------GITHUB-------------------------------------#

echo "Eseguo il commit e il push..."
echo ""

# Esegui il push del commit al repository remoto
# git pull
git add -A .
git commit -a -m "IBiSco: Simulazione eseguita con successo!"
git push origin master
echo "Commit e push completati."
#-------------------GITHUB-------------------------------------#

else

echo ""
echo "Job allocato. Generazione del body per email di notifica..."
echo ""

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

Task totali: ${2}
Job totali:  ${#jobs[@]}


| Job Name | Job ID | Task per Job | Esito   |
EOF

for ((i=0; i<max_len; i++)); do
    job_id=${ids[$i]:-""}
    task_per_job=${tasks_per_job[$i]:-""}
    esito_val=${esito[$i]:-""}
    job_name=${jobs[$i]:-""}
    
    # Stampa della riga formattata
    printf "| %-8s | %-6s | %-12s | %-7s |\n" "$job_name" "$job_id" "$task_per_job" "$esito_val" >> $output_file
done

# EOF

echo "Il file $output_file è stato generato con successo."
echo ""

#-------------------GITHUB-------------------------------------#

echo "Eseguo il commit e il push..."
echo ""

# Esegui il push del commit al repository remoto
# git pull
git add -A .
git commit -a -m "IBiSco: Jobs lanciati con successo!"
git push origin master
echo "Commit e push completati."
#-------------------GITHUB-------------------------------------#

fi



