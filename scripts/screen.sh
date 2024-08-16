# !/bin/bash

# Esegui il programma R volte, salvando l'output in file separati
echo "Esecuzione ogni 30s delle ${2} realizzazioni ..."
for numero in $(seq 1 $2); do
    echo "Esecuzione realizzazione $numero..."
    srun --propagate=NONE -p sequential ./a.out $3 $5 $7 &> "Dati_$6/output${numero}_G${1}.txt" &
    sleep 30
done

# Attendi che tutti i processi in background siano completati
wait
echo "Jobs completati!"
echo ""
screen -X quit
