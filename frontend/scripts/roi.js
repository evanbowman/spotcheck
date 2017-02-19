function ready(fn) {
    if (document.readyState == "complete") {
	fn();
    } else {
	document.addEventListener("DOMContentLoaded", fn);
    }
}

var g_dragging = false;

function getImgOffsetAsRatio(e, callerCtx) {
    var offset = $(callerCtx).offset();
    return Object.freeze({
	x: (e.pageX - (offset.left + g_imgDrawInfo.xstart)) / (g_imgDrawInfo.width),
	y: (e.pageY - (offset.top + g_imgDrawInfo.ystart)) / (g_imgDrawInfo.height)
    });
}

$("main").on("mousedown", "#roi-preview", function(e) {
    global.marqueeTopLeft = getImgOffsetAsRatio(e, this);
    g_dragging = true;
    global.selectionActive = true;
});

$("main").on("mousemove", "#roi-preview", function(e) {
    if (g_dragging) {
	global.marqueeBottomRight = getImgOffsetAsRatio(e, this);
	repaintPreview();
    }
});

$("main").on("mouseleave", "#roi-preview", function(e) {
    if (g_dragging) {
	global.marqueeBottomRight = getImgOffsetAsRatio(e, this);
	g_dragging = false;
    }
});

$("main").on("mouseup", "#roi-preview", function(e) {
    if (g_dragging) {
	global.marqueeBottomRight = getImgOffsetAsRatio(e, this);
	g_dragging = false;
	enableNextButton();
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
    global.roiRows = getTbValueAsInt(this);
    repaintPreview();
});

$("#roi-cols-tb").on("input", function() {
    global.roiCols = getTbValueAsInt(this);
    repaintPreview();
});

$("#roi-preview").on("dragstart", function(event) { event.preventDefault(); });

var g_imgDrawInfo = {
    xstart: 0, ystart: 0, width: 0, height: 0, scale: 1
}

function renderBackendImgOutput(canvas, ctx) {
    var img = document.getElementById("roi-img");
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
    var maskCanvas = document.getElementById("roi-canvas-mask");
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
    var rectStartX = g_imgDrawInfo.xstart + g_imgDrawInfo.width * global.marqueeTopLeft.x;
    var rectEndX = g_imgDrawInfo.xstart + g_imgDrawInfo.width * global.marqueeBottomRight.x;
    var rectStartY = g_imgDrawInfo.ystart + g_imgDrawInfo.height * global.marqueeTopLeft.y;
    var rectEndY = g_imgDrawInfo.ystart + g_imgDrawInfo.height * global.marqueeBottomRight.y;
    ctx.beginPath();
    ctx.lineWidth = "6";
    ctx.strokeStyle = "#EEEEEE";
    var rectWidth = rectEndX - rectStartX;
    var rectHeight = rectEndY - rectStartY;
    maskNonSelected(canvas, ctx, rectStartX, rectStartY, rectWidth, rectHeight);
    ctx.rect(rectStartX, rectStartY, rectWidth, rectHeight);
    ctx.stroke();
    ctx.closePath();
    var rowDivisions = global.roiRows - 1;
    var colDivisions = global.roiCols - 1;
    var rowRawSize = rectHeight / global.roiRows;
    var colRawSize = rectWidth / global.roiCols;
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
    var canvas = document.getElementById("roi-canvas");
    var ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    renderBackendImgOutput(canvas, ctx);
    if (global.selectionActive) {
	renderSelectionGrid(canvas, ctx);
    }
}

function updateRoioldImg() {
    var img = document.getElementById("roi-img");
    var src = img.src;
    var pos = src.indexOf('?');
    if (pos >= 0) {
	src = src.substr(0, pos);
    }
    var date = new Date();
    img.src = src + '?v=' + date.getTime();
}

document.getElementById("roi-img").onload = function() {
    repaintPreview();
}

function onWindowUpdate() {
    ready(function() {
	var canvas = document.getElementById("roi-canvas");
	canvas.width = $("#roi-preview").parent().width();
	canvas.height = $("#roi-preview").parent().height();
	repaintPreview();
    });
}

function onNextPressed() {
    global.backend.clear_targets();
    var startx = global.marqueeTopLeft.x;
    var starty = global.marqueeTopLeft.y;
    var dispx = (global.marqueeBottomRight.x - global.marqueeTopLeft.x) / global.roiCols;
    var dispy = (global.marqueeBottomRight.y - global.marqueeTopLeft.y) / global.roiRows;
    var gridSectors = [];
    for (var i = 0; i < global.roiRows; ++i) {
	for (var j = 0; j < global.roiCols; ++j) {
	    global.backend.add_target(i, j,
				      startx + dispx * j,
				      starty + dispy * i,
				      startx + dispx * (j + 1),
				      starty + dispy * (i + 1),
				      0.0 /* TODO... */);
	}
    }
    global.backend.split_sectors(() => {
	if (!global.backend.is_busy()) {
	    window.location.href = "frontend/layouts/threshold.html";
	}
    });
}

function onBackPressed() {
    window.location.href = "index.html";
}

window.onload = onWindowUpdate;

window.onresize = onWindowUpdate;

function disableNextButton() {
    var nextButton = document.getElementById("next");
    nextButton.disabled = true;
}

function enableNextButton() {
    var nextButton = document.getElementById("next");
    nextButton.disabled = false;
}

ready(() => {
    if (!global.selectionActive) {
	disableNextButton();
    }
});

ready(() => {
    document.getElementById("roi-rows-tb").value = global.roiRows;
    document.getElementById("roi-cols-tb").value = global.roiCols;
});
