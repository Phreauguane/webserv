<?php
echo "MIAOUUUU\n";

// Récupérer et afficher le paramètre 'name'
if (isset($_GET['name'])) {
    echo "Hello, " . htmlspecialchars($_GET['name']) . "!";
} else {
    echo "No name provided.";
}
?>