#!/bin/bash

output_file="scripts/body.txt"
error_code=$1  # Il codice di errore passato come primo argomento
extra_message=$2

# Seleziona la descrizione dell'errore in base al codice di errore
error_message=""

case $error_code in
    100)
        error_message="FILE O DIRECTORY NON TROVATI:\nIl file 'a.out' o la directory 'Dati' non esistono.\n${extra_message}"
        ;;
    150)
        error_message="PERMESSI INSUFFICIENTI: Non hai i permessi necessari per eseguire i comandi richiesti.\n${extra_message}"
        ;;
    200)
        error_message="PROBLEMI DI SPAZIO SU DISCO: Spazio su disco insufficiente per creare o spostare i file.\n${extra_message}"
        ;;
    250)
        error_message="ERRORE DI ESECUZIONE DEL JOB: Il job non è riuscito a partire a causa di risorse non disponibili.\n${extra_message}"
        ;;
    300)
        error_message="ERRORE NELLA MODIFICA DEI FILE DI OUTPUT: La modifica dei file di output non è riuscita.\n${extra_message}"
        ;;
    350)
        error_message="ERRORE NEL CALCOLO DELLE MEDIE: Il calcolo delle medie non è riuscito a causa di dati malformati.\n${extra_message}"
        ;;
    400)
        error_message="ERRORE DI TEMPO DI ESECUZIONE: La differenza di tempo calcolata per la durata del processo è errata.\n${extra_message}"
        ;;
    450)
        error_message="ERRORE DI NOTIFICA: La notifica di completamento non è stata inviata correttamente.\n${extra_message}"
        ;;
    500)
        error_message="USCITA DALLA SCREEN: Il comando per uscire dallo screen non è riuscito.\n${extra_message}"
        ;;
    550)
        error_message="ERRORE NEL PROCESSO DI PULIZIA: La rimozione dei file temporanei non è riuscita.\n${extra_message}"
        ;;
    *)
        error_message="ERRORE SCONOSCIUTO: Codice di errore non riconosciuto.\n${extra_message}"
        ;;
esac

# Assembla il contenuto del body.txt con la descrizione dell'errore
cat <<EOF > $output_file
------------------------------
    [ERRORE $error_code]
------------------------------

$error_message

-----------------------------

EOF

echo ""
echo "Il file $output_file è stato generato con successo."
echo ""

#-------------------GITHUB-------------------------------------#

echo "Eseguo il commit e il push..."
echo ""

# Esegui il push del commit al repository remoto
# git pull
git add .
git commit -a -m "IBiSco: Errore!"
git push origin master
echo "Commit e push completati."