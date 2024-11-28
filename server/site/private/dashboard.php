<?php
require_once '../includes/header.php';

if (!isLoggedIn()) {
    header('Location: /site/public/login.php');
    exit;
}

$username = $_SESSION['user_id'];
?>

<div class="container">
    <h1>Tableau de bord</h1>
    <p>Bienvenue, <?php echo htmlspecialchars($username); ?>!</p>
    <p>Dernière connexion : <?php echo date('d/m/Y H:i:s', $_SESSION['last_login']); ?></p>
    
    <div class="dashboard-stats">
        <h3>Vos informations</h3>
        <ul>
            <li>Session ID: <?php echo session_id(); ?></li>
            <li>Cookies actifs: <?php echo count($_COOKIE); ?></li>
            <li>Temps de session: <?php echo round((time() - $_SESSION['last_login']) / 60); ?> minutes</li>
        </ul>
    </div>
</div>

<?php require_once '../includes/footer.php'; ?>