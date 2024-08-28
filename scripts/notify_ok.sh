#!/bin/bash

output_file="scripts/body.txt"

# Verifica se esiste il file di media
if ! ls "${1}" 1> /dev/null 2>&1; then

cat <<EOF > $output_file
------------------------------
[Dettagli della Simulazione]
------------------------------

ERRORE: FILE ${1} NON TROVATO!

-----------------------------
[Dettagli Job Lanciati]
-----------------------------

Task totali: ${5}
Job totali:

EOF

fi

echo ""
echo "File ${1} trovato. Generazione del body per email di notifica..."

# Assembla il contenuto del body.txt
cat <<EOF > $output_file
------------------------------
[Dettagli della Simulazione]
------------------------------
Start Date: ${6}
End Date:   $(date +"%d-%m-%Y %H:%M:%S")
Durata:     ${7}

Modello selezionato: ${3}
Osservabile scelto:  ${2}

Parametri del Modello:
- L:     ${4}
- nstep: $(grep -oP '(?<=^nstep=)\d+' "${1}")
- Sz:    $(grep -oP '(?<=^Sz=)-?\d+' "${1}")
- Jxy:   1
- Jz:    $(grep -oP '(?<=^Jz=)\d+(\.\d+)?' "${1}")
- eps:   $(grep -oP '(?<=^eps=)\d+(\.\d+)?' "${1}")
- alpha: $(grep -oP '(?<=^alpha=)\d+(\.\d+)?' "${1}")

-----------------------------
[Dettagli Job Lanciati]
-----------------------------

Task totali: ${5}
Job totali:

EOF

echo "Il file $output_file è stato generato con successo."
echo ""

#-------------------GITHUB-------------------------------------#

echo "Eseguo il commit e il push..."
echo ""

# Esegui il push del commit al repository remoto
# git pull
git add .
git commit -a -m "IBiSco: Jobs eseguiti con successo!"
git push origin master
echo "Commit e push completati."