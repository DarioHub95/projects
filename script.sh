#!/bin/bash
#----------------------LOGS--------------------------------------------------------|

exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>log.out 2>&1
# Everything below will go to the file 'log.out':

#----------------------------------------------------------------------------------|

echo ""
GREEN='\033[0;32m'
NC='\033[0m'
screen_script_path="scripts/screen.sh"
media_script_path="scripts/media.sh"

if ls Dati_* 1> /dev/null 2>&1; then
echo -e "${GREEN}[TASK 0 - PULIZIA AMBIENTE]-----------------------------------------------|${NC}"
    echo ""
    echo "Eliminazione directory..."
    echo ""
    rm -rf Dati_* Media_* Durata_*
fi

# Inserire modello
echo -e "${GREEN}[TASK 1 - MODELLO DI PARTICELLE]------------------------------------------|${NC}"
echo ""
echo "Modelli di particelle disponibili:"
echo ""
echo " 1 - PARTICELLE LIBERE"
echo " 2 - PARTICELLE INTERAGENTI"
echo " 3 - MODELLO DI ANDERSON"
echo " 4 - MANY BODY LOCALIZATION (MBL)"
echo " 5 - MBL + BAGNO SULL'ULTIMO SITO"
echo ""
read -p "Inserire numero del corrispondente modello: " P
echo ""
# Usa sed per sostituire il valore di P
sed -i "s/int P = [0-9]*;/int P = $P;/" "main.c"

# Inserire osservabile
echo -e "${GREEN}[TASK 2 - OSSERVABILE]----------------------------------------------------|${NC}"
echo ""
echo "Osservabili disponibili:"
echo ""
echo " 2 - ENEGIA TOTALE E_tot"
# echo " 3 - ENEGIA E_hop"
# echo "10 - ENEGIA E_Int"
# echo "12 - ENEGIA E_mag"
echo " 4 - ENEGIE H_hop, H_int, H_mag"
echo " 5 - AUTOCORRELAZIONE 8 SPIN"
echo " 6 - COMPONENI SPIN SU L"
echo ""
read -p "Inserire numero del corrispondente osservabile: " O
echo ""

# Inserire vars per autocorrelazione
if [ "$O" -eq 5 ]; then
echo -e "${GREEN}[TASK 2.1 - OSSERVABILE AUTOCORRELAZIONE]---------------------------------|${NC}"
echo ""
echo "Osservabile AUTOCORRELAZIONE SPIN-SPIN C(t_w, t)."
echo ""
echo "Lunghezza della catena impostata a: 8 siti"
echo ""
read -p "Inserire tempo di waiting (t_w): " tw
echo ""
fi

echo -e "${GREEN}[TASK 3 - STRUTTURA DATI]-------------------------------------------------|${NC}"
echo ""
# Inserire numero di step MC
read -p "Numero di step Montecarlo (nstep): " nstep
echo ""

# Inserire il valore di R
read -p "Numero di realizzazioni per gruppo (R): " R
echo ""

# Inserire il valore di G (numero di gruppi)
read -p "Numero di gruppi di realizzazioni (G): " G
echo ""

# Compila i file sorgente C
echo -e "${GREEN}[TASK 4 - COMPILAZIONE]----------------------------------------------------|${NC}"
echo ""
echo "Compilazione dei file sorgente..."
g++ -I/lustre/home/adecandia/.lib2/ main.c loop.c kernel.c -L/lustre/home/adecandia/.lib2/ -lpvm -o a.out
echo ""

if [ "$O" -eq 2 ] || [ "$O" -eq 3 ] || [ "$O" -eq 10 ] || [ "$O" -eq 12 ]; then
    var=OSSERV_$O
elif [ "$O" -eq 4 ]; then
    var=ENERGIE
elif [ "$O" -eq 5 ]; then
    var="CORR_tw_${tw}"
elif [ "$O" -eq 6 ]; then
    var=SPIN
fi

# Loop per creare screen e eseguire comandi
echo -e "${GREEN}[TASK 5 - ESECUZIONE SIMULAZIONI]------------------------------------------|${NC}"
echo ""
# Crea una directory per i file di output
mkdir -p "Dati_${var}"
echo "Creata la directory Dati_${var} per i file di output..."
echo ""
for g in $(seq 1 $G); do
    screen_name="G$g"
    echo "Creazione dello screen: $screen_name"
    echo "Esecuzione di $screen_script_path..."
    echo ""
    screen -dmS "$screen_name" bash -c "bash $screen_script_path ${g} ${R} ${O} ${G} ${nstep} ${var} ${tw}; exec bash"
    sleep 1
    if [ "$G" -gt 1 ]; then sleep 9
    fi

done

echo -e "${GREEN}[TASK 6 - SCREEN PER LA MEDIA]---------------------------------------------|${NC}"
echo ""
echo "Creazione dello screen: media"
echo "Esecuzione di $media_script_path..."
echo ""
screen -dmS "media" bash -c "bash $media_script_path ${g} ${R} ${O} ${G} ${nstep} ${var} ${tw}; exec bash"
sleep 2

echo -e "${GREEN}[TASK 7 - SCREEN ATTIVI]---------------------------------------------------|${NC}"
echo ""
screen -ls
echo "Usa il comando 'screen -ls' per visualizzare lo stato degli screen."
echo ""
sed -i "s/int P = [0-9]*;/int P = 0;/" "main.c"