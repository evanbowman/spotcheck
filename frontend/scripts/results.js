var fs = require("fs");

function ready(fn) {
    if (document.readyState != "loading") {
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

function verifyValidData(JSONObj) {
    if (!JSONObj.hasOwnProperty("row") ||
	!JSONObj.hasOwnProperty("col")) {
	throw "Results data has invalid format";
    }
}

function inflateTbody(resultsJSON, table) {
    var tableBody = document.createElement('TBODY');
    table.appendChild(tableBody);
    for (var i = 0; i < resultsJSON.length; ++i) {
	var obj = resultsJSON[i];
	verifyValidData(obj);
	var tr = document.createElement('TR');
	tableBody.appendChild(tr);
	var cellno = 0;
	var rowId, colId;
	rowId = parseInt(obj["row"]) + (global.rowStart - 1);
	colId = parseInt(obj["col"]) + (global.colStart - 1);
	var nameCell = tr.insertCell(cellno++);
	var idCell = tr.insertCell(cellno++);
	if (rowId < global.galData.length) {
	    if (colId < global.galData[rowId].length) {
		try {
		    nameCell.innerHTML = global.galData[rowId][colId].name;
		} catch (err) {
		    window.alert("missing name field for " + rowId + " " + colId);
		}
		idCell.innerHTML = global.galData[rowId][colId].id;
	    } else {
		nameCell.innerHTML = " ";
		idCell.innerHTML = " ";
	    }
	}
	for (var key in obj) {
	    if (key == "row") {
		var cell = tr.insertCell(cellno++);
		cell.innerHTML = rowId + 1;
	    } else if (key == "col") {
		var cell = tr.insertCell(cellno++);
		cell.innerHTML = colId + 1;
	    } else if (obj.hasOwnProperty(key)) {
		var cell = tr.insertCell(cellno++);
		cell.innerHTML = obj[key];
	    }
	}
    }
    return tableBody;
}

function deselectAllTheadCells() {
    var table = document.getElementById("proxy-thead");
    for (var i = 0, cell; cell = table.rows[0].cells[i]; i++) {
	cell.className = "";
    }
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
	cell.onclick = (function(cellno, tab, head) {
	    return function() {
		for (var i = 0, cell; cell = head.rows[0].cells[i]; i++) {
		    cell.className = "";
		}
		head.rows[0].cells[cellno].className = "selected-thead";
		sortTable(tab.tBodies[0], cellno);
	    }
	})(i, table, proxyTable);
    }
    inflateTbody(resultsJSON, table);
    proxyTable.rows[0].cells[0].click();
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

function parseTargets(resultsJSON) {
    var targets = [];
    for (var idx in resultsJSON) {
	targets.push({row: resultsJSON[idx]["row"], col: resultsJSON[idx]["col"]});
    }
    return targets;
}

function runUserMetrics(resultsJSON, configJSON) {
    const vm = require("vm");
    const targets = parseTargets(resultsJSON);
    const sandbox = {ctx: {src: null, mask: null, out: null}};
    vm.createContext(sandbox);
    for (key in configJSON) {
	if (!configJSON[key]["builtin"] && configJSON[key]["enabled"]) {
	    const script = new vm.Script(configJSON[key]["src"]);
	    for (idx in targets) {
		sandbox["ctx"]["src"] = targets[idx]["row"];
		sandbox["ctx"]["mask"] = targets[idx]["col"];
		script.runInContext(sandbox);
	    }
	}
    }
}

function init() {
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
    global.backend.write_results_JSON(function() {
	fs.readFile("./frontend/temp/results.json", (err, data) => {
	    if (err) {
		window.alert(err);
		return;
	    }
	    var resultsJSON = JSON.parse(data);
	    var configJSON = JSON.parse(fs.readFileSync(require("os").homedir() + "/.spotcheck.json"));
	    runUserMetrics(resultsJSON, configJSON);
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

var g_chart;

function onCreateChartPressed() {
    window.location.href = "visualization.html";
}

ready(init);
