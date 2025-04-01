<?php
require_once '../includes/header.php';

if (!isLoggedIn()) {
    header('Location: /public/login.php');
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

// Ajouter cette partie au début du fichier, après les vérifications de session
if (isset($_GET['download']) && !empty($_GET['download'])) {
    $filename = basename($_GET['download']);
    $filepath = 'uploads/' . $filename;
    
    if (file_exists($filepath)) {
        // Détection du type MIME
        $finfo = finfo_open(FILEINFO_MIME_TYPE);
        $mime_type = finfo_file($finfo, $filepath);
        finfo_close($finfo);
        
        // Headers pour le téléchargement
        header('Content-Type: ' . $mime_type);
        header('Content-Disposition: attachment; filename="' . $filename . '"');
        header('Content-Length: ' . filesize($filepath));
        header('Cache-Control: no-cache, must-revalidate');
        header('Pragma: public');
        
        // Envoi du fichier
        readfile($filepath);
        exit;
    }
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
                        echo '<div class="file-info">';
                        echo '<span class="file-name">' . htmlspecialchars($file) . '</span>';
                        echo '<span class="file-size">' . formatBytes($fileSize) . '</span>';
                        echo '</div>';
                        echo '<div class="file-actions">';
                        echo '<button onclick="deleteItem(\'' . htmlspecialchars($file) . '\')" class="btn btn-delete">Supprimer</button>';
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

<style>
.file-item {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 15px;
    margin: 10px 0;
    background: white;
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.file-info {
    display: flex;
    align-items: center;
    gap: 20px;
}

.file-name {
    font-weight: 500;
    color: #333;
}

.file-size {
    color: #666;
    font-size: 0.9em;
}

.file-actions {
    display: flex;
    gap: 10px;
}

.btn {
    padding: 8px 15px;
    border-radius: 4px;
    border: none;
    cursor: pointer;
    font-size: 14px;
    transition: all 0.2s ease;
}

.btn-delete {
    background-color: #f44336;
    color: white;
}

.btn-delete:hover {
    background-color: #da190b;
}
</style>

<script>
function deleteItem(filename) {
    if (confirm('Êtes-vous sûr de vouloir supprimer ce fichier ?')) {
        fetch('delete_file.php', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: 'file=' + encodeURIComponent(filename)
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                location.reload();
            } else {
                alert('Erreur lors de la suppression du fichier');
            }
        });
    }
}
</script>

<?php require_once '../includes/footer.php'; ?>