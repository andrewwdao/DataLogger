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


/* ======================================================
				        MODAL CHART
=======================================================*/

let chartLineColors = ['#1e39e8', '#e81ea5'];
let chartBackgroundColors = ['#d8e3f0', '#edb4da'];

function createChart(data) {
    let chartContainer = document.getElementById('chart-container');
    chartContainer.innerHTML = '';

    let chartLabels = [];
    let chartData = {};

    Object.keys(data[0]['payload']).forEach(field => chartData[field] = []);

    data.forEach(record => {
        chartLabels.push(new Date(record['timestamp']*1000).toLocaleString());
        Object.keys(record['payload']).forEach(field => {
            chartData[field].push(record['payload'][field]);
        });
    });

    let colorIndex = 0;
    Object.keys(data[0]['payload']).forEach(field => {
        let myChart = document.createElement('canvas');
        myChart.className = 'chart';
        let ctx = myChart.getContext('2d');
        chartContainer.appendChild(myChart);

        let options = {
            responsive: true,
            maintainAspectRatio: false,
            tooltips: {
                mode: 'nearest',
                intersect: false,
            },
            hover: {
            mode: 'nearest',
            intersect: false
            },
            scales: {
                yAxes: [{
                    id: field,
                    position: 'left',
                    ticks: {
                        beginAtZero: true,
                        // suggestedMax: 2.5,
                    }
                }],
                xAxes: [{
                    ticks: {
                        maxRotation: 0,
                        autoSkipPadding: 100
                    }
                }]
            },
        }
        
        let chartConfig = {
            type: 'line',
            data: {
                labels: chartLabels,
                datasets: [{
                    label: field,
                    data: chartData[field],
                    // backgroundColor: 'rgba(0, 0, 0, 0)',
                    backgroundColor: chartBackgroundColors[colorIndex++%chartBackgroundColors.length],
                    borderColor: chartLineColors[colorIndex++%chartLineColors.length],
                    borderWidth: 1.5,
                    lineTension: 0.1,
                    pointRadius: 0,
                    yAxisID: field,
                    fill: false,
                }]
            },
            options: options,
        }

        let chart = new Chart(ctx, chartConfig);
    });

}


/* ======================================================
				    	 MODAL
=======================================================*/
let currentModalStationId = null;

function fetchHistoryData() {
    let startTimestamp = (document.getElementById("start-date-input").valueAsNumber + document.getElementById("start-time-input").valueAsNumber)/1000;
    let endTimestamp = (document.getElementById("end-date-input").valueAsNumber + document.getElementById("end-time-input").valueAsNumber)/1000;
    
    if (!isNaN(startTimestamp) && !isNaN(endTimestamp) && currentModalStationId) {
        const fetchUrl = `http://datalogger.ddns.net:8080/stations/${currentModalStationId}/data?start=${startTimestamp}&end=${endTimestamp}`;
        fetch(fetchUrl).then(res => {
            if (Math.floor(res.status/100) != 2) {
                console.log("Unexpected error fetching " + fetchUrl);
                return "";
            } else {
                return res.json();
            }
        }).then( data => {
            if (data && data.length) {
                createChart(data);
            }
        });
    } else {
        alert("Enter start and end date time");
    }
}

function toggleStationDetailModal(stationId) {
    currentModalStationId = stationId;
    togglePageOverlay();
    document.getElementById("page-overlay").onclick = () => {toggleStationDetailModal()};
    document.getElementById("node-detail-modal").classList.toggle("show");
}



/* ======================================================
				    	 MAP
=======================================================*/

function toggleDetailStationLabel(sw) {
    toggleSwitch(sw);
    document.getElementById('detail-node-label-flag').classList.toggle('active');
}

function updateNavStationSearchOptions(filter) {
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

function bringStationToFront(station, bringToFront) {
    let pos = mymap.latLngToLayerPoint(station.marker.getLatLng()).round();
    // bringToFront = bringToFront || station.marker.isPopupOpen();
    if (bringToFront) {
        station.marker.setZIndexOffset(5000 - pos.y);
    } else {
        station.marker.setZIndexOffset(pos.y);
    }
}

function fetchStationData(station) {
    const fetchUrl = HOST + `/stations/${station.id}/latest`;
    fetch(fetchUrl).then(res => res.json()).then(
        data => {
            station.data = data;
            updateStationPopup(station);
            if (!station.marker.isPopupOpen()) station.marker.openPopup();
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
            <div onclick="toggleStationDetailModal('${station.id}')" class="footer">Thêm thông tin</div>
        <div class="footer-float-fix">a</div>
    `);

}

function addStation(id, name, lat, lng, params) {
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
                    <div> ${params.length} thông số: ${params.map(param => param.toUpperCase()).join(", ")} </div>
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
        bringStationToFront(station, true);
    }).on("mouseout", () => {
        bringStationToFront(station, false);
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
                    addStation(station['station_code'], station['station_address'], station['station_latitude'], station['station_longitude'], station['station_params']);
                });
                updateNavStationSearchOptions();
            }
        }
    );
}

// addStation('CT33', 'D114. Lê Thị Hồng Gấm', 10.05, 105.74);
fetchStations();
windowResize();
window.addEventListener("resize", windowResize);
document.getElementById("nav-node-search-input").addEventListener("keyup", () => {
    updateNavStationSearchOptions(document.getElementById("nav-node-search-input").value);
});

document.getElementById("end-date-input").value = document.getElementById("start-date-input").value = new Date().toISOString().substr(0, 10);
document.getElementById("end-time-input").value = document.getElementById("start-time-input").value = new Date().toTimeString().substr(0,5);

// let popup = L.popup()
//     .setLatLng([10.05, 105.74])
//     .setContent('<p>Hello world!<br />This is a nice popup.</p>')
//     .openOn(mymap)
//     .openPopup();


/* ======================================================
				    	 CHART
=======================================================*/

function customTooltip(tooltipModel) {
    // Tooltip Element
    let tooltipEl = document.getElementById('chartjs-tooltip');

    // Create element on first render
    if (!tooltipEl) {
        tooltipEl = document.createElement('div');
        tooltipEl.id = 'chartjs-tooltip';
        tooltipEl.innerHTML = '<table></table>';
        document.body.appendChild(tooltipEl);
    }

    // Hide if no tooltip
    if (tooltipModel.opacity === 0) {
        tooltipEl.style.opacity = 0;
        return;
    }

    // Set caret Position
    tooltipEl.classList.remove('above', 'below', 'no-transform');
    if (tooltipModel.yAlign) {
        tooltipEl.classList.add(tooltipModel.yAlign);
    } else {
        tooltipEl.classList.add('no-transform');
    }

    function getBody(bodyItem) {
        return bodyItem.lines;
    }

    // Set Text
    if (tooltipModel.body) {
        let titleLines = tooltipModel.title || [];
        let bodyLines = tooltipModel.body.map(getBody);

        let innerHtml = '<thead>';

        titleLines.forEach(function(title) {
            innerHtml += '<tr><th>' + title + '</th></tr>';
        });
        innerHtml += '</thead><tbody>';

        bodyLines.forEach(function(body, i) {
            let colors = tooltipModel.labelColors[i];
            let style = 'background:' + colors.backgroundColor;
            style += '; border-color:' + colors.borderColor;
            style += '; border-width: 2px';
            let span = '<span style="' + style + '"></span>';
            innerHtml += '<tr><td>' + span + body + '</td></tr>';
        });
        innerHtml += '</tbody>';

        let tableRoot = tooltipEl.querySelector('table');
        tableRoot.innerHTML = innerHtml;
    }

    // `this` will be the overall tooltip
    let position = this._chart.canvas.getBoundingClientRect();

    // Display, position, and set styles for font
    tooltipEl.style.opacity = 1;
    tooltipEl.style.position = 'absolute';
    tooltipEl.style.left = position.left + window.pageXOffset + tooltipModel.caretX + 'px';
    tooltipEl.style.top = position.top + window.pageYOffset + tooltipModel.caretY + 'px';
    tooltipEl.style.fontFamily = tooltipModel._bodyFontFamily;
    tooltipEl.style.fontSize = tooltipModel.bodyFontSize + 'px';
    tooltipEl.style.fontStyle = tooltipModel._bodyFontStyle;
    tooltipEl.style.padding = tooltipModel.yPadding + 'px ' + tooltipModel.xPadding + 'px';
    tooltipEl.style.pointerEvents = 'none';
}