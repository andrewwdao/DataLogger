"use strict";

function togglePageOverlay() {
    document.getElementById("page-overlay").classList.toggle("show");
}

function toggleNodeDetailModal() {
    togglePageOverlay();
    document.getElementById("page-overlay").onclick = () => {toggleNodeDetailModal()};
    document.getElementById("node-detail-modal").classList.toggle("show");
}

function toggleNav() {
    document.getElementById("nav").classList.toggle("show");
}

function toggleNavDropdown(clicked) {
    clicked.classList.toggle("show");
}