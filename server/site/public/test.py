#!/usr/bin/python3
import os
import cgi
import urllib.parse

print("Content-type: text/html\n")
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Test Python CGI</title>
    <link rel="stylesheet" href="/site/assets/css/style.css">
</head>
<body>
    <div class="container">
        <h1>Test Python CGI</h1>
""")

# Récupérer et traiter les données du formulaire
query_string = os.environ.get('QUERY_STRING', '')
if query_string:
    # Parser les paramètres de l'URL
    params = urllib.parse.parse_qs(query_string)
    if 'name' in params:
        name = params['name'][0]
        print(f"<p>Bonjour, {name}!</p>")
        print("<p><a href='/site/public/test.py' class='btn'>Retour au formulaire</a></p>")
    else:
        print("<p>Erreur: paramètre 'name' manquant</p>")
else:
    # Afficher le formulaire
    print("""
        <form method="get" action="/site/public/test.py">
            <div class="form-group">
                <label for="name">Votre prenom:</label>
                <input type="text" name="name" id="name" required>
            </div>
            <button type="submit" class="btn">Envoyer</button>
        </form>
    """)

print("""
    </div>
</body>
</html>
""") 