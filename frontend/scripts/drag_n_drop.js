function ready(fn) {
    if (document.readyState != 'loading'){
        fn();
    } else {
        document.addEventListener('DOMContentLoaded', fn);
    }
}
ready(function() {
    window.ondragover = function(e) { e.preventDefault(); return false };
    window.ondrop = function(e) { e.preventDefault(); return false };
    var holder = document.getElementById('holder');
    holder.ondragover = function() { this.className = 'hover'; return false; };
    holder.ondragleave = function() { this.className = ''; return false; };
    holder.ondrop = function(e) {
        e.preventDefault();
	for (var i = 0; i < e.dataTransfer.files.length; ++i) {
	    global.backend.import_source_image(() => {
		window.alert("Upload successful...");
	    }, e.dataTransfer.files[i].path);
	}
	this.className = '';
        return false;
    };
});
