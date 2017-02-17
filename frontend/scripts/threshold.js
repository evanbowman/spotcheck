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

$("#thresh-slider").on("input", function() {
    ready(() => {
	var tb = document.getElementById("thresh-textbox");
	tb.value = this.value;
    });
});

var thumbs = [];

function refreshGalleryThumb(row, col) {
    var img = thumbs[row][col];
    var src = img.src;
    var pos = src.indexOf("?");
    if (pos >= 0) {
	src = src.substr(0, pos);
    }
    var data = new Date();
    img.src = src + "?v=" + data.getTime();
}

function unselAll() {
    for (var i = 0; i < thumbs.length; ++i) {
	for (var j = 0; j < thumbs[i].length; ++j) {
	    thumbs[i][j].className = "gallery-box-unsel";
	}
    }
}

function init() {
    for (var rowNum = 0; rowNum < global.roiRows; ++rowNum) {
	row = [];
	thumbs.push(row);
	var pathPrefix = "../temp/edit";
	for (var colNum = 0; colNum < global.roiCols; ++colNum) {
	    var img = document.createElement("img");
	    row.push(img);
	    img.src = pathPrefix + rowNum + colNum + ".png";
	    img.tabIndex = 0;
	    var div = document.createElement("div");
	    var span = document.createElement("span");
	    span.appendChild(img);
	    div.appendChild(span);
	    img.onclick = (function(tmpRow, tmpCol) {
		return function() {
		    unselAll();
		    this.className = "gallery-box-sel";
		    document.getElementById("thresh-slider").onchange = function() {
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
	    img.className = "gallery-box-unsel";
	    $("#gallery").append(div);
	}
	var currentThresh = global.backend.get_target_thresh(0, 0);
	var threshSlider = document.getElementById("thresh-slider");
	var threshTextbox = document.getElementById("thresh-textbox");
	threshSlider.value = currentThresh;
	threshTextbox.value = currentThresh;
	thumbs[0][0].click();
    }
}

ready(init);
