function onTestClicked() {
    global.backend.workloadTest(() => {
	document.getElementById("demo").innerHTML = "It worked!";
    });
}
