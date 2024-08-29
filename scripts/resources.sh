#!/bin/bash

# Contare il numero di nodi in stato "allocated", "mixed" e "idle"
allocated_count=$(sinfo -lN | grep " parallel" | awk '$4 == "allocated"' | wc -l)
mixed_count=$(sinfo -lN | grep " parallel" | awk '$4 == "mixed"' | wc -l)
idle_count=$(sinfo -lN | grep " parallel" | awk '$4 == "idle"' | wc -l)

# Stampare il numero di nodi in ciascuno stato
echo "===================================="
echo "Nodi Allocated: $allocated_count"
echo "Nodi Mixed:     $mixed_count"
echo "Nodi Idle:      $idle_count"
echo "===================================="
echo ""

# Creare la lista dei nodi in stato "mixed" e "idle"
echo "Lista dei nodi Mixed e Idle per la partizione parallel:"
sinfo -lN | sed -n '2p'
sinfo -lN | grep " parallel" | awk '$4 == "mixed" || $4 == "idle"'

# Ottenere le risorse idle (inattive) dal cluster
idle_cpus=$(sinfo -o "%C" | tail -n 1 | awk -F "/" '{print $2}')

# Estrai le colonne MEMORY e FREE_MEM da sinfo
sinfo_output=$(sinfo -e -o "%m %e" | tail -n +2)

# Inizializza variabili per memoria totale e memoria libera
total_memory=0
free_memory=0

# Itera attraverso ciascuna riga dell'output di sinfo
while read -r line; do
  # Estrai il valore di MEMORY e FREE_MEM dalla riga
  memory=$(echo $line | awk '{print $1}')
  free_mem=$(echo $line | awk '{print $2}')

  # Somma la memoria totale e la memoria libera
  total_memory=$((total_memory + memory))
  free_memory=$((free_memory + free_mem))
done <<< "$sinfo_output"

idle_mem=$(echo "scale=2; ($free_memory / $total_memory) * 100" | bc)

# Mostrare le risorse idle
echo ""
echo "Risorse disponibili (Idle) nel cluster:"
echo "===================================="
echo "CPU disponibili:     $idle_cpus"
echo "Memoria disponibile: $idle_mem %"
echo "===================================="
echo ""