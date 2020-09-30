
var mymap = L.map('main-map').setView([10.05, 105.74], 13);

L.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token={accessToken}', {
    // attribution: 'Map data &copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors, <a href="https://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, Imagery © <a href="https://www.mapbox.com/">Mapbox</a>',
    maxZoom: 18,
    id: 'mapbox/streets-v11',
    tileSize: 512,
    zoomOffset: -1,
    accessToken: 'pk.eyJ1IjoiaW5lZWR0b2hhdmVicmVha2Zhc3QyIiwiYSI6ImNrZjEzZTZ6dzEwM3YyenBoMWEwMDF0MzEifQ.bhiK1OFliX2mrcZDmTwq-w'
}).addTo(mymap);

var marker = L.marker([10.05, 105.74]).addTo(mymap);

marker.bindPopup(`
    <div class="header">CT33-D114. Lê Thị Hồng Gấm</div>
    <div class="row">
        <span class="label">Thời điểm</span>
        <span class="value">13/09/2020 20:19</span>
    </div>
    <div class="row">
        <span class="label">AL(2)</span>
        <span class="value">0.76(bar)</span>
    </div>
    <div onclick="toggleNodeDetailModal()" class="footer">Thêm thông tin</div>
    <div class="footer-float-fix">a</div>
`, {
    className: 'node-popup',
    offset: [0, -2],
}).openPopup();

marker.bindTooltip(`
    <b>CT33</b><br>
    <b>D114. Lê Thị Hồng Gấm</b>
`, {
    permanent: true,
    offset: [-15, 18],
    direction: 'bottom',
    className: 'node-tooltip'
});