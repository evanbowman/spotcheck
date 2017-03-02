var fs = require("fs");

var g_resultsJSON;
var g_labels;

function ready(fn) {
    if (document.readyState != "loading") {
        fn();
    } else {
        document.addEventListener("DOMContentLoaded", fn);
    }
}

function createChartLabels() {
    var labelsSet = new Set();
    var ret = [];
    for (var i = 0; i < g_resultsJSON.length; ++i) {
	var obj = g_resultsJSON[i];
	var rowId = parseInt(obj["row"]) + (global.rowStart - 1);
	var colId = parseInt(obj["col"]) + (global.colStart - 1);
	if (rowId < global.galData.length) {
	    if (colId < global.galData[rowId].length) {
		try {
		    var name = global.galData[rowId][colId].name;
		    labelsSet.add(name);
		} catch (err) {
		    window.alert(err);
		    continue;
		}
	    }
	}
    }
    labelsSet.forEach(function(name) {
	ret.push(name);
    });
    return ret;
}

function createChartData(labels, locusFieldName) {
    var ret = [];
    for (var i = 0; i < labels.length; ++i) {
	var accum = 0;
	var count = 0;
	for (var j = 0; j < g_resultsJSON.length; ++j) {
	    var obj = g_resultsJSON[j];
	    var rowId = parseInt(obj["row"]) + (global.rowStart - 1);
	    var colId = parseInt(obj["col"]) + (global.colStart - 1);
	    if (global.galData[rowId][colId].name == labels[i]) {
		accum += obj[locusFieldName];
		++count;
	    }
	}
	ret.push(accum / count);
    }
    return ret;
}

function loadResults() {
    g_resultsJSON = JSON.parse(fs.readFileSync("./frontend/temp/results.json"));
}

function getAvailableFields() {
    var obj = g_resultsJSON[0];
    
}

function refreshChart(locusFieldName) {
    var data = createChartData(g_labels, locusFieldName);
    var g_chart = new Chart(chart.getContext("2d"), {
	type: 'bar',
	data: {
            labels: g_labels,
            datasets: [{
		label: "average " + locusFieldName,
		data: data,
		backgroundColor: 'rgba(127, 180, 184, 0.2)',
		borderColor: 'rgba(99, 152, 156, 1)',
		borderWidth: 1
            }]
	},
	options: {
	    responsive: true,
	    maintainAspectRatio: false,
            scales: {
		yAxes: [{
                    ticks: {
			beginAtZero:true
                    }
		}]
            }
	}
    });
}

function init() {
    loadResults();
    getAvailableFields();
    g_labels = createChartLabels();
    refreshChart("volume");
    document.getElementById("saveas").onchange = function() {
	var fname = this.value;
	var imgData = document.getElementById("chart").toDataURL('image/png');
	var data = imgData.replace(/^data:image\/\w+;base64,/, "");
	var buf = new Buffer(data, 'base64');
	fs.writeFile(fname, buf, (err) => {
	    if (err) {
		window.alert(err);
	    }
	});
    };
}

ready(init);

function onBackPressed() {
    window.location.href = "results.html";
}

function onExportPressed() {
    document.getElementById("saveas").click();
}
