#!/bin/bash

echo ""
GREEN='\033[0;32m'
NC='\033[0m'
script_path="scripts/ibisco_simulation.sh"
# script_path="scripts/lxgriv_simulation.sh"


echo -e "${GREEN}[TASK 0 - PULIZIA AMBIENTE]-----------------------------------------------|${NC}"
echo ""
echo "Rimozione file di log..."
echo ""
rm -rf srun.log scripts/*.log
# Controlla tutti i file .txt nella directory corrente
for file in *.txt; do
    if [ -f "$file" ]; then
        echo "Sposto file di media..."
        echo ""
        mv "$file" Medie/
    fi
done

# Inserire osservabile
echo -e "${GREEN}[TASK 1 - OSSERVABILE]----------------------------------------------------|${NC}"
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
echo -e "${GREEN}[TASK 1.1 - OSSERVABILE AUTOCORRELAZIONE]---------------------------------|${NC}"
echo ""
echo "Osservabile AUTOCORRELAZIONE SPIN-SPIN C(t_w, t)."
echo ""
echo "Lunghezza della catena impostata a: 8 siti"
echo ""
L=8
sed -i "s/int L=[0-9]*;/int L=8;/" "main.c"
sed -i "s/int Sz=-\?1;/int Sz=0;/" "main.c"
read -p "Inserire tempo di waiting (t_w): " tw
echo ""
sed -i "s/int tw=[0-9]*;/int tw=$tw;/" "main.c"
fi

if [ "$O" -eq 2 ] || [ "$O" -eq 3 ] || [ "$O" -eq 10 ] || [ "$O" -eq 12 ]; then
    var="Oss$O"
elif [ "$O" -eq 4 ]; then
    var=Energie
elif [ "$O" -eq 5 ]; then
    var="Corr_tw${tw}"
elif [ "$O" -eq 6 ]; then
    var=Spin
fi
# Trova una cartella contenente 'Energie' e verifica la presenza di un solo file con il prefisso 'Durata_'
if [[ $(squeue -u $USER -o "%.8i %.10P %.20j %.10u %.2t %.10M %.5D %.35R" | grep "${var}" | wc -l) -ne 0 ]]; then
    echo "Acquisizione dei dati ${var} in esecuzione."
    echo ""
    squeue -u $USER -o "%.8i %.10P %.20j %.10u %.2t %.10M %.5D %.35R"
    echo ""
    echo "Interruzione dello script..."
    echo ""
    exit 1  
fi


# Inserire modello
echo -e "${GREEN}[TASK 2 - MODELLO DI PARTICELLE]------------------------------------------|${NC}"
echo ""
echo "Modelli 1D di particelle disponibili:"
echo ""
echo " 1 - PARTICELLE LIBERE"
echo "     Jz  = 0     hi  = 0    alfa = 0"
echo ""
echo " 2 - PARTICELLE INTERAGENTI (ISING)"
echo "     Jz  = 0.2   hi  = 0    alfa = 0"
echo ""
echo " 3 - MODELLO DI ANDERSON"
echo "     Jz  = 0     hi  = 5    alfa = 0"
echo ""
echo " 4 - MANY BODY LOCALIZATION (MBL)"
echo "     Jz  = 0.2   hi  = 5    alfa = 0"
echo ""
echo " 5 - MBL + BAGNO SULL'ULTIMO SITO"
echo "     Jz  = 0.2   hi  = 5    alfa = 0.01"
echo ""
read -p "Inserire numero del corrispondente modello: " P
if [ "$P" -eq 1 ]; then
echo " MODELLO: PARTICELLE LIBERE"
elif [ "$P" -eq 2 ]; then
echo " MODELLO: PARTICELLE INTERAGENTI (ISING)"
elif [ "$P" -eq 3 ]; then
echo " MODELLO: ANDERSON"
elif [ "$P" -eq 4 ]; then
echo " MODELLO: MANY BODY LOCALIZATION (MBL)"
elif [ "$P" -eq 5 ]; then
echo " MODELLO: MBL + BAGNO SULL'ULTIMO SITO"
fi
echo ""

# Controlla se la variabile è definita
if [ -z "$L" ]; then
while true; do
read -p "Inserire lunghezza pari e maggiore di 2 della catena (L): " L
echo ""
    # Verifica che la variabile sia maggiore di 2
    if (( L > 2 )); then
        # Controlla se la variabile è dispari
        if (( L % 2 != 0 )); then
            sed -i "s/int Sz=[0-9]*;/int Sz=-1;/" "main.c"
            echo "La lunghezza è dispari. Magnetizzazione totale Sz impostata a: -1"
            echo ""
        else
            sed -i "s/int Sz=-\?1;/int Sz=0;/" "main.c"
            echo "La lunghezza è pari. Magnetizzazione totale Sz impostata a: 0"
            echo ""
        fi
        break
    else
        echo "Errore: La lunghezza della catena deve essere un numero maggiore di 2. Riprova.."
        echo ""
        unset L  # Resetta la variabile per richiedere un nuovo input
    fi
done
sed -i "s/int L=[0-9]*;/int L=$L;/" "main.c"
sleep 2
fi

if [ "$P" -eq 1 ]; then
    mod=Free
elif [ "$P" -eq 2 ]; then
    mod=Ising
elif [ "$P" -eq 3 ]; then
    mod=Anderson
elif [ "$P" -eq 4 ]; then
    mod=MBL
elif [ "$P" -eq 5 ]; then
    mod=MBL+B
fi


echo -e "${GREEN}[TASK 3 - RISORSE CLUSTER IBISCO]------------------------------------------|${NC}"
echo ""

./scripts/resources.sh

echo -e "${GREEN}[TASK 4 - STRUTTURA DATI]------------------------------------------------|${NC}"
echo ""

echo "Eseguire troppi job rispetto al numero di CPU disponibili può causare lunghe attese in coda,"
echo "poiché i job aspettano risorse libere."
echo ""

# Inserire il valore di R
read -p "Numero di run (R): " R
echo ""

# Inserire il valore di J (numero di jobs)
read -p "Numero di jobs da eseguire (J): " J
echo ""

# Inserire numero di step MC
read -p "Numero di step Montecarlo (nstep): " nstep
echo ""

# Sostituzione dei parametri di input nel main.c
sed -i "s/int P = [0-9]*;/int P = $P;/" "main.c"
sed -i "s/int nstep=[0-9]*;/int nstep=$nstep;/" "main.c"
sed -i "s/int Oss=[0-9]*;/int Oss=$O;/" "main.c"


# Compila i file sorgente C
echo -e "${GREEN}[TASK 5 - COMPILAZIONE]----------------------------------------------------|${NC}"
echo ""
# Crea una directory per i file di output
mkdir -p "Dati_${var}"
echo "Creata la directory Dati_${var} per i file di output..."
echo ""
echo "Compilazione dei file sorgente 'main.c' 'loop.c' 'kernel.c'..."
mpiCC -O3 -I/lustre/home/adecandia/.lib2/ main.c loop.c kernel.c -L/lustre/home/adecandia/.lib2/ -lpvm -o "Dati_${var}"/a.out
# g++ -I/lustre/home/adecandia/.lib2/ main.c loop.c kernel.c -L/lustre/home/adecandia/.lib2/ -lpvm -o a.out
echo ""

# Loop per creare screen e eseguire comandi
echo -e "${GREEN}[TASK 6 - ESECUZIONE SIMULAZIONI]------------------------------------------|${NC}"
echo ""
echo "Creazione dello screen: ${var}"
echo "Esecuzione di $script_path..."
echo ""
screen -dmS "${var}" bash -c "bash $script_path ${J} ${R} ${var} ${mod}; exec bash"
sleep 1

# # Loop per creare screen e eseguire comandi
# echo -e "${GREEN}[TASK 5 - ESECUZIONE SIMULAZIONI]------------------------------------------|${NC}"
# echo ""
# echo "Creazione dello screen: ${var}"
# echo "Esecuzione di $script_path..."
# echo ""
# screen -dmS "${var}" bash -c "bash $script_path ${J} ${R} ${var} ${mod}; exec bash"
# sleep 1

# echo -e "${GREEN}[TASK 6 - SCREEN PER LA MEDIA]---------------------------------------------|${NC}"
# echo ""
# echo "Creazione dello screen: media_${var}"
# echo "Esecuzione di $media_script_path..."
# echo ""
# screen -dmS "media_${var}" bash -c "bash $media_script_path ${O} ${var} ${mod} ${L}; exec bash"
# sleep 1

screen -ls
echo "Usa il comando 'screen -ls' per visualizzare lo stato degli screen."
echo "Usa il comando 'screen -r ${var}' per visualizzare l'acquisizione."
# echo "Usa il comando 'screen -r media_${var}' per visualizzare l'acquisizione."
echo ""


# # Reimposta i default dei parametri di input nel main.c
# sed -i "s/int P = [0-9]*;/int P = 0;/" "main.c"
# sed -i "s/int L=[0-9]*;/int L=3;/" "main.c"
# sed -i "s/int nstep=[0-9]*;/int nstep=20;/" "main.c"
# sed -i "s/int tw=[0-9]*;/int tw=1;/" "main.c"
# sed -i "s/int Oss=[0-9]*;/int Oss=0;/" "main.c" 
