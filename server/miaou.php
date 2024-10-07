<?php
// Vérifie si la méthode de la requête est POST
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Récupérer le nom du meow
    $meowName = $_POST['miaou'] ?? 'Miaou non fourni';

    // Récupérer les caractéristiques sélectionnées
    $features = isset($_POST['features']) ? $_POST['features'] : [];

    // Afficher le résultat
    echo "<!DOCTYPE html>
    <html lang='en'>
    <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
        <title>Miaou - Result</title>
        <link rel='stylesheet' href='miaou.css'>
    </head>
    <body>
        <div class='header'>
            <h1>Results for \"$meowName\"</h1>
        </div>
        <div class='container'>
            <h2>Your Meow's Features:</h2>
            <ul>";

    // Vérifie si des caractéristiques ont été sélectionnées
    if (empty($features)) {
        echo "<li>No features selected.</li>";
    } else {
        foreach ($features as $feature) {
            echo "<li>" . htmlspecialchars($feature) . "</li>";
        }
    }

    echo "</ul>
        <a href='index.html'>Go Back</a>
        </div>
    </body>
    </html>";
} else {
    // Rediriger vers le formulaire si la requête n'est pas POST
    header("Location: index.html");
    exit();
}
?>
