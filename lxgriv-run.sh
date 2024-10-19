#!/bin/bash

echo ""
GREEN='\033[0;32m'
NC='\033[0m'
# script_path="scripts/ibisco_simulation.sh"
script_path="scripts/lxgriv_simulation.sh"


# echo -e "${GREEN}[TASK 0 - PULIZIA AMBIENTE]-----------------------------------------------|${NC}"
# echo ""
# echo "Rimozione file di log..."
# echo ""
# rm -rf *.log scripts/*.log

# Verifica se la cartella "Medie" esiste, altrimenti creala
if [ ! -d "Medie" ]; then
    echo "La cartella Medie non esiste. Creazione della cartella..."
    mkdir Medie
fi

# Inserire osservabile
echo -e "${GREEN}[TASK 1 - OSSERVABILE]----------------------------------------------------|${NC}"
echo ""
echo "Osservabili disponibili:"
echo ""
echo " 2 - ENEGIA TOTALE E_tot"
echo ""
# echo " 3 - ENEGIA E_hop"
# echo "10 - ENEGIA E_Int"
# echo "12 - ENEGIA E_mag"
echo " 4 - ENEGIE H_hop, H_int, H_mag"
echo ""
echo " 5 - AUTOCORRELAZIONE 8 SPIN"
echo ""
echo " 6 - COMPONENI SPIN SU i=L"
echo ""
read -p "Inserire numero del corrispondente osservabile: " O
echo ""
if [ "$O" -eq 2 ]; then
echo " OSSERVABILE: ENEGIA TOTALE E_tot"
elif [ "$O" -eq 6 ]; then
echo " OSSERVABILE: COMPONENI SPIN SU i=L"
elif [ "$O" -eq 4 ]; then
echo " OSSERVABILE: ENEGIE H_hop, H_int, H_mag"
elif [ "$O" -eq 5 ]; then
echo " OSSERVABILE: AUTOCORRELAZIONE 8 SPIN"
fi
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
else
    # setta tw=1 se l'osservabile è diverso dall'autocorrelazione
    sed -i "s/int tw=[0-9]*;/int tw=1;/" "main.c"
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

if screen -ls | grep -q "${var}"; then
    echo "Acquisizione dei dati ${var} in esecuzione."
    echo ""
    screen -ls
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
echo ""
if [ "$P" -eq 1 ]; then
    echo " MODELLO: PARTICELLE LIBERE"
    mod=Free
elif [ "$P" -eq 2 ]; then
    echo " MODELLO: PARTICELLE INTERAGENTI (ISING)"
    mod=Ising
elif [ "$P" -eq 3 ]; then
    echo " MODELLO: ANDERSON"
    mod=Anderson
elif [ "$P" -eq 4 ]; then
    echo " MODELLO: MANY BODY LOCALIZATION (MBL)"
    mod=MBL
elif [ "$P" -eq 5 ]; then
    echo " MODELLO: MBL + BAGNO SULL'ULTIMO SITO"
    mod=MBL+B
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

echo -e "${GREEN}[TASK 3 - STRUTTURA DATI]------------------------------------------------|${NC}"
echo ""

# Inserire il valore di R
read -p "Numero di run (R): " R
echo ""

# Controlla se R è maggiore di 200
while [ "$R" -gt 200 ]; do
    echo "Il valore inserito è maggiore di 200. Per favore, inserisci un valore più basso."
    read -p "Inserisci un nuovo valore per R: " R
    echo ""
done

# Inserire il valore di J (numero di jobs) o usa il valore predefinito di 1
read -p "Numero di jobs da eseguire (J): " J
if [ -z "$J" ]; then
    J=1
    echo "J non valorizzato, impostato a default: $J"
fi
echo ""

# Inserire numero di step MC
if [ ! -z "$tw" ] && [ "$tw" -gt 1 ]; then
    echo "Attenzione, il conteggio degli step MC partirà da tw = ${tw}."
    nstep=$tw
fi
read -p "Numero di step Montecarlo (nstep): " n
((nstep += n))
echo "Numero di step MC totali: $nstep"
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
echo "Creata la directory per i file di output: Dati_${var}"
echo ""
echo "Compilazione dei file sorgente 'main.c' 'loop.c' 'kernel.c'..."
mpiCC -O3 -I/home/desposito/.lib2/ main.c loop.c kernel.c -L/home/desposito/.lib2/ -lpvm -o "Dati_${var}"/a.out
# g++ -I/home/desposito/.lib2/ main.c loop.c kernel.c -L/home/desposito/.lib2/ -lpvm
echo ""

# Loop per creare screen e eseguire comandi
echo -e "${GREEN}[TASK 6 - ESECUZIONE SIMULAZIONI]------------------------------------------|${NC}"
echo ""
echo "Creazione dello screen: ${mod}_${var}"
echo ""
echo "Esecuzione di $script_path..."
echo ""
screen -dmS "${mod}_${var}" bash -c "bash $script_path ${J} ${R} ${var} ${mod}; exec bash"
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
echo "Usa il comando ' screen -ls ' per visualizzare lo stato degli screen."
echo "Usa il comando ' screen -r ${mod}_${var} ' per visualizzare l'acquisizione."
# echo "Usa il comando 'screen -r media_${var}' per visualizzare l'acquisizione."
echo ""


# # Reimposta i default dei parametri di input nel main.c
# sed -i "s/int P = [0-9]*;/int P = 0;/" "main.c"
# sed -i "s/int L=[0-9]*;/int L=3;/" "main.c"
# sed -i "s/int nstep=[0-9]*;/int nstep=20;/" "main.c"
# sed -i "s/int tw=[0-9]*;/int tw=1;/" "main.c"
# sed -i "s/int Oss=[0-9]*;/int Oss=0;/" "main.c" 
