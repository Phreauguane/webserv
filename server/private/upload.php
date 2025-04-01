<?php
require_once '../includes/header.php';

if (!isLoggedIn()) {
    header('Location: /public/login.php');
    exit;
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $dossier_destination = "nigga/";
    $max_size = 30720; // 30 KB en octets
    
    if (!isset($error)) {
        if (!file_exists($dossier_destination)) {
            if (!mkdir($dossier_destination, 0777, true)) {
                die("Erreur : Impossible de créer le dossier 'uploads/'");
            }
        }

        if (!isset($_FILES["fichier"])) {
            $error = "Erreur : Le champ 'fichier' n'existe pas";
        } elseif ($_FILES["fichier"]["size"] > $max_size) {
            $error = "Erreur : Le fichier dépasse la taille maximale autorisée (30 KB)";
        } elseif ($_FILES["fichier"]["error"] === UPLOAD_ERR_INI_SIZE ||
                  $_FILES["fichier"]["error"] === UPLOAD_ERR_FORM_SIZE ||
                  $_FILES["fichier"]["error"] === UPLOAD_ERR_PARTIAL ||
                  $_FILES["fichier"]["error"] === UPLOAD_ERR_NO_FILE) {
            $error = "Erreur lors de l'upload : ";
            switch ($_FILES["fichier"]["error"]) {
                case UPLOAD_ERR_INI_SIZE:
                    $error .= "Le fichier dépasse la taille maximale autorisée";
                    break;
                case UPLOAD_ERR_FORM_SIZE:
                    $error .= "Le fichier dépasse la taille maximale autorisée";
                    break;
                case UPLOAD_ERR_PARTIAL:
                    $error .= "Le fichier n'a été que partiellement uploadé";
                    break;
                case UPLOAD_ERR_NO_FILE:
                    $error .= "Aucun fichier n'a été uploadé";
                    break;
                default:
                    $error .= "Erreur inconnue";
            }
        } else {
            // Nettoyer le nom du fichier
            $filename = basename($_FILES["fichier"]["name"]);
            
            // Convertir les caractères spéciaux en ASCII
            $filename = iconv('UTF-8', 'ASCII//TRANSLIT', $filename);
            
            // Remplacer les espaces par des tirets bas
            $filename = str_replace(' ', '_', $filename);
            
            // Supprimer tous les caractères non alphanumériques (sauf points et tirets bas)
            $filename = preg_replace('/[^A-Za-z0-9._-]/', '', $filename);
            
            // Initialiser le compteur avant la boucle
            $counter = 1;
            $fichier_cible = $dossier_destination . $filename;
            
            while (file_exists($fichier_cible)) {
                $info = pathinfo($filename);
                $fichier_cible = $dossier_destination . $info['filename'] . '_' . $counter . '.' . $info['extension'];
                $counter++;
            }
            
            if (move_uploaded_file($_FILES["fichier"]["tmp_name"], $fichier_cible)) {
                chmod($fichier_cible, 0777);
                $success = "Le fichier a été uploadé avec succès";
            } else {
                $error = "Erreur lors du déplacement du fichier";
            }
        }
    }
}
?>

<!DOCTYPE html>
<html lang="fr">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Upload de fichiers</title>
    <link rel="stylesheet" href="/site/assets/css/style.css" />
  </head>
  <body>
    <div class="container">
      <h1>Upload de fichiers</h1>
      <div class="upload-container">
        <?php if (isset($error)): ?>
            <div class="message error"><?php echo $error; ?></div>
        <?php endif; ?>
        <?php if (isset($success)): ?>
            <div class="message success"><?php echo $success; ?></div>
        <?php endif; ?>
        
        <form action="upload.php" method="POST" enctype="multipart/form-data">
          <div class="form-group">
            <label for="fichier">Sélectionner un fichier :</label>
            <input type="file" name="fichier" id="fichier" required />
            <p id="fileSize" class="file-info"></p>
          </div>
          <button type="submit" class="btn">Uploader</button>
        </form>
        <div class="upload-info">
          <p>Taille maximale : 30 KB</p>
          <p>Formats acceptés : tous</p>
        </div>
        <a href="drive.php" class="btn">Voir tous les fichiers</a>
      </div>
    </div>
    <script>
    document.getElementById('fichier').addEventListener('change', function(e) {
        const file = e.target.files[0];
        const fileSizeElement = document.getElementById('fileSize');
        const submitButton = document.querySelector('button[type="submit"]');
        const maxSize = 30 * 1024; // 30KB en octets
        
        if (file) {
            const fileSize = file.size;
            let sizeStr;
            
            if (fileSize < 1024) {
                sizeStr = fileSize + ' B';
            } else if (fileSize < 1024 * 1024) {
                sizeStr = (fileSize / 1024).toFixed(2) + ' KB';
            } else {
                sizeStr = (fileSize / (1024 * 1024)).toFixed(2) + ' MB';
            }
            
            fileSizeElement.textContent = 'Taille du fichier : ' + sizeStr;
            
            if (fileSize > maxSize) {
                fileSizeElement.style.color = 'red';
                fileSizeElement.textContent += ' (dépasse la limite de 30KB)';
                submitButton.disabled = true;
                submitButton.style.opacity = '0.5';
                submitButton.style.cursor = 'not-allowed';
            } else {
                fileSizeElement.style.color = 'green';
                submitButton.disabled = false;
                submitButton.style.opacity = '1';
                submitButton.style.cursor = 'pointer';
            }
        } else {
            fileSizeElement.textContent = '';
            submitButton.disabled = false;
            submitButton.style.opacity = '1';
            submitButton.style.cursor = 'pointer';
        }
    });

    // Validation du formulaire avant soumission
    document.querySelector('form').addEventListener('submit', function(e) {
        const file = document.getElementById('fichier').files[0];
        const maxSize = 30 * 1024; // 30KB
        
        if (file && file.size > maxSize) {
            e.preventDefault();
            alert('Le fichier est trop volumineux. La taille maximale autorisée est de 30KB.');
        }
    });
    </script>
  </body>
</html>
<?php require_once '../includes/footer.php'; ?>