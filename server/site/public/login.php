<?php
require_once '../includes/header.php';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'];
    $password = $_POST['password'];
    
    $users = json_decode(file_get_contents('../database/users.json'), true);
    
    if (isset($users[$username]) && password_verify($password, $users[$username]['password'])) {
        $_SESSION['user_id'] = $username;
        $_SESSION['last_login'] = time();
        
        // Définir un cookie pour se souvenir de l'utilisateur
        if (isset($_POST['remember']) && $_POST['remember'] == 'on')
        {
            setcookie('remember_user', $username, time() + (86400 * 30), "/"); // 30 jours
        }
        
        header('Location: /site/private/dashboard.php');
        exit;
    } else {
        $error = "Nom d'utilisateur ou mot de passe incorrect";
    }
}
?>

<div class="container">
    <div class="auth-container">
        <h2>Connexion</h2>
        <?php if (isset($error)): ?>
            <div class="message error"><?php echo $error; ?></div>
        <?php endif; ?>
        
        <form method="POST" action="">
            <div class="form-group">
                <label for="username">Nom d'utilisateur</label>
                <input type="text" id="username" name="username" required>
            </div>
            
            <div class="form-group">
                <label for="password">Mot de passe</label>
                <input type="password" id="password" name="password" required>
            </div>
            
            <div class="form-group">
                <label>
                    <input type="checkbox" name="remember"> Se souvenir de moi
                </label>
            </div>
            
            <button type="submit" class="btn">Se connecter</button>
        </form>
    </div>
</div>

<?php require_once '../includes/footer.php'; ?>