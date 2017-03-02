var fs = require("fs");

var g_hasHeightMap = false;
var g_hasMetaData = false;

global.selectionActive = false;
global.marqueeTopLeft = Object.freeze({x: 0, y: 0});
global.marqueeBottomRight = Object.freeze({x: 1, y: 1});

function ready(fn) {
    if (document.readyState != "loading"){
        fn();
    } else {
        document.addEventListener("DOMContentLoaded", fn);
    }
}

function updateFrame(elemId, dropResponse) {
    window.ondragover = function(e) { e.preventDefault(); return false; };
    window.ondrop = function(e) { e.preventDefault(); return false; };
    var holder = document.getElementById(elemId);
    holder.ondragover = function() { this.className = "hover"; return false; };
    holder.ondragleave = function() { this.className = ""; return false; };
    holder.ondrop = function(e) {
        e.preventDefault();
        for (var i = 0; i < e.dataTransfer.files.length; ++i) {
            dropResponse(e.dataTransfer.files[i].path);
        }
        this.className = "";
        return false;
    };
}

function verifyExtension(path, regexp) {
    var ext = path.split(".").pop();
    if (!ext.match(regexp)) {
	return false;
    }
    return true;
}

ready(() => {
    updateFrame("tiff-frame", (path) => {
	if (verifyExtension(path, "tif|tiff|TIF|TIFF")) {
	    global.backend.import_source_image(() => {
		g_hasHeightMap = true;
		if (g_hasMetaData) {
		    ready(enableNextButton);
		}
            }, path);
	} else {
	    window.alert("File extension invalid. Expected: tif or tiff or TIF or TIFF.");
	}
    });
});

function infoContainer() {
    this.block = 0;
    this.row = 0;
    this.column = 0;
    this.id = null;
    this.name = null;
}

function parseGal(fileData) {
    var lines = fileData.split("\n");
    var datahead = '\"Block\"	\"Row\"	\"Column\"	\"ID\"	\"Name\"';
    var n;
    for (n = 0; n < lines.length; ++n) {
	if (lines[n].trim() == datahead) {
	    ++n;
	    break;
	}
    }
    var galdata = [];
    var rowMax = 1;
    var colMax = 1;
    for (var k = n; k < lines.length; k++) {
	var temp = lines[k].trim().split(/\s+/);
	if (temp.length != 5) {
	    if (temp.length == 1) {
		if (temp == lines[k].trim()) {
		    continue;
		}
	    }
	    window.alert("gal parser failed to interpret line " + (k + 1));
	    continue;
	}

	empty_container = new infoContainer();
	
	temp[0] = parseInt(temp[0]);
	temp[1] = parseInt(temp[1]);
	temp[2] = parseInt(temp[2]);
	rowMax = Math.max(temp[1], rowMax);
	colMax = Math.max(temp[2], colMax);
	
	//PUT DATA INTO CONTAINER
	empty_container.block = temp[0];
	empty_container.row = temp[1];
	empty_container.column = temp[2];
	empty_container.id = temp[3];
	empty_container.name = temp[4];

	//PUT CONTAINER INTO ARRAY
	galdata.push(empty_container);
    }
    global.roiRows = rowMax;
    global.roiCols = colMax;
    global.rowStart = 1;
    global.colStart = 1;
    global.galData = new Array(rowMax);
    for (var i = 0; i < rowMax; ++i) {
	global.galData[i] = new Array(colMax);
    }
    for (var i = 0; i < galdata.length; ++i) {
	var row = galdata[i].row - 1;
	var col = galdata[i].column - 1;
	var infoElement = Object.freeze({
	    id: galdata[i].id.trim(),
	    name: galdata[i].name.trim()
	});
	global.galData[row][col] = infoElement;
    }
}

function importGalFile(path) {
    fs.readFile(path, (err, data) => {
	if (err) {
	    window.alert("Failed to load file: " + err);
	}
	parseGal("" + data);
	g_hasMetaData = true;
	if (g_hasHeightMap) {
	    ready(enableNextButton);
	}
    });
}

ready(() => {
    updateFrame("gal-frame", (path) => {
	if (verifyExtension(path, "gal")) {
	    importGalFile(path);
	} else {
	    window.alert("File extension invalid. Expected: gal");
	}
    });
});


$("#choose-tiff").on("change", function() {
    if (verifyExtension(this.value, "tif|tiff|TIF|TIFF")) {
	global.backend.import_source_image(() => {
	    g_hasHeightMap = true;
	    if (g_hasMetaData) {
		ready(enableNextButton);
	    }
        }, this.value);
    }
});

$("#choose-gal").on("change", function() {
    if (verifyExtension(this.value, "gal")) {
	importGalFile(this.value);
    }
});


function onNextPressed() {
    global.backend.provide_norm_preview(() => {
	window.location.href = "frontend/layouts/roi.html";
    });
}

function enableNextButton() {
    var nextButton = document.getElementById("index-next");
    nextButton.disabled = false;
}

function disableNextButton() {
    var nextButton = document.getElementById("index-next");
    nextButton.disabled = true;
}

ready(disableNextButton);
