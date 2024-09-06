#!/bin/bash

output_file="scripts/body.txt"
error_code=$1  # Il codice di errore passato come primo argomento
extra_message=$2
nl=$'\n'  # newline
# Seleziona la descrizione dell'errore in base al codice di errore
error_message=""

case $error_code in
    100)
        error_message_1="FILE O DIRECTORY NON TROVATI"
        error_message_2="I file di dati o la directory 'Dati' non esistono."
        ;;
    110)
        error_message_1="ESEGUIBILE TROVATO"
        error_message_2="Il file 'a.out' non è stato trovato."
        ;;
    150)
        error_message_1="PERMESSI INSUFFICIENTI"
        error_message_2="Non hai i permessi necessari per eseguire i comandi richiesti."
        ;;
    200)
        error_message_1="FILE DI MEDIA NON TROVATO"
        error_message_2="Il file non è stato generato."
        ;;
    250)
        error_message_1="ERRORE DI ESECUZIONE DEI JOB"
        error_message_2="I job non sono riusciti a partire a causa di risorse non disponibili."
        ;;
    300)
        error_message_1="ERRORE DI ESECUZIONE DEL JOB"
        error_message_2="Il job non è partito. Potrebbe essere a rischio cancellazione per mancanza di risorse."
        ;;
    350)
        error_message_1="ERRORE NEL CALCOLO DELLE MEDIE"
        error_message_2="Il calcolo delle medie non è riuscito a causa di dati malformati."
        ;;
    450)
        error_message_1="ERRORE DI NOTIFICA"
        error_message_2="La notifica di completamento non è stata inviata correttamente."
        ;;
    550)
        error_message_1="WARNING NEL PROCESSO DI PULIZIA DEI DATI"
        error_message_2="[media.sh] Eseguita la rimozione dei file di output non conformi o che hanno superato la soglia del 20% di tolleranza di '-nan'."
        ;;

esac

if [ $error_code != 550 ]; then
git_message="Errore $error_code"
# Assembla il contenuto del body.txt con la descrizione dell'errore
cat <<EOF > $output_file
------------------------------
    [ERRORE $error_code]
------------------------------

$error_message_1 $nl
$error_message_2 $nl
${extra_message}

EOF

else
git_message="Warning $error_code"
# Assembla il contenuto del body.txt con la descrizione del warning
cat <<EOF > $output_file
------------------------------
[WARNING $error_code]
------------------------------

$error_message_1 $nl
$error_message_2 $nl
${2}
${3} $nl
${4}

EOF
fi

echo ""
echo "Il file $output_file è stato generato con successo."
echo ""

#-------------------GITHUB-------------------------------------#

echo "Eseguo il commit e il push..."
echo ""

# Esegui il push del commit al repository remoto
# git pull
git add .
git commit -a -m "IBiSco: ${git_message}!"
git push origin ibisco
echo "Commit e push completati."