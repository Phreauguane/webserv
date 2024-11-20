<?php
session_start();

// Détruire la session
session_destroy();

// Supprimer le cookie de "remember me"
if (isset($_COOKIE['remember_user'])) {
    setcookie('remember_user', '', time() - 3600, '/');
}

// Rediriger vers la page de connexion
header('Location: /site/public/login.php');
exit;