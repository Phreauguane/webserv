console.log('JavaScript chargé avec succès');
window.onerror = function(msg, url, lineNo, columnNo, error) {
    console.error('Erreur : ' + msg + '\nURL : ' + url + '\nLigne : ' + lineNo);
    return false;
};

window.addEventListener('load', function() {
    document.body.classList.remove('js-loading');
    document.body.classList.add('js-loaded');
    
    // Gestion des messages d'alerte
    const messages = document.querySelectorAll('.message');
    messages.forEach(message => {
        setTimeout(() => {
            message.style.opacity = '0';
            setTimeout(() => message.remove(), 300);
        }, 5000);
    });

    // Validation des formulaires
    const forms = document.querySelectorAll('form');
    forms.forEach(form => {
        form.addEventListener('submit', function(e) {
            const passwordInputs = form.querySelectorAll('input[type="password"]');
            if (passwordInputs.length > 1) {
                const passwords = Array.from(passwordInputs).map(input => input.value);
                if (new Set(passwords).size > 1) {
                    e.preventDefault();
                    alert('Les mots de passe ne correspondent pas');
                }
            }
        });
    });

    // Animation du menu de navigation
    const navItems = document.querySelectorAll('.navbar a');
    navItems.forEach(item => {
        item.addEventListener('mouseenter', function() {
            this.style.transition = 'color 0.3s ease';
            this.style.color = '#4CAF50';
        });
        item.addEventListener('mouseleave', function() {
            this.style.transition = 'color 0.3s ease';
            this.style.color = '';
        });
    });
});

// document.addEventListener('DOMContentLoaded', function() {
//     // Créer un compteur pour les feuilles de style
//     let styleSheets = document.styleSheets;
//     let loadedStyles = 0;
    
//     function checkStylesLoaded() {
//         loadedStyles++;
//         if (loadedStyles === styleSheets.length) {
//             document.body.classList.remove('js-loading');
//             document.body.classList.add('js-loaded');
//         }
//     }
    
//     // Vérifier chaque feuille de style
//     Array.from(styleSheets).forEach(sheet => {
//         if (sheet.href) {
//             let link = document.querySelector(`link[href="${sheet.href}"]`);
//             if (link) {
//                 if (link.sheet) {
//                     checkStylesLoaded();
//                 } else {
//                     link.onload = checkStylesLoaded;
//                 }
//             }
//         }
//     });
// });


document.addEventListener('DOMContentLoaded', function() {
    // Attendre un court instant pour s'assurer que les styles sont appliqués
    requestAnimationFrame(() => {
        requestAnimationFrame(() => {
            document.body.classList.remove('js-loading');
            document.body.classList.add('js-loaded');
        });
    });
});