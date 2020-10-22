"use strict";

function toggleSwitch(sw) {
    sw.classList.toggle('active');
}

function toggleNav() {
    document.getElementById("nav").classList.toggle("show");
}

function toggleNavDropdown(clicked) {
    clicked.classList.toggle("show");
}