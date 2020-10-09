"use strict";

const HOST = 'http://datalogger.ddns.net:8080';

let stations = [
    // {
    //     id: 'CT33',
    //     name: 'D114. Lê Thị Hồng Gấm',
    //     lat: 10.05,
    //     lng: 105.74,
    //     lastUpdate: '13/09/2020 20:19',
    //     data: {
    //         'AL (2)': 0.76,
    //     }
    // }
];

function windowResize() {
    let options = document.getElementById("nav-node-search-options");
    options.style.maxHeight = Math.min(window.innerHeight - options.offsetTop - 100, 300) + "px";
}

function togglePageOverlay() {
    document.getElementById("page-overlay").classList.toggle("show");
}

function toggleNodeDetailModal() {
    togglePageOverlay();
    document.getElementById("page-overlay").onclick = () => {toggleNodeDetailModal()};
    document.getElementById("node-detail-modal").classList.toggle("show");
}

function toggleDetailNodeLabel(sw) {
    toggleSwitch(sw);
    document.getElementById('detail-node-label-flag').classList.toggle('active');
}

function updateNavNodeSearchOptions(filter) {
    let options = document.getElementById("nav-node-search-options");
    options.innerHTML = '';
    stations.forEach(station => {
        let optionValue = `${station.id} - ${station.name}`;
        let i = 0;
        if (!filter || optionValue.indexOf(filter) != -1) {
            let option = document.createElement('div');
            option.className = 'option';
            option.innerHTML = optionValue;
            option.onclick = () => {
                station.marker.openPopup();
            };
            options.appendChild(option);
        }
    });

    if (!options.innerHTML) {
        options.innerHTML = `
            <div class="option">(Không có dữ liệu)</div>
        `;
    }
}


let mymap = L.map('main-map').setView([9.997626, 105.728626], 15);

L.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token={accessToken}', {
    // attribution: 'Map data &copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors, <a href="https://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, Imagery © <a href="https://www.mapbox.com/">Mapbox</a>',
    maxZoom: 18,
    id: 'mapbox/streets-v11',
    tileSize: 512,
    zoomOffset: -1,
    accessToken: 'pk.eyJ1IjoiaW5lZWR0b2hhdmVicmVha2Zhc3QyIiwiYSI6ImNrZjEzZTZ6dzEwM3YyenBoMWEwMDF0MzEifQ.bhiK1OFliX2mrcZDmTwq-w'
}).addTo(mymap);

function bringNodeToFront(station, bringToFront) {
    let pos = mymap.latLngToLayerPoint(station.marker.getLatLng()).round();
    // bringToFront = bringToFront || station.marker.isPopupOpen();
    if (bringToFront) {
        station.marker.setZIndexOffset(5000 - pos.y);
    } else {
        station.marker.setZIndexOffset(pos.y);
    }
}

function fetchStationData(station) {
    const fetchUrl = HOST + `/stations/${station.id}/lastest`;
    fetch(fetchUrl).then(res => res.json()).then(
        data => {
            station.data = data;
            updateStationPopup(station);
        }
    );
}

function updateStationPopup(station) {
    let stationData = `
        <div class="row">
            <span class="label">Thời điểm</span>
            <span class="value">
                ${station.data.timestamp ?
                    new Date(station.data.timestamp*1000).toLocaleDateString() + ' ' + new Date(station.data.timestamp*1000).toLocaleTimeString() :
                    'Chưa có thông tin'}
            </span>
        </div>
    `;
    for (let key in station.data.payload) {
        stationData += `
            <div class="row">
                <span class="label">${key}</span>
                <span class="value">${station.data.payload[key]}</span>
            </div>
        `;
    }

    station.marker._popup.setContent(`
        <div class="header">${station.id + ' - ' + station.name}</div>
            <!--<div class="row">
                <span class="label">Thời điểm</span>
                <span class="value">${station.lastUpdate ? station.lastUpdate : 'Chưa có thông tin'}</span>
            </div>-->
            ${stationData}
            <div onclick="toggleNodeDetailModal()" class="footer">Thêm thông tin</div>
        <div class="footer-float-fix">a</div>
    `);
}

function addStation(id, name, lat, lng) {
    let active = Math.random() >= 0.3;

    let station = {
        id: id,
        name: name,
        lat: lat,
        lng: lng,
        data: {
            'Thông số': 'Chưa có thông tin'
        }
    };
    stations.push(station);

    let markerIcon = L.divIcon({
        className: "marker",
        iconAnchor: [14, 28],
        popupAnchor: [0, -22],
        html: `
            <span class="marker-icon ${active ? 'active' : 'inactive'}"></span>
            <div class="node-label">
                <div class="header">${id} - ${name}</div>
                <div class="detail">
                    <div> 2 thông số: AL, P01 </div>
                    <div> Cập nhật: 08/10/2020 08:00 PM </div>
                    <div class="${active ? 'active' : 'inactive'}"> ${active ? 'Đang hoạt động' : 'Mất kết nối'} </div>
                </div>
            </div>
        `,
    });
    station.marker = L.marker([lat, lng], {
        icon: markerIcon
    }).addTo(mymap);

    station.marker.bindPopup("",
        {
            className: 'node-popup',
            offset: [0, -2],
            autoClose: false,
            // closeOnClick: false,
        }
    );
    updateStationPopup(station);

    station.marker.on('click', (e) => {
        fetchStationData(station);
    }).on("mouseover", () => {
        bringNodeToFront(station, true);
    }).on("mouseout", () => {
        bringNodeToFront(station, false);
    });
}

function fetchStations() {
    const fetchUrl = HOST + '/stations';
    fetch(fetchUrl).then(
        res => {
            if (Math.floor(res.status/100) != 2) {
                console.log("Unexpected error fetching " + fetchUrl);
                return "";
            } else {
                return res.json();
            }
        }
    ).then(
        fStations => {
            if (fStations) {
                fStations.forEach(station => {
                    addStation(station['station_code'], station['station_address'], station['station_latitude'], station['station_longitude']);
                });
                updateNavNodeSearchOptions();
            }
        }
    );
}

// addStation('CT33', 'D114. Lê Thị Hồng Gấm', 10.05, 105.74);
fetchStations();
windowResize();
window.addEventListener("resize", windowResize);
document.getElementById("nav-node-search-input").addEventListener("keyup", () => {
    updateNavNodeSearchOptions(document.getElementById("nav-node-search-input").value);
});

// var popup = L.popup()
//     .setLatLng([10.05, 105.74])
//     .setContent('<p>Hello world!<br />This is a nice popup.</p>')
//     .openOn(mymap)
//     .openPopup();