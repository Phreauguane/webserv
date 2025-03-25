<?php
// Désactiver l'affichage des erreurs dans la sortie
error_reporting(0);
ini_set('display_errors', 0);

// S'assurer qu'aucun contenu n'a été envoyé avant
if (headers_sent()) {
    exit;
}

// Nettoyer tout buffer de sortie existant
ob_clean();

// Définir les headers
header('Content-Type: application/json');
header('Cache-Control: no-cache, must-revalidate');

require_once '../includes/header.php';

// Vérifier si l'utilisateur est connecté
if (!isLoggedIn()) {
    echo json_encode(['status' => 'error', 'message' => 'Non autorisé']);
    exit;
}

// Vérifier si un fichier a été spécifié
if (!isset($_POST['file'])) {
    echo json_encode(['status' => 'error', 'message' => 'Aucun fichier spécifié']);
    exit;
}

$filename = basename($_POST['file']);
$filepath = __DIR__ . "/uploads/" . $filename;

try {
    // Vérifier si le fichier existe
    if (!file_exists($filepath)) {
        echo json_encode(['status' => 'error', 'message' => 'Fichier non trouvé']);
        exit;
    }

    // Tenter de supprimer le fichier
    if (unlink($filepath)) {
        // Succès : envoyer une réponse positive
        echo json_encode(['status' => 'success', 'message' => 'Fichier supprimé avec succès']);
    } else {
        // Échec : envoyer une réponse d'erreur
        echo json_encode(['status' => 'error', 'message' => 'Erreur lors de la suppression']);
    }
} catch (Exception $e) {
    echo json_encode(['status' => 'error', 'message' => 'Erreur inattendue']);
}

// S'assurer qu'aucun contenu supplémentaire n'est envoyé
exit;
?>