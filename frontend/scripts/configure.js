var currentSelected = null;

var span = document.getElementsByClassName("close")[0];

function onRemovePressed() {
    if (currentSelected) {
	$(currentSelected).remove();
	currentSelected = null;
	document.getElementById("remove-btn").style.display = "none";
	document.getElementById("edit-btn").style.display = "none";
	delete g_metricTable[$(currentSelected).find("td:nth-child(1)").text()];
    }
}

var g_metricTable = {
    "area": {
	builtin: true,
    },
    "min height": {
	builtin: true,
    },
    "max height": {
	builtin: true,
    },
    "average height": {
	builtin: true,
    },
    "circularity": {
	builtin: true,
    },
    "volume": {
	builtin: true,
    }
};

var editor = CodeMirror.fromTextArea(document.getElementById("code"), {
    lineNumbers: true,
    styleActiveLine: true,
    matchBrackets: true,
});

editor.setOption("theme", "solarized dark");
editor.setOption("mode", "javascript");

var g_editing = "";

function onScriptSavePressed() {
    var nameField = document.getElementById("script-name-field");
    if (nameField.value == "") {
	nameField.focus();
	return;
    }
    if (g_metricTable.hasOwnProperty(nameField.value)) {
	if (nameField.value != g_editing) {
	    window.alert("That metric name is already in use!");
	    nameField.focus();
	    return;
	}
    }
    g_metricTable[nameField.value] = {
	src: editor.getValue(),
	builtin: false,
    };
    if (nameField.value != g_editing) {
	$("#table").append([
	    "<tr>",
	    "<td>" + nameField.value + "</td>",
	    "</tr>"
	].join(""));
	$("tr").click(onRowClicked);
    }
    span.click();
    g_editing = "";
}

function onDonePressed() {
    var userMetrics = {};
    for (key in g_metricTable) {
	if (!g_metricTable[key]["builtin"]) {
	    userMetrics[key] = g_metricTable[key];
	}
    }
    window.alert(JSON.stringify(userMetrics));
    window.location.href = "index.html";
}

function onRowClicked() {
    $(this).addClass("selected").siblings().removeClass("selected");
    currentSelected = $(this);
    var $td = $(this).find("td:nth-child(1)");
    if (!g_metricTable[$td.text()]["builtin"]) {
	document.getElementById("remove-btn").style.display = "";
	document.getElementById("edit-btn").style.display = "";
    } else {
	document.getElementById("remove-btn").style.display = "none";
	document.getElementById("edit-btn").style.display = "none";
    }
}

function onCreatePressed() {
    editor.setValue("\nfunction main(srcPixels, maskPixels) {\n  var result = 0;\n  //...\n  return result;\n}");
    document.getElementById("modal-btn").click();
}

function onEditPressed() {
    var currentName = $(currentSelected).find("td:nth-child(1)").text();
    editor.setValue(g_metricTable[currentName]["src"]);
    document.getElementById("script-name-field").value = currentName;
    document.getElementById("modal-btn").click();
    g_editing = currentName;
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

document.getElementById("remove-btn").style.display = "none";

document.getElementById("edit-btn").style.display = "none";
