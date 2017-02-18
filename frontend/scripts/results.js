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
    for (var i = 0; i < resultsJSON.length; ++i) {
	var obj = resultsJSON[i];
	var row = table.insertRow(i);
	var cellno = 0;
	for (var key in obj) {
	    if (obj.hasOwnProperty(key)) {
		var cell = row.insertCell(cellno);
		cell.innerHTML = obj[key];
		++cellno;
	    }
	}
    }
}

function onStart() {
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

ready(onStart());
