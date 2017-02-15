function ready(fn) {
    if (document.readyState == "complete") {
	fn();
    } else {
	document.addEventListener("DOMContentLoaded", fn);
    }
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

var marqueeTopLeft;
var marqueeBottomRight;
var dragging = false;

function getImgOffsetAsRatio(e, callerThis) {
    var offset = $(callerThis).offset();
    return Object.freeze({
	x: (e.pageX - offset.left) / $(callerThis).width(),
	y: (e.pageY - offset.top) / $(callerThis).height()
    });
}

$("main").on("mousedown", "#thresh-preview", function(e) {
    marqueeTopLeft = getImgOffsetAsRatio(e, this);
    dragging = true;
});

$("main").on("mousemove", "#thresh-preview", function(e) {
    if (dragging) {
	marqueeBottomRight = getImgOffsetAsRatio(e, this);
    }
});

$("main").on("mouseleave", "#thresh-preview", function(e) {
    if (dragging) {
	marqueeBottomRight = getImgOffsetAsRatio(e, this);
	window.alert("Start: " + marqueeTopLeft.x + ", " + marqueeTopLeft.y +
		     "\nStop: " + marqueeBottomRight.x + ", " + marqueeBottomRight.y);
	dragging = false;
    }
});

$("main").on("mouseup", "#thresh-preview", function(e) {
    if (dragging) {
	marqueeBottomRight = getImgOffsetAsRatio(e, this);
	window.alert("Start: " + marqueeTopLeft.x + ", " + marqueeTopLeft.y +
		     "\nStop: " + marqueeBottomRight.x + ", " + marqueeBottomRight.y);
	dragging = false;
    }
})

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

$("#roi-textbox").on("change", function() {
    var values = this.value.split(",");
    if (values.length != 4) {
	window.alert("Invalid input");
	return;
    }
    values.forEach((val) => {
	if (val < 0 || val > 100) {
	    window.alert("Invalid input");
	    return;
	}
    });
    global.backend.set_roi(updateThresholdImg, values, global.threshRenderCircles);
});

$("#thresh-preview").on("dragstart", function(event) { event.preventDefault(); });

function onToggleOverlayPressed() {
    ready(() => {
	global.threshRenderCircles = !global.threshRenderCircles;
	var slider = document.getElementById("thresh-slider");
	global.backend.set_threshold(updateThresholdImg, slider.value,
				     global.threshRenderCircles);
    });
}

function showBackendImgOutput(canvas, ctx) {
    var img = document.getElementById("thresh-img");
    var wScale = 1.0, hScale = 1.0;
    if (img.width > canvas.width) {
	wScale = canvas.width / img.width;
    }
    if (img.height > canvas.height) {
	hScale = canvas.height / img.height;
    }
    var scale = Math.min(hScale, wScale);
    var drawWidth = img.width * scale;
    var drawHeight = img.height * scale;
    var drawStartX = (canvas.width - drawWidth) / 2;
    var drawStartY = (canvas.height - drawHeight) / 2;
    ctx.drawImage(img, drawStartX, drawStartY, drawWidth, drawHeight);
}

function repaintPreview() {
    var canvas = document.getElementById("thresh-canvas");
    var ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    showBackendImgOutput(canvas, ctx);
}

function updateThresholdImg() {
    var img = document.getElementById("thresh-img");
    var src = img.src;
    var pos = src.indexOf('?');
    if (pos >= 0) {
	src = src.substr(0, pos);
    }
    var date = new Date();
    img.src = src + '?v=' + date.getTime();
}

document.getElementById("thresh-img").onload = function() {
    repaintPreview();
}

function onWindowUpdate() {
    ready(function() {
	var canvas = document.getElementById("thresh-canvas");
	canvas.width = $("#thresh-preview").parent().width();
	canvas.height = $("#thresh-preview").parent().height();
	repaintPreview();
    });
}

function onAnalyzePressed() {
    ready(() => {
	global.backend.launch_analysis(() => {
	    window.location.href = "frontend/layouts/results.html";
	});
    });
}

window.onload = onWindowUpdate;

window.onresize = onWindowUpdate;
