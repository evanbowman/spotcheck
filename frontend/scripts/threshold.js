function ready(fn) {
    if (document.readyState == "complete") {
	fn();
    } else {
	document.addEventListener("DOMContentLoaded", fn);
    }
}

var g_selectionActive = false;
var g_marqueeRows = 1;
var g_marqueeCols = 1;
var g_marqueeTopLeft = Object.freeze({x: 0, y: 0});
var g_marqueeBottomRight = Object.freeze({x: 1, y: 1});
var g_dragging = false;

function getImgOffsetAsRatio(e, callerCtx) {
    var offset = $(callerCtx).offset();
    return Object.freeze({
	x: (e.pageX - (offset.left + g_imgDrawInfo.xstart)) / (g_imgDrawInfo.width),
	y: (e.pageY - (offset.top + g_imgDrawInfo.ystart)) / (g_imgDrawInfo.height)
    });
}

$("main").on("mousedown", "#thresh-preview", function(e) {
    g_marqueeTopLeft = getImgOffsetAsRatio(e, this);
    g_dragging = true;
    g_selectionActive = true;
});

$("main").on("mousemove", "#thresh-preview", function(e) {
    if (g_dragging) {
	g_marqueeBottomRight = getImgOffsetAsRatio(e, this);
	repaintPreview();
    }
});

$("main").on("mouseleave", "#thresh-preview", function(e) {
    if (g_dragging) {
	g_marqueeBottomRight = getImgOffsetAsRatio(e, this);
	g_dragging = false;
    }
});

$("main").on("mouseup", "#thresh-preview", function(e) {
    if (g_dragging) {
	g_marqueeBottomRight = getImgOffsetAsRatio(e, this);
	g_dragging = false;
	enableAnalyzeButton();
    }
});

function getTbValueAsInt(callerCtx) {
    if (callerCtx.value.length == 0) {
	return 0;
    }
    var value = parseInt(callerCtx.value);
    if (value <= 0) {
	return 1;
    }
    return value;
}

$("#roi-rows-tb").on("input", function() {
    g_marqueeRows = getTbValueAsInt(this);
    repaintPreview();
});

$("#roi-cols-tb").on("input", function() {
    g_marqueeCols = getTbValueAsInt(this);
    repaintPreview();
});

$("#thresh-preview").on("dragstart", function(event) { event.preventDefault(); });

var g_imgDrawInfo = {
    xstart: 0, ystart: 0, width: 0, height: 0, scale: 1
}

function renderBackendImgOutput(canvas, ctx) {
    var img = document.getElementById("thresh-img");
    var wScale = 1.0, hScale = 1.0;
    if (img.width > canvas.width) {
	wScale = canvas.width / img.width;
    }
    if (img.height > canvas.height) {
	hScale = canvas.height / img.height;
    }
    var scale = Math.min(hScale, wScale);
    g_imgDrawInfo.width = img.width * scale;
    g_imgDrawInfo.height = img.height * scale;
    g_imgDrawInfo.xstart = (canvas.width - g_imgDrawInfo.width) / 2;
    g_imgDrawInfo.ystart = (canvas.height - g_imgDrawInfo.height) / 2;
    g_imgDrawInfo.scale = scale;
    ctx.drawImage(img, g_imgDrawInfo.xstart, g_imgDrawInfo.ystart,
		  g_imgDrawInfo.width, g_imgDrawInfo.height);
}

function maskNonSelected(canvas, ctx, selX, selY, selWidth, selHeight) {
    var maskCanvas = document.getElementById("thresh-canvas-mask");
    maskCanvas.width = canvas.width;
    maskCanvas.height = canvas.height;
    var maskCtx = maskCanvas.getContext("2d");
    maskCtx.clearRect(0, 0, canvas.width, canvas.height);
    maskCtx.fillStyle = "rgba(0, 0, 0, 0.25)";
    maskCtx.fillRect(g_imgDrawInfo.xstart, g_imgDrawInfo.ystart,
		     g_imgDrawInfo.width, g_imgDrawInfo.height);
    maskCtx.fill();
    maskCtx.clearRect(selX, selY, selWidth, selHeight);
    ctx.drawImage(maskCanvas, 0, 0);
}

function renderSelectionGrid(canvas, ctx) {
    var rectStartX = g_imgDrawInfo.xstart + g_imgDrawInfo.width * g_marqueeTopLeft.x;
    var rectEndX = g_imgDrawInfo.xstart + g_imgDrawInfo.width * g_marqueeBottomRight.x;
    var rectStartY = g_imgDrawInfo.ystart + g_imgDrawInfo.height * g_marqueeTopLeft.y;
    var rectEndY = g_imgDrawInfo.ystart + g_imgDrawInfo.height * g_marqueeBottomRight.y;
    ctx.beginPath();
    ctx.lineWidth = "6";
    ctx.strokeStyle = "#EEEEEE";
    var rectWidth = rectEndX - rectStartX;
    var rectHeight = rectEndY - rectStartY;
    maskNonSelected(canvas, ctx, rectStartX, rectStartY, rectWidth, rectHeight);
    ctx.rect(rectStartX, rectStartY, rectWidth, rectHeight);
    ctx.stroke();
    ctx.closePath();
    var rowDivisions = g_marqueeRows - 1;
    var colDivisions = g_marqueeCols - 1;
    var rowRawSize = rectHeight / g_marqueeRows;
    var colRawSize = rectWidth / g_marqueeCols;
    ctx.save();
    ctx.lineWidth = "1";
    if (rowDivisions > 0) {
	for (var i = 0; i < rowDivisions; ++i) {
	    ctx.beginPath();
	    ctx.moveTo(rectStartX, rectStartY + (i + 1) * rowRawSize);
	    ctx.lineTo(rectEndX, rectStartY + (i + 1) * rowRawSize);
	    ctx.stroke();
	    ctx.closePath();
	}
    }
    if (colDivisions > 0) {
	for (var i = 0; i < colDivisions; ++i) {
	    ctx.beginPath();
	    ctx.moveTo(rectStartX + (i + 1) * colRawSize, rectStartY);
	    ctx.lineTo(rectStartX + (i + 1) * colRawSize, rectEndY);
	    ctx.stroke();
	    ctx.closePath();
	}
    }
    ctx.restore();
}

function repaintPreview() {
    var canvas = document.getElementById("thresh-canvas");
    var ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    renderBackendImgOutput(canvas, ctx);
    if (g_selectionActive) {
	renderSelectionGrid(canvas, ctx);
    }
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
    var startx = g_marqueeTopLeft.x;
    var starty = g_marqueeTopLeft.y;
    var dispx = (g_marqueeBottomRight.x - g_marqueeTopLeft.x) / g_marqueeCols;
    var dispy = (g_marqueeBottomRight.y - g_marqueeTopLeft.y) / g_marqueeRows;
    var gridSectors = [];
    for (var i = 0; i < g_marqueeRows; ++i) {
	for (var j = 0; j < g_marqueeCols; ++j) {
	    global.backend.add_target(i, j,
				      startx + dispx * j,
				      starty + dispy * i,
				      startx + dispx * (j + 1),
				      starty + dispy * (i + 1));
	}
    }
    global.backend.launch_analysis(() => {
	if (!global.backend.is_busy()) {
	    window.location.href = "frontend/layouts/results.html";
	}
    });
}

function onBackPressed() {
    window.location.href = "index.html";
}

window.onload = onWindowUpdate;

window.onresize = onWindowUpdate;

function disableAnalyzeButton() {
    var analyzeButton = document.getElementById("analyze");
    analyzeButton.disabled = true;
}

function enableAnalyzeButton() {
    var analyzeButton = document.getElementById("analyze");
    analyzeButton.disabled = false;
}

ready(disableAnalyzeButton);
