var fs = require("fs");

function ready(fn) {
    if (document.readyState != "loading"){
        fn();
    } else {
        document.addEventListener("DOMContentLoaded", fn);
    }
}

function makeHeader(resultsJSON) {
    var header = document.getElementById("header");
    var span = document.createElement("span");
    for (var key in resultsJSON[0]) {
	var h1 = document.createElement("H1");
	h1.innerHTML = key;
	span.appendChild(h1);
    }
    header.appendChild(span);
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
    var data = fs.readFileSync("./frontend/temp/results.json", "utf8");
    var resultsJSON = JSON.parse(data);
    populateTable(resultsJSON);
    makeHeader(resultsJSON);
}

ready(onStart());
