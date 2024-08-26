# !/bin/bash

# Esegui il programma R volte, salvando l'output in file separati
echo "Esecuzione ogni 30s delle ${2} run ..."
for i in $(seq 1 $2); do
    echo "Esecuzione run $i..."
    srun --propagate=NONE -p sequential ./a.out &> "Dati_$3/output${i}_G${1}.txt" &
    sleep 30
done

# Attendi che tutti i processi in background siano completati
wait
echo "Jobs completati!"
echo ""
screen -X quit
