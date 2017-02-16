function ready(fn) {
    if (document.readyState == "complete") {
	fn();
    } else {
	document.addEventListener("DOMContentLoaded", fn);
    }
}

function disableNextButton() {
    var nextButton = document.getElementById("next");
    nextButton.disabled = true;
}

function enableNextButton() {
    var nextButton = document.getElementById("next");
    nextButton.disabled = false;
}

ready(disableNextButton);

function onNextPressed() {
    window.location.href = "frontend/layouts/analysis.html"
}

function onBackPressed() {
    window.location.href = "roi.html"
}

var thumbs = [];

function init() {
    for (var rowNum = 0; rowNum < global.roiRows; ++rowNum) {
	row = [];
	thumbs.push(row);
	var pathPrefix = "../temp/edit";
	for (var colNum = 0; colNum < global.roiCols; ++colNum) {
	    var img = document.createElement("img");
	    row.push(img);
	    img.src = pathPrefix + rowNum + colNum + ".png";
	    var div = document.createElement("div");
	    var span = document.createElement("span");
	    var slider = document.createElement("input");
	    slider.setAttribute("type", "range");
	    span.appendChild(img);
	    span.appendChild(slider);
	    div.appendChild(span);
	    slider.onchange = (function(tmpRow, tmpCol) {
		return function() {
		    window.alert(tmpRow + " " + tmpCol);
		}
	    })(rowNum, colNum);
	    img.style.cssText = "width: 100px; float: left; image-rendering: crisp-edges;";
	    $("main").append(div);
	}
    }
}

ready(init);
