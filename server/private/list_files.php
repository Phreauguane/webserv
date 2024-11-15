<?php
// Définir le dossier à scanner
$folderPath = '../upload'; // Remplacez ceci par le chemin réel du dossier

// Vérifier si le dossier existe
if (is_dir($folderPath)) {
    // Ouvrir le dossier
    $files = scandir($folderPath); // scandir renvoie une liste de tous les fichiers dans le dossier

    // Filtrer les fichiers (scandir inclut . et ..)
    $files = array_diff($files, array('.', '..'));

    // Afficher chaque fichier
    foreach ($files as $file) {
        // Obtenir le chemin complet du fichier
        $filePath = $folderPath . '/' . $file;

        // Vérifier si c'est bien un fichier
        if (is_file($filePath))
		{
            // Obtenir la taille du fichier
            $fileSize = filesize($filePath);
            // Afficher le fichier avec son nom et sa taille
            echo '<li class="file-item">';
            echo '<span class="file-name">' . htmlspecialchars($file) . '</span>';
            echo '<span class="file-size">' . formatBytes($fileSize) . '</span>';
            echo '<div class="file-action">';
			// echo '<form action="drive.php" method="DELETE" enctype="multipart/form-data">';
            // echo '<button class="delete-btn">Supprimer</button>';
			echo '<button onclick="deleteItem()">Delete Item</button>

			<script>
			function deleteItem() {
			  fetch("../upload/main.c", {
				method: "DELETE",
			  })
			  .then(response => response.json())
			  .then(data => {
				console.log("Item deleted:", data)
			  })
			  .catch(error => {
				console.error("Error:", error)
			  })
			}
			</script>';
            echo '</form>';
			echo '</div>';
            echo '</li>';
        }
    }
} else {
    echo 'Le dossier n\'existe pas.';
}

// Fonction pour formater la taille des fichiers (en octets)
function formatBytes($bytes, $precision = 2) {
    $units = array('B', 'KB', 'MB', 'GB', 'TB');
    $bytes = max($bytes, 0);
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
    $pow = min($pow, count($units) - 1);
    $bytes /= pow(1024, $pow);
    return round($bytes, $precision) . ' ' . $units[$pow];
}
?>
