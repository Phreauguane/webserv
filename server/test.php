<?php
// Afficher un message de début
echo "Début de l'attente de 30 secondes...\n";

// Enregistrer le temps de début
$start_time = time();

// Attendre 30 secondes
sleep(30);

// Calculer le temps écoulé réel
$elapsed_time = time() - $start_time;

// Afficher un message de fin
echo "Fin de l'attente après $elapsed_time secondes.\n";
?>