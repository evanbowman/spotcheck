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

ready(() => {
    updateFrame("tiff-frame", (path) => {
        var ext = path.split(".").pop();
        if (ext != "tiff") {
            window.alert("Error: expected tiff file");
        } else {
            global.backend.import_source_image(() => {
                window.alert("Import complete...");
		g_hasHeightMap = true;
		if (g_hasMetaData) {
		    ready(enableNextButton);
		}
            }, path);
        }
    });
});

ready(() => {
    updateFrame("gal-frame", (path) => {
        var ext = path.split(".").pop();
        if (ext != "gal") {
            window.alert("Error: expected gal file");
        } else {
            global.backend.import_source_gal(() => {
                window.alert("Import complete...");
		g_hasMetaData = true;
		if (g_hasHeightMap) {
		    ready(enableNextButton);
		}
            }, path);
        }
    });
});

function onNextPressed() {
    global.backend.set_threshold(() => {
        window.location.href = "frontend/layouts/threshold.html";
    }, 127, global.threshRenderCircles);
}

function enableNextButton() {
    var nextButton = document.getElementById("index-next");
    nextButton.disabled = false;
}

function disableNextButton() {
    var nextButton = document.getElementById("index-next");
    nextButton.disabled = true;
}

ready(disableNextButton());
