<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = $_POST['username'];
    $password = $_POST['password'];

    // Load the users from the JSON file
    $file = '../database/users.json';
    if (!file_exists($file)) {
        echo "<h1> No users found! </h1>";
        exit;
    }

    $users = json_decode(file_get_contents($file), true);

    // Check if the user exists and verify the password
    if (isset($users[$username]) && password_verify($password, $users[$username])) {
        // Correct credentials
        echo "Login successful! Welcome, " . htmlspecialchars($username);
        session_start();
        $_SESSION['username'] = $username;
    } else {
        // Incorrect credentials
        echo "Invalid username or password!";
    }
}
?>