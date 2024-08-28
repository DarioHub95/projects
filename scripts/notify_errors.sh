#!/bin/bash

output_file="scripts/body.txt"
error_code=$1  # Il codice di errore passato come primo argomento
extra_message=$2

# Seleziona la descrizione dell'errore in base al codice di errore
error_message=""

case $error_code in
    100)
        error_message_1="FILE O DIRECTORY NON TROVATI"
        error_message_2="Il file 'a.out' o la directory 'Dati' non esistono."
        error_message_3="NOTA: ${extra_message}"
        ;;
    150)
        error_message_1="PERMESSI INSUFFICIENTI"
        error_message_2="Non hai i permessi necessari per eseguire i comandi richiesti."
        error_message_3="NOTA: ${extra_message}"
        ;;
    200)
        error_message_1="PROBLEMI DI SPAZIO SU DISCO"
        error_message_2="Spazio su disco insufficiente per creare o spostare i file."
        error_message_3="NOTA: ${extra_message}"
        ;;
    250)
        error_message_1="ERRORE DI ESECUZIONE DEL JOB"
        error_message_2="Il job non è riuscito a partire a causa di risorse non disponibili."
        error_message_3="NOTA: ${extra_message}"
        ;;
    300)
        error_message_1="ERRORE NELLA MODIFICA DEI FILE DI OUTPUT"
        error_message_2="La modifica dei file di output non è riuscita."
        error_message_3="NOTA: ${extra_message}"
        ;;
    350)
        error_message_1="ERRORE NEL CALCOLO DELLE MEDIE"
        error_message_2="Il calcolo delle medie non è riuscito a causa di dati malformati."
        error_message_3="NOTA: ${extra_message}"
        ;;
    400)
        error_message_1="ERRORE DI TEMPO DI ESECUZIONE"
        error_message_2="La differenza di tempo calcolata per la durata del processo è errata."
        error_message_3="NOTA: ${extra_message}"
        ;;
    450)
        error_message_1="ERRORE DI NOTIFICA"
        error_message_2="La notifica di completamento non è stata inviata correttamente."
        error_message_3="NOTA: ${extra_message}"
        ;;
    500)
        error_message_1="USCITA DALLA SCREEN"
        error_message_2="Il comando per uscire dallo screen non è riuscito."
        error_message_3="NOTA: ${extra_message}"
        ;;
    550)
        error_message_1="ERRORE NEL PROCESSO DI PULIZIA"
        error_message_2="La rimozione dei file temporanei non è riuscita."
        error_message_3="NOTA: ${extra_message}"
        ;;
    *)
        error_message_1="ERRORE SCONOSCIUTO"
        error_message_2="Codice di errore non riconosciuto."
        error_message_3="NOTA: ${extra_message}"
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