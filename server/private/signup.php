<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = $_POST['username'];
    $password = $_POST['password'];

    // Hash the password for security
    $hashed_password = password_hash($password, PASSWORD_DEFAULT);

    // Load the existing users from the JSON file
    $file = '../database/users.json';
    $users = [];

    // Check if the file exists and load the contents
    if (file_exists($file)) {
        $users = json_decode(file_get_contents($file), true);
    }

    // Check if the username is already taken
    if (isset($users[$username])) {
        echo "Username already exists!";
        exit;
    }

    // Add the new user to the array
    $users[$username] = $hashed_password;

    // Save the updated users back to the JSON file
    file_put_contents($file, json_encode($users));

    echo "User signed up successfully!";
}
?>
