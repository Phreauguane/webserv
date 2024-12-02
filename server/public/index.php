<?php require_once '../includes/header.php'; ?>

<div class="container">
    <div class="welcome-section">
        <h1>Bienvenue sur notre site</h1>
        <?php if (isLoggedIn()): ?>
            <p>Content de vous revoir, <?php echo htmlspecialchars($_SESSION['user_id']); ?> !</p>
            <div class="action-buttons">
                <a href="/private/dashboard.php" class="btn">Accéder au tableau de bord</a>
                <a href="/private/profile.php" class="btn">Gérer mon profil</a>
            </div>
        <?php else: ?>
            <p>Veuillez vous connecter ou créer un compte pour accéder à toutes les fonctionnalités.</p>
            <div class="action-buttons">
                <a href="/public/login.php" class="btn">Se connecter</a>
                <a href="/public/register.php" class="btn">S'inscrire</a>
            </div>
        <?php endif; ?>
    </div>
</div>

<?php require_once '../includes/footer.php'; ?>
