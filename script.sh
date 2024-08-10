#!/bin/bash
echo ""

# # Compila i file sorgente C
# echo "Compilazione dei file sorgente..."
# g++ -I/lustre/home/adecandia/.lib2/ main.c loop.c kernel.c -L/lustre/home/adecandia/.lib2/ -lpvm -o a.out
# echo ""

# Definisci la variabile R
R=2

# Crea una directory per i file di output
output_dir="R_${R}"
mkdir -p $output_dir
echo "Creata la directory $output_dir per i file di output con samples Monte Carlo..."
echo ""

# # Esegui il programma R volte, salvando l'output in file separati
# echo "Esecuzione ogni 30s delle $R realizzazioni ..."
# for numero in $(seq 1 $R); do
#     echo "Esecuzione realizzazione $numero..."
#     srun --propagate=NONE -p sequential ./a.out &> "${output_dir}/output${numero}.txt" &
#     sleep 30
# done

# echo ""

# # Attendi che tutti i processi in background siano completati
# wait
# echo "Tutti i jobs sono stati completati."
# echo ""

# # Processa i file di output nella directory
# echo "Pulizia dei file di output eliminando le righe 'step'..."
# for file in "${output_dir}"/output*.txt; do
#     sed -i '/^step/d' "$file"
# done

# # Salva le prime 10 righe del primo file di output in mediaR$R.txt
# head -n 17 "${output_dir}/output1.txt" > "mediaR${R}.txt"

# # Rimuovi le prime 10 righe da ogni file di output
# for file in "${output_dir}"/output*.txt; do
#     tail -n +18 "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
# done
# echo ""

#-----------------------------------------------------


# Crea una variabile per accumulare la somma
total_sum=0

# Loop attraverso ciascun file
for file in "${output_dir}"/output*.txt; do
    # Estrai la prima riga del file e salvala in una variabile
    first_line=$(head -n 1 "$file")
    
    # Somma i valori nella prima riga
    # Supponiamo che i valori siano separati da spazi o tabulazioni
    sum=$(echo "$first_line" | awk '{s=0; for(i=1; i<=NF; i++) s+=$i; print s}')
    
    # Aggiungi il risultato alla somma totale
    total_sum=$(echo "$total_sum + $sum" | bc)
done

# Mostra la somma totale
echo "La somma totale delle prime righe è: $total_sum"



#----------------------------------------------------



# # Inizializza array per memorizzare la somma delle colonne e il conteggio dei valori validi
# declare -a sums
# declare -a counts
# declare -i line_count=0

# # Inizializza gli array sums e counts con zeri
# for file in "${output_dir}"/output*.txt; do
#     awk '{if ($1 != "-nan") sums[NR] += $1; if ($1 != "-nan") counts[NR] += 1} END {for (i = 1; i <= NR; i++) print sums[i], counts[i]}' "$file" |
#     while read -r sum count; do
#         sums[$line_count]=$(echo "${sums[$line_count]:-0} + $sum" | bc -l)
#         counts[$line_count]=$(( ${counts[$line_count]:-0} + ${count:-0} ))
#         ((line_count++))
#     done
# done
# echo "Somma dei valori delle righe completata."

# echo ""


# # Inserisci una riga vuota dopo le prime 17 righe
# sed -i '17a\' "mediaR${R}.txt"

# # Aggiungi somme e conteggi al file, partendo dalla riga 18
# for ((i=0; i<line_count; i++)); do
#     echo "${sums[$i]}" >> "mediaR${R}.txt"
#     echo "${counts[$i]}" >> "mediaR${R}.txt"
# done




# # Calcola la media per ogni riga, escludendo le righe con -nan, e salva in mediaR${R}.txt
# echo "Calcolo delle medie per riga ed esclusione dei valori -nan..."
# for ((i=0; i<line_count; i++)); do
#     if [ ${counts[$i]} -gt 0 ]; then
#         average=$(echo "scale=6; ${sums[$i]} / ${counts[$i]}" | bc -l | sed 's/\./,/')
#     else
#         average="nan"
#     fi
#     echo "$i, $average" >> "${output_dir}/mediaR${R}.txt"
# done
# echo "Calcolo delle medie completato. Risultati salvati in ${output_dir}/mediaR${R}.txt."


# # Processa i file di output nella directory
# echo "Sostituzione punti -> virgole nel file delle medie..."
# for file in mediaR${R}.txt; do
#     sed -i 's/\./,/g' "$file"
# done
# echo ""