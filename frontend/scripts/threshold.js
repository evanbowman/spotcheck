function ready(fn) {
    if (document.readyState == "complete") {
	fn();
    } else {
	document.addEventListener("DOMContentLoaded", fn);
    }
}

function updateThresholdImg() {
    var img = document.getElementById("thresh-preview");
    var src = img.src;
    var pos = src.indexOf('?');
    if (pos >= 0) {
	src = src.substr(0, pos);
    }
    var date = new Date();
    img.src = src + '?v=' + date.getTime();
}

$("#thresh-slider").on("change", function() {
    ready(() => {
	var tb = document.getElementById("thresh-textbox");
	tb.value = this.value;
	global.backend.set_threshold(updateThresholdImg,
				     this.value, global.threshRenderCircles);
    });
});

$("#thresh-slider").on("input", function() {
    ready(() => {
	var tb = document.getElementById("thresh-textbox");
	tb.value = this.value;
    });
});

$("#thresh-textbox").on("change", function() {
    if (isNaN(this.value)) {
	window.alert("Input must be a number");
	return;
    }
    if (this.value < 1 || this.value > 255) {
	window.alert("Threshold values must within range: [1 , 255]");
	return;
    }
    ready(() => {
	var slider = document.getElementById("thresh-slider");
	slider.value = this.value;
	global.backend.set_threshold(updateThresholdImg,
				     this.value, global.threshRenderCircles);
    });
});

function onToggleOverlayPressed() {
    ready(() => {
	global.threshRenderCircles = !global.threshRenderCircles;
	var slider = document.getElementById("thresh-slider");
	global.backend.set_threshold(updateThresholdImg, slider.value,
				     global.threshRenderCircles);
    });
}

function onAnalyzePressed() {
    ready(() => {
	global.backend.launch_analysis(() => {
	    window.location.href = "frontend/layouts/results.html";
	});
    });
}
