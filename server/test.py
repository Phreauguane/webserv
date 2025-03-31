#!/usr/bin/env python3

import time
import sys

# Envoyer l'en-tête HTTP
print("Content-type: text/html\n\n")

# Début du document HTML
print("<html><head><title>Test d'attente de 30 secondes</title></head><body>")
print("<h1>Test d'attente de 30 secondes</h1>")

# Afficher un message de début et vider le buffer pour que l'utilisateur le voie immédiatement
print("<p>Début de l'attente de 30 secondes...</p>")
print("<script>document.getElementById('status').innerHTML = 'En cours d'attente...';</script>")
sys.stdout.flush()

# Enregistrer le temps de début
start_time = time.time()

# Attendre 30 secondes
time.sleep(30)

# Calculer le temps écoulé réel
elapsed_time = time.time() - start_time

# Afficher un message de fin
print(f"<p>Fin de l'attente après {elapsed_time:.2f} secondes.</p>")
print("</body></html>")