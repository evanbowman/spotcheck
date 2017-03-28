var currentSelected = null;

var span = document.getElementsByClassName("close")[0];

function onRemovePressed() {
    if (currentSelected) {
	$(currentSelected).remove();
	currentSelected = null;
    }
}

function onScriptSavePressed() {
    var nameField = document.getElementById("script-name-field");
    if (nameField.value == "") {
	nameField.focus();
	return;
    }
    $("#table").append([
	"<tr>",
	"<td>Ima test!</td>",
	"</tr>"
    ].join(""));
    $("tr").click(onRowClicked);
    span.click();
}

function onDonePressed() {
    window.location.href = "index.html";
}

function onRowClicked() {
    $(this).addClass("selected").siblings().removeClass("selected");
    currentSelected = $(this);
}

var editor = CodeMirror.fromTextArea(document.getElementById("code"), {
    lineNumbers: true,
    styleActiveLine: true,
    matchBrackets: true,
});

editor.setOption("theme", "solarized dark");
editor.setOption("mode", "javascript");

function onCreatePressed() {
    editor.setValue("\nfunction main(srcPixels, maskPixels) {\n  //...\n}");
    document.getElementById("modal-btn").click();
}

$("tr").click(onRowClicked);

document.getElementById("modal-btn").onclick = function() {
    document.getElementById("modal").style.display = "block";
    editor.refresh();
}

span.onclick = function() {
    document.getElementById("script-name-field").value = "";
    document.getElementById("modal").style.display = "none";
}

window.onclick = function(event) {
    var modal = document.getElementById("modal");
    if (event.target == modal) {
        modal.style.display = "none";
    }
}
