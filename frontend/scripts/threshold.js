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
    // global.backend.run_analysis(function() {
    window.location.href = "frontend/layouts/analysis.html"
    //})
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
	    $("main").append(img);
	}
    }
}

ready(init);
