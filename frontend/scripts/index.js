var g_hasHeightMap = false;
var g_hasMetaData = false;

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

ready(() => {
    updateFrame("gal-frame", (path) => {
	if (verifyExtension(path, "gal")) {
	    global.backend.import_source_gal(() => {
		g_hasMetaData = true;
		if (g_hasHeightMap) {
		    ready(enableNextButton);
		}
            }, path);
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
	global.backend.import_source_gal(() => {
	    g_hasMetaData = true;
	    if (g_hasHeightMap) {
		ready(enableNextButton);
	    }
        }, this.value);
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
