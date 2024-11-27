<?php
session_start();

function isLoggedIn() {
    return isset($_SESSION['user_id']);
}
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Mon Site</title>
    <link rel="preload" href="/site/assets/css/style.css" as="style" media="all">
    <link rel="stylesheet" href="/site/assets/css/style.css" media="all">
    <link rel="icon" type="image/x-icon" href="/site/assets/images/favicon.ico">
</head>
<body class="js-loading">
    <nav class="navbar">
        <div class="container">
            <ul>
                <li><a href="/site/public/index.php">Accueil</a></li>
                <?php if (isLoggedIn()): ?>
                    <li><a href="/site/private/dashboard.php">Dashboard</a></li>
                    <li><a href="/site/private/profile.php">Profil</a></li>
                    <li><a href="/site/public/logout.php">Déconnexion</a></li>
                    <li><a href="/site/private/drive.php">Drive</a></li>
                    <li><a href="/site/private/upload.php">Upload</a></li>
                <?php else: ?>
                    <li><a href="/site/public/login.php">Connexion</a></li>
                    <li><a href="/site/public/register.php">Inscription</a></li>
                <?php endif; ?>
            </ul>
        </div>
    </nav>
    <!-- <script>
        document.addEventListener('DOMContentLoaded', function() {
            document.body.classList.remove('js-loading');
            document.body.classList.add('js-loaded');
        });
    </script> -->