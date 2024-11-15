<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Mon Gestionnaire de Fichiers</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
        }

        .container {
            width: 80%;
            margin: 20px auto;
        }

        h1 {
            text-align: center;
            color: #4CAF50;
        }

        .file-list {
            list-style-type: none;
            padding: 0;
        }

        .file-item {
            background-color: #fff;
            margin: 10px 0;
            padding: 10px;
            border-radius: 5px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-shadow: 0 0 5px rgba(0, 0, 0, 0.1);
        }

        .file-name {
            font-weight: bold;
            color: #333;
        }

        .file-size {
            color: #777;
        }

        .file-action {
            display: flex;
            justify-content: space-between;
            width: 100px;
        }

        .file-action button {
            padding: 5px 10px;
            border: none;
            background-color: #4CAF50;
            color: white;
            border-radius: 5px;
            cursor: pointer;
        }

        .file-action button:hover {
            background-color: #45a049;
        }

        .file-action .delete-btn {
            background-color: #f44336;
        }

        .file-action .delete-btn:hover {
            background-color: #e53935;
        }
    </style>
</head>
<body>

<div class="container">
    <h1>Liste des fichiers</h1>
    <ul class="file-list">
        <!-- Les fichiers seront injectés ici par PHP -->
        <?php include('list_files.php'); ?>
    </ul>
</div>

</body>
</html>
