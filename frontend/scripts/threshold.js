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
    var pathPrefix = "../temp/edit";
    for (var i = 0; i < global.roiRows; ++i) {
	row = [];
	thumbs.push(row);
	for (var j = 0; j < global.roiCols; ++j) {
	    row.push(img);
	    var img = document.createElement("img");
	    img.src = pathPrefix + i + j + ".png";
	    var div = document.createElement("div");
	    var span = document.createElement("span");
	    var slider = document.createElement("input");
	    slider.setAttribute("type", "range");
	    span.appendChild(img);
	    span.appendChild(slider);
	    div.appendChild(span);
	    slider.setAttribute("onchange", function() {
		window.alert(i + " " + j);
	    });
	    img.style.cssText = "width: 100px; float: left; image-rendering: crisp-edges;";
	    $("main").append(div);
	}
    }
}

ready(init);
