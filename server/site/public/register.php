<?php
require_once '../includes/header.php';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'];
    $password = $_POST['password'];
    $confirm_password = $_POST['confirm_password'];
    
    if ($password !== $confirm_password) {
        $error = "Les mots de passe ne correspondent pas";
    } else {
        $users = json_decode(file_get_contents('../database/users.json'), true) ?? [];
        
        if (isset($users[$username])) {
            $error = "Ce nom d'utilisateur existe déjà";
        } else {
            $users[$username] = [
                'password' => password_hash($password, PASSWORD_DEFAULT),
                'created_at' => date('Y-m-d H:i:s')
            ];
            
            file_put_contents('../database/users.json', json_encode($users, JSON_PRETTY_PRINT));
            
            $_SESSION['user_id'] = $username;
            header('Location: /site/private/dashboard.php');
            exit;
        }
    }
}
?>

<div class="container">
    <div class="auth-container">
        <h2>Inscription</h2>
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
                <label for="confirm_password">Confirmer le mot de passe</label>
                <input type="password" id="confirm_password" name="confirm_password" required>
            </div>
            
            <button type="submit" class="btn">S'inscrire</button>
        </form>
    </div>
</div>

<?php require_once '../includes/footer.php'; ?>