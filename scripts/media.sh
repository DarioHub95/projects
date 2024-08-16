# !/bin/bash
# set -x

# Primo istante di tempo
start_time=$(date +%s)

while screen -ls | grep -q "Sockets"; do
    sleep 1
done

echo "Pulizia dei dati di output eliminando le righe 'step' ed 'srun'..."
for file in "Dati_$6"/output*.txt; do
    sed -i '/^step/d' "$file"
    sed -i '/^srun/d' "$file"
done

# # Conta il numero di file nella cartella e salva le prime 16 righe del primo file di media totale
R_tot=$(ls -1 "Dati_$6" | wc -l)
head -n 16 "Dati_$6"/output1_G1.txt > "Media_R_${R_tot}.txt"

# Conteggio del numero massimo di occorrenze di "-nan" negli output
max_nan_count=0
for file in "Dati_$6"/output*.txt; do
    nan_count=$(grep -c "\-nan" "$file")
    
    if (( nan_count > max_nan_count )); then
        max_nan_count=$nan_count
    fi
done
echo "Il massimo numero di '-nan' è ${max_nan_count:-0}."

echo "Rimuovi ${max_nan_count:-0} righe non sommabili da ogni file di output..."
for file in "Dati_$6"/output*.txt; do
    tail -n +$((max_nan_count + 16 + 1)) "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
done
echo ""

# Calcolo delle medie a 1 colonna (OPERATORE SINGOLO)
if [ "$3" -eq 2 ] || [ "$3" -eq 3 ] || [ "$3" -eq 10 ] || [ "$3" -eq 12 ]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$6"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=2; i++) for(j=1; j<R; j++) $i+=$(i+j*2); printf "%15.10g\t%15.10g\n", $1/R, $2/R}' > "temp_output.txt"
fi

# Calcolo delle medie a 3 colonne (ENERGIE / COMPONENTI SPIN SU L)
if [[ "$3" -eq 4 || "$3" -eq 6 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$6"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=4; i++) for(j=1; j<R; j++) $i+=$(i+j*4); printf "%15.10g\t%15.10g\t%15.10g\t%15.10g\n", $1/R, $2/R, $3/R, $4/R}' > "temp_output.txt"
fi

# # (DA FARE) Calcolo delle medie a 6 colonne (ENERGIE + COMPONENTI SPIN)
# if [[ "$3" -eq 5 || "$3" -eq 6 ]]; then
# echo "Media su tutte le realizzazioni..."
# paste -d+ "Dati_$6"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=3; i++) for(j=1; j<R; j++) $i+=$(i+j*3); printf "\t%.15f\t%.15f\t%.15f\n", $1/R, $2/R, $3/R}' > "temp_output.txt"
# fi

# Calcolo delle medie a 8 colonne (CORRELAZIONE PER L=8)
if [[ "$3" -eq 5 ]]; then
echo "Media su tutte le realizzazioni..."
paste -d+ "Dati_$6"/output*.txt | awk -v R=$R_tot '{for(i=1; i<=9; i++) for(j=1; j<R; j++) $i+=$(i+j*9); printf "%15.10g\t%15.10g\t%15.10g\t%15.10g\t%15.10g\t%15.10g\t%15.10g\t%15.10g\t%15.10g\n", $1/R, $2/R, $3/R, $4/R, $5/R, $6/R, $7/R, $8/R, $9/R}' > "temp_output.txt"
fi

# Inserisci l'output dopo la 16esima riga
{
    head -n 16 "Media_R_${R_tot}.txt"
    cat "temp_output.txt"
} > "Media_R_${R_tot}.txt.tmp"

mv "Media_R_${R_tot}.txt.tmp" "Media_R_${R_tot}.txt"
rm temp_*.txt
echo ""

# # Processa i file di output nella directory
# echo "Sostituzione punti con virgole nel file delle medie..."
# for file in "Media_R_${R_tot}.txt"; do
#     sed -i 's/\./,/g' "$file"
# done

# Secondo istante di tempo e uscita dagli screen
end_time=$(date +%s)
time_diff=$((end_time - start_time))
hms=$(date -u -d @$time_diff +'%H:%M:%S')
echo "$hms" > "Durata_=_${hms}"

screen -X quit
