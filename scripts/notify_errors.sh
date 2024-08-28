#!/bin/bash

output_file="scripts/body.txt"

# Assembla il contenuto del body.txt
cat <<EOF > $output_file
------------------------------

ERRORE: FILE ${1} NON TROVATO!

-----------------------------

EOF

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