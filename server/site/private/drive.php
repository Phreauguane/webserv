<?php
require_once '../includes/header.php';

if (!isLoggedIn()) {
    header('Location: /site/public/login.php');
    exit;
}

// Définir le dossier à scanner
$upload_dir = 'uploads/';

// Fonction pour formater la taille des fichiers
function formatBytes($bytes) {
    if ($bytes > 0) {
        $i = floor(log($bytes) / log(1024));
        $sizes = array('B', 'KB', 'MB', 'GB', 'TB');
        return sprintf('%.2f', $bytes / pow(1024, $i)) * 1 . ' ' . $sizes[$i];
    }
    return '0 B';
}
?>

<div class="container">
    <h1>Liste des fichiers</h1>
    <div class="file-actions">
        <a href="upload.php" class="btn">Uploader un fichier</a>
    </div>
    <ul class="file-list">
        <?php
        // Vérifier si le dossier existe
        if (is_dir($upload_dir)) {
            $files = scandir($upload_dir);
            foreach ($files as $file) {
                if ($file != '.' && $file != '..') {
                    $filePath = $upload_dir . $file;
                    if (is_file($filePath)) {
                        $fileSize = filesize($filePath);
                        echo '<li class="file-item">';
                        echo '<span class="file-name">' . htmlspecialchars($file) . '</span>';
                        echo '<span class="file-size">' . formatBytes($fileSize) . '</span>';
                        echo '<div class="file-action">';
                        echo '<button onclick="deleteItem(\'' . htmlspecialchars($file) . '\')">Supprimer</button>';
                        echo '</div>';
                        echo '</li>';
                    }
                }
            }
        } else {
            echo '<li>Le dossier uploads n\'existe pas.</li>';
        }
        ?>
    </ul>
</div>

<script>
function deleteItem(filename) {
    if (confirm('Êtes-vous sûr de vouloir supprimer ce fichier ?')) {
        fetch('/site/private/uploads/' + filename, {
            method: 'DELETE',
        })
        .then(response => {
            if (response.ok) {
                location.reload();
            } else {
                alert('Erreur lors de la suppression du fichier');
            }
        })
        .catch(error => {
            console.error('Erreur:', error);
            alert('Erreur lors de la suppression du fichier');
        });
    }
}
</script>

<?php require_once '../includes/footer.php'; ?>