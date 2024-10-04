document.addEventListener("DOMContentLoaded", function() {
    const menuToggle = document.getElementById("menu-toggle");
    const navMenu = document.getElementById("nav-menu");
    const profileIcon = document.getElementById("profile-icon");
    const profileDropdown = document.getElementById("profile-dropdown");

    // Toggle the navigation menu
    menuToggle.addEventListener("click", function() {
        navMenu.classList.toggle("hidden");
    });

    // Toggle the profile dropdown
    profileIcon.addEventListener("click", function() {
        profileDropdown.classList.toggle("hidden");
    });

    // Close dropdown if clicked outside
    document.addEventListener("click", function(e) {
        if (!profileIcon.contains(e.target) && !profileDropdown.contains(e.target)) {
            profileDropdown.classList.add("hidden");
        }
    });
});
