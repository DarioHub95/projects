# !/bin/bash
echo ""

# Esegui il programma R volte, salvando l'output in file separati
echo "Esecuzione ogni 25s delle ${2} realizzazioni ..."
for numero in $(seq 1 $2); do
    echo "Esecuzione realizzazione $numero..."
    srun --propagate=NONE -p sequential ./a.out $3 $5 $8 &> "Dati_$7/output${numero}_G${1}.txt" &
    sleep 30
done

# Attendi che tutti i processi in background siano completati
wait
echo "Jobs completati!"
echo ""

#--------------------------------------------------------------------------
# if [ $1 -eq $4 ]; then
# echo "Pulizia dei dati di output eliminando le righe 'step'..."
# for file in "Dati_$7"/output*.txt; do
#     sed -i '/^step/d' "$file"
#     sed -i '/^srun/d' "$file"
# done


# # Salva le prime 16 righe del primo file di media totale
# num1="$2"
# num2="$4"
# R_tot=$((num1*num2))
# head -n 16 "Dati_$7/output1_G1.txt" > "Media_R_${R_tot}.txt"

# # Conteggio del numero massimo di occorrenze di "-nan" negli output
# max_nan_count=0
# for file in "Dati_$7"/output*.txt; do
#     nan_count=$(grep -c "\-nan" "$file")
    
#     if (( nan_count > max_nan_count )); then
#         max_nan_count=$nan_count
#     fi
# done
# echo "Il massimo numero di '-nan' è ${max_nan_count:-0}."

# sleep 15

# echo "Rimuovi righe non sommabili da ogni file di output..."
# for file in "Dati_$7"/output*.txt; do
#     tail -n +$((max_nan_count + 16 + 1)) "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
# done
# echo ""

# fi

# # Calcolo delle medie a 1 colonna (OPERATORE SINGOLO)
# if [ "$3" -eq 2 ] || [ "$3" -eq 3 ] || [ "$3" -eq 4 ] || [ "$3" -eq 10 ] || [ "$3" -eq 12 ]; then
# echo "Media su tutte le realizzazioni..."
# paste -d+ "Dati_$7"/output*.txt | bc | awk -v R=$2 '{printf "\t%.15f\n", $1/R}' > "temp_output1.txt"
# awk '{print NR, $0}' "temp_output1.txt" > "temp_output.txt"
# sed -i 's/^/\t/' "temp_output.txt"
# fi

# # Calcolo delle medie a 3 colonne (ENERGIE / COMPONENTI SPIN)
# if [[ "$3" -eq 5 || "$3" -eq 6 ]]; then
# echo "Media su tutte le realizzazioni..."
# paste -d+ "Dati_$7"/output*.txt | awk -v R=$2 '{for(i=1; i<=3; i++) for(j=1; j<R; j++) $i+=$(i+j*3); printf "\t%.15f\t%.15f\t%.15f\n", $1/R, $2/R, $3/R}' > "temp_output1.txt"
# awk '{print NR, $0}' "temp_output1.txt" > "temp_output.txt"
# sed -i 's/^/\t/' "temp_output.txt"
# fi

# # # DA FARE Calcolo delle medie a 6 colonne (ENERGIE + COMPONENTI SPIN)
# # if [[ "$3" -eq 5 || "$3" -eq 6 ]]; then
# # echo "Media su tutte le realizzazioni..."
# # paste -d+ "Dati_$7"/output*.txt | awk -v R=$2 '{for(i=1; i<=3; i++) for(j=1; j<R; j++) $i+=$(i+j*3); printf "\t%.15f\t%.15f\t%.15f\n", $1/R, $2/R, $3/R}' > "temp_output1.txt"
# # awk '{print NR, $0}' "temp_output1.txt" > "temp_output.txt"
# # sed -i 's/^/\t/' "temp_output.txt"
# # fi

# # # DA FARE Calcolo delle medie a 8 colonne (CORRELAZIONE PER L=8)
# # if [[ "$3" -eq 5 || "$3" -eq 6 ]]; then
# # echo "Media su tutte le realizzazioni..."
# # paste -d+ "Dati_$7"/output*.txt | awk -v R=$2 '{for(i=1; i<=3; i++) for(j=1; j<R; j++) $i+=$(i+j*3); printf "\t%.15f\t%.15f\t%.15f\n", $1/R, $2/R, $3/R}' > "temp_output1.txt"
# # awk '{print NR, $0}' "temp_output1.txt" > "temp_output.txt"
# # sed -i 's/^/\t/' "temp_output.txt"
# # fi


# # Inserisci l'output dopo la 15esima riga
# {
#     head -n 15 "Media_R_${R_tot}.txt"
#     cat "temp_output.txt"
# } > "Media_R_${R_tot}.txt.tmp"

# mv "Media_R_${R_tot}.txt.tmp" "Media_R_${R_tot}.txt"
# rm temp_*.txt
# echo ""

# # Processa i file di output nella directory
# echo "Sostituzione punti con virgole nel file delle medie..."
# for file in "Media_R_${R_tot}.txt"; do
#     sed -i 's/\./,/g' "$file"
# done

# # Secondo istante di tempo e uscita dagli screen
# if [ -e "Media_R_${R_tot}.txt" ]; then
#     wait
#     end_time=$(date +%s)
#     time_diff=$((end_time - $6))
#     hms=$(date -u -d @$time_diff +'%H:%M:%S')
#     echo "$hms" > "Durata_=_${hms}"
# fi

screen -X quit

