var fs = require("fs");

function ready(fn) {
    if (document.readyState != "loading"){
        fn();
    } else {
        document.addEventListener("DOMContentLoaded", fn);
    }
}

function inflateThead(resultsJSON, table) {
    var obj = resultsJSON[0];
    var header = table.createTHead();
    var row = header.insertRow(0);
    var cellno = 0;
    var cell = row.insertCell(cellno++);
    cell.innerHTML = "name";
    cell = row.insertCell(cellno++);
    cell.innerHTML = "id";
    for (var key in obj) {
	cell = row.insertCell(cellno++);
	cell.innerHTML = key;
    }
    return header;
}

function inflateTbody(resultsJSON, table) {
    var tableBody = document.createElement('TBODY');
    table.appendChild(tableBody);
    for (var i = 0; i < resultsJSON.length; ++i) {
	var obj = resultsJSON[i];
	var tr = document.createElement('TR');
	tableBody.appendChild(tr);
	var cellno = 0;
	var rowId, colId;
	if (obj.hasOwnProperty("row")) {
	    rowId = parseInt(obj["row"]);
	} else {
	    window.alert("Internal error: malformed results JSON, missing field 'col'");
	    return;
	}
	if (obj.hasOwnProperty("col")) {
	    colId = parseInt(obj["col"]);
	} else {
	    window.alert("Internal error: malformed results JSON, missing field 'row'");
	    return;
	}
	var nameCell = tr.insertCell(cellno++);
	var idCell = tr.insertCell(cellno++);
	if (rowId < global.galData.length) {
	    if (colId < global.galData[rowId].length) {
		nameCell.innerHTML = global.galData[rowId][colId].name;
		idCell.innerHTML = global.galData[rowId][colId].id;
	    } else {
		nameCell.innerHTML = " ";
		idCell.innerHTML = " ";
	    }
	}
	for (var key in obj) {
	    if (obj.hasOwnProperty(key)) {
		var cell = tr.insertCell(cellno++);
		cell.innerHTML = obj[key];
	    }
	}
    }
    return tableBody;
}

function populateTable(resultsJSON) {
    var table = document.getElementById("results-table");
    var proxyTable = document.getElementById("proxy-thead");
    if (resultsJSON.length > 0) {
	var header = inflateThead(resultsJSON, table);
	header.className = "hidden";
	var fakeHeader = inflateThead(resultsJSON, proxyTable);
    } else {
	return;
    }
    for (var i = 0, cell; cell = proxyTable.rows[0].cells[i]; i++) {
	cell.onclick = (function(cellno, tab) {
	    return function() {
		sortTable(tab.tBodies[0], cellno);
	    }
	})(i, table);
    }
    inflateTbody(resultsJSON, table);
    var proxyBod = inflateTbody(resultsJSON, proxyTable);
    proxyBod.className = "hidden";
    sortTable(table.tBodies[0], 0);
}

function sortTable(tbl, bycell) {
    var store = [];
    for (var i = 0, len = tbl.rows.length; i < len; i++) {
        var row = tbl.rows[i];
        var sortnr = parseFloat(row.cells[bycell].textContent || row.cells[bycell].innerText);
        if(!isNaN(sortnr)) store.push([sortnr, row]);
    }
    store.sort(function(x, y) {
        return x[0] - y[0];
    });
    for (var i = 0, len = store.length; i < len; i++) {
        tbl.appendChild(store[i][1]);
    }
    store = null;
}

function init() {
    global.backend.write_results_JSON(function() {
	fs.readFile("./frontend/temp/results.json", (err, data) => {
	    if (err) {
		window.alert(err);
		return;
	    }
	    var resultsJSON = JSON.parse(data);
	    populateTable(resultsJSON);
	});
    });
}

function onBackPressed() {
    window.location.href = "threshold.html";
}

function onExportPressed() {
    document.getElementById("saveas").click();
}

document.getElementById("saveas").onchange = function() {
    var csvStr = "";
    var table = document.getElementById("results-table");
    for (var i = 0, row; row = table.rows[i]; i++) {
	for (var j = 0, col; col = row.cells[j]; j++) {
	    csvStr += col.innerHTML + ",";
	}
	csvStr += "\n";
    }
    var fname = this.value;
    fs.writeFile(fname, csvStr, (err) => {
	if (err) {
	    throw err;
	}
    });
}

ready(init());
