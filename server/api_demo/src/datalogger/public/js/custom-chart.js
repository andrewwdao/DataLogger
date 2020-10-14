/* ======================================================
				    	 CHART
=======================================================*/

function customTooltip(tooltipModel) {
    // Tooltip Element
    var tooltipEl = document.getElementById('chartjs-tooltip');

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
        var titleLines = tooltipModel.title || [];
        var bodyLines = tooltipModel.body.map(getBody);

        var innerHtml = '<thead>';

        titleLines.forEach(function(title) {
            innerHtml += '<tr><th>' + title + '</th></tr>';
        });
        innerHtml += '</thead><tbody>';

        bodyLines.forEach(function(body, i) {
            var colors = tooltipModel.labelColors[i];
            var style = 'background:' + colors.backgroundColor;
            style += '; border-color:' + colors.borderColor;
            style += '; border-width: 2px';
            var span = '<span style="' + style + '"></span>';
            innerHtml += '<tr><td>' + span + body + '</td></tr>';
        });
        innerHtml += '</tbody>';

        var tableRoot = tooltipEl.querySelector('table');
        tableRoot.innerHTML = innerHtml;
    }

    // `this` will be the overall tooltip
    var position = this._chart.canvas.getBoundingClientRect();

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


/* ======================================================
				        MODAL CHART
=======================================================*/

var ctx = document.getElementById('chart').getContext('2d');

var chartData = [];
var chartData2 = [];
var chartLabels = [];
for (let i = 0; i < 200; i++) {
    chartData.push(Number((Math.random()*0.3 + 1).toFixed(1)));
    chartData2.push(Number((Math.random()*150 + 10).toFixed(1)));
    chartLabels.push(new Date().toLocaleTimeString());
}

var data = {
    labels: chartLabels,
    datasets: [
        {
            label: 'P02',
            data: chartData,
            // backgroundColor: 'rgba(0, 0, 0, 0)',
            backgroundColor: '#d8e3f0',
            borderColor: '#1e39e8',
            borderWidth: 1,
            lineTension: 0.1,
            pointRadius: 0,
            yAxisID: 'P02',
            fill: false,
        },
        {
            label: 'Flow',
            data: chartData2,
            // backgroundColor: 'rgba(0, 0, 0, 0)',
            backgroundColor: '#edb4da',
            borderColor: '#e81ea5',
            borderWidth: 1,
            lineTension: 0.1,
            pointRadius: 0,
            yAxisID: 'Flow',
            fill: false,
        }
    ]
}

var options = {
    responsive: true,
    maintainAspectRatio: false,
    tooltips: {
        // enabled: false,
        // custom: customTooltip,
        mode: 'nearest',
        intersect: false,
    },
    hover: {
      mode: 'nearest',
      intersect: false
    },
    scales: {
        yAxes: [
            {
                id: 'P02',
                position: 'left',
                ticks: {
                    beginAtZero: true,
                    suggestedMax: 2.5,
                }
            },
            {
                id: 'Flow',
                position: 'right',
                ticks: {
                    beginAtZero: true,
                    suggestedMax: 180,
                }
            }
        ],
        xAxes: [{
            ticks: {
                maxRotation: 0,
                autoSkipPadding: 100
            }
        }]
    },

    onHover: function onHover (evt, activeElements) {
        var datasetIndex;
        if (!activeElements || !activeElements.length) {
            datasetIndex = -1;
        } else {
            datasetIndex = activeElements[0]._datasetIndex;
        }

        for (let i = 0; i < this.data.datasets.length; i++) {
            if (datasetIndex != i)
                this.data.datasets[i].borderWidth = 1;
            else
                this.data.datasets[i].borderWidth = 2;
        }
        this.update();
    },
}

var chartConfig = {
    type: 'line',
    data: data,
    options: options,
}

var myLineChart = new Chart(ctx, chartConfig);