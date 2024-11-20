<?php
require_once '../includes/header.php';

if (!isLoggedIn()) {
    header('Location: /site/public/login.php');
    exit;
}

$username = $_SESSION['user_id'];
$users = json_decode(file_get_contents('../database/users.json'), true);
$user_data = $users[$username];

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $current_password = $_POST['current_password'];
    $new_password = $_POST['new_password'];
    $confirm_password = $_POST['confirm_password'];
    
    if (password_verify($current_password, $user_data['password'])) {
        if ($new_password === $confirm_password) {
            $users[$username]['password'] = password_hash($new_password, PASSWORD_DEFAULT);
            file_put_contents('../database/users.json', json_encode($users, JSON_PRETTY_PRINT));
            $success = "Mot de passe modifié avec succès";
        } else {
            $error = "Les nouveaux mots de passe ne correspondent pas";
        }
    } else {
        $error = "Mot de passe actuel incorrect";
    }
}
?>

<div class="container">
    <div class="profile-container">
        <h2>Mon Profil</h2>
        
        <?php if (isset($success)): ?>
            <div class="message success"><?php echo $success; ?></div>
        <?php endif; ?>
        
        <?php if (isset($error)): ?>
            <div class="message error"><?php echo $error; ?></div>
        <?php endif; ?>
        
        <div class="user-info">
            <p><strong>Nom d'utilisateur:</strong> <?php echo htmlspecialchars($username); ?></p>
            <p><strong>Inscrit le:</strong> <?php echo $user_data['created_at']; ?></p>
        </div>
        
        <form method="POST" action="" class="password-form">
            <h3>Changer le mot de passe</h3>
            
            <div class="form-group">
                <label for="current_password">Mot de passe actuel</label>
                <input type="password" id="current_password" name="current_password" required>
            </div>
            
            <div class="form-group">
                <label for="new_password">Nouveau mot de passe</label>
                <input type="password" id="new_password" name="new_password" required>
            </div>
            
            <div class="form-group">
                <label for="confirm_password">Confirmer le nouveau mot de passe</label>
                <input type="password" id="confirm_password" name="confirm_password" required>
            </div>
            
            <button type="submit" class="btn">Modifier le mot de passe</button>
        </form>
    </div>
</div>

<?php require_once '../includes/footer.php'; ?>