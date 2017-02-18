var fs = require("fs");

function ready(fn) {
    if (document.readyState != "loading"){
        fn();
    } else {
        document.addEventListener("DOMContentLoaded", fn);
    }
}

function populateTable(resultsJSON) {
    var table = document.getElementById("results-table");
    var galData = global.galData;
    var numRows = 0;
    for (var i = 0; i < galData.length; ++i) {
	for (var j = 0; j < galData[i].length; ++j) {
	    var row = table.insertRow(numRows++);
	    var cell = row.insertCell(0);
	    cell.innerHTML = galData[i][j].id;
	}
    }
}

function init() {
    fs.readFile("./frontend/temp/results.json", (err, data) => {
	if (err) {
	    window.alert(err);
	    return;
	}
	var resultsJSON = JSON.parse(data);
	populateTable(resultsJSON);
    });
}

function onBackPressed() {
    window.location.href = "threshold.html";
}

function onExportPressed() {
    // TODO
}

ready(init());
