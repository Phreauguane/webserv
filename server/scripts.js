document.addEventListener("DOMContentLoaded", function() {
    const menuToggle = document.getElementById("menu-toggle");
    const menuDropdown = document.getElementById("menu-dropdown");
    const profileIcon = document.getElementById("profile-icon");
    const profileDropdown = document.getElementById("profile-dropdown");

    // Toggle the navigation menu with animation
    menuToggle.addEventListener("click", function() {
        menuDropdown.classList.toggle("visible");
    });

    // Toggle the profile dropdown with animation
    profileIcon.addEventListener("click", function() {
        profileDropdown.classList.toggle("visible");
    });

    // Close dropdown if clicked outside
    document.addEventListener("click", function(e) {
        if (!profileIcon.contains(e.target) && !profileDropdown.contains(e.target)) {
            profileDropdown.classList.remove("visible");
        }
        if (!menuToggle.contains(e.target) && !menuDropdown.contains(e.target)) {
            menuDropdown.classList.remove("visible");
        }
    });
});
