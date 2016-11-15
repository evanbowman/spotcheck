function onAnalyzePressed() {
    global.backend.launch_analysis(() => {
	window.location.href = "frontend/layouts/results.html";
    });
}
