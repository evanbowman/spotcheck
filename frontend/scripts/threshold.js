function ready(fn) {
    if (document.readyState == "complete") {
	fn();
    } else {
	document.addEventListener("DOMContentLoaded", fn);
    }
}

function onNextPressed() {
    global.backend.launch_analysis(function() {
	if (!global.backend.is_busy()) {
	    window.location.href = "frontend/layouts/analysis.html";
	}
    });
}

function onBackPressed() {
    window.location.href = "roi.html"
}

$("#thresh-slider").on("input", function() {
    ready(() => {
	var tb = document.getElementById("thresh-textbox");
	tb.value = this.value;
    });
});

var thumbs = [];

function refreshGalleryThumb(row, col) {
    var norm = thumbs[row][col].norm;
    var contour = thumbs[row][col].contour;
    var src1 = norm.src;
    var src2 = contour.src;
    var pos1 = src1.indexOf("?");
    var pos2 = src2.indexOf("?");
    if (pos1 >= 0) {
	src1 = src1.substr(0, pos1);
    }
    if (pos2 >= 0) {
	src2 = src2.substr(0, pos2);
    }
    var data = new Date();
    norm.src = src1 + "?v=" + data.getTime();
    contour.src = src2 + "?v=" + data.getTime();
}

function unselAll() {
    for (var i = 0; i < thumbs.length; ++i) {
	for (var j = 0; j < thumbs[i].length; ++j) {
	    thumbs[i][j].div.className = "gallery-box-unsel";
	}
    }
}

function init() {
    for (var rowNum = 0; rowNum < global.roiRows; ++rowNum) {
	row = [];
	thumbs.push(row);
	var normPathPrefix = "../temp/norm";
	var contourPathPrefix = "../temp/mask";
	for (var colNum = 0; colNum < global.roiCols; ++colNum) {
	    var norm = document.createElement("img");
	    var contour = document.createElement("img");
	    norm.src = normPathPrefix + rowNum + colNum + ".png";
	    contour.src = contourPathPrefix + rowNum + colNum + ".png";
	    var div = document.createElement("div");
	    div.appendChild(norm);
	    div.appendChild(contour);
	    row.push({div: div, norm: norm, contour: contour});
	    div.onclick = (function(tmpRow, tmpCol) {
		return function() {
		    unselAll();
		    this.className = "gallery-box-sel";
		    document.getElementById("thresh-slider").oninput = function() {
			global.backend.update_target_thresh(function() {
			    refreshGalleryThumb(tmpRow, tmpCol);
			}, tmpRow, tmpCol, this.value);
		    }
		    document.getElementById("thresh-textbox").onchange = function() {
			global.backend.update_target_thresh(function() {
			    refreshGalleryThumb(tmpRow, tmpCol);
			    document.getElementById("thresh-slider").value = this.value;
			}, tmpRow, tmpCol, this.value);
		    }
		    var currentThresh = global.backend.get_target_thresh(tmpRow, tmpCol);
		    document.getElementById("thresh-slider").value = currentThresh;
		    document.getElementById("thresh-textbox").value = currentThresh;
		}
	    })(rowNum, colNum);
	    div.className = "gallery-box-unsel";
	    $("#gallery").append(div);
	}
	var currentThresh = global.backend.get_target_thresh(0, 0);
	var threshSlider = document.getElementById("thresh-slider");
	var threshTextbox = document.getElementById("thresh-textbox");
	threshSlider.value = currentThresh;
	threshTextbox.value = currentThresh;
	thumbs[0][0].div.click();
    }
}

ready(init);
