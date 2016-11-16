function ready(fn) {
    if (document.readyState != 'loading'){
        fn();
    } else {
        document.addEventListener('DOMContentLoaded', fn);
    }
}

function updateFrame(elemId, dropResponse) {
    window.ondragover = function(e) { e.preventDefault(); return false; };
    window.ondrop = function(e) { e.preventDefault(); return false; };
    var holder = document.getElementById(elemId);
    holder.ondragover = function() { this.className = 'hover'; return false; };
    holder.ondragleave = function() { this.className = ''; return false; };
    holder.ondrop = function(e) {
        e.preventDefault();
	for (var i = 0; i < e.dataTransfer.files.length; ++i) {
	    dropResponse(e.dataTransfer.files[i].path);
	}
	this.className = '';
        return false;
    };
}

ready(() => {
    updateFrame('tiff-frame', (path) => {
	var ext = path.split('.').pop();
	if (ext != 'tiff') {
	    window.alert('Error: expected tiff file');
	} else {
	    global.backend.import_source_image(() => {
		window.alert('Import complete...');
	    }, path);
	}
    });
});

ready(() => {
    updateFrame('gal-frame', (path) => {
	var ext = path.split('.').pop();
	if (ext != 'gal') {
	    window.alert('Error: expected gal file');
	} else {
	    global.backend.import_source_gal(() => {
		window.alert('Import complete...');
	    }, path);
	}
    });
});
