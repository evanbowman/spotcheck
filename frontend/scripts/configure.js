var fs = require("fs");

var currentSelected = null;

var span = document.getElementsByClassName("close")[0];

function onRemovePressed() {
    if (currentSelected) {
	$(currentSelected).remove();
	document.getElementById("remove-btn").style.display = "none";
	document.getElementById("edit-btn").style.display = "none";
	delete g_metricTable[$(currentSelected).find("td:nth-child(1)").text()];
	currentSelected = null;
    }
}

var g_metricTable = null;

const configFile = "/.spotcheck.json";

var homeDir = require("os").homedir();
if (!fs.existsSync(homeDir + configFile)) {
    global.backend.write_default_config(homeDir + configFile);
}
var data = fs.readFileSync(homeDir + configFile, "utf8");
g_metricTable = JSON.parse(data);

for (key in g_metricTable) {
    $("#table").append([
	"<tr>",
	"<td>" + key + "</td>",
	"</tr>"
    ].join(""));
}
$("tr").click(onRowClicked);

var editor = CodeMirror.fromTextArea(document.getElementById("code"), {
    lineNumbers: true,
    styleActiveLine: true,
    matchBrackets: true,
    gutters: ["CodeMirror-lint-markers"],
    lint: true
});

editor.setOption("theme", "solarized dark");
editor.setOption("mode", "javascript");

var g_editing = "";

function onSavePressed() {
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
	enabled: true,
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
    fs.writeFileSync(homeDir + "/.spotcheck.json", JSON.stringify(g_metricTable));
}

function onDonePressed() {
    fs.writeFile(homeDir + "/.spotcheck.json", JSON.stringify(g_metricTable), function(err) {
	if (err) {
	    window.alert(err);
	}
	global.backend.configure(homeDir + "/.spotcheck.json");
    });
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
    editor.setValue("\nfunction main(src, mask) {\n  var result = 0;\n  // ... your code here\n  return result;\n}");
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

document.getElementById("remove-btn").style.display = "none";

document.getElementById("edit-btn").style.display = "none";

//same function from roi.js, will i get an error for redefinieng
//should be fine
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

function ready(fn){
    if(document.readyState == "complete"){
        fn();
    } else{
        document.addEventListener("DOMContentLoaded",fn);
    }
}

ready(()=>{
    if(!global.scaleSet){
        global.xScale = 1;
        global.yScale = 1;
        global.hScale = 1;
    }
    $("#config-xScaleField").on("input",function (){
        global.xScale = getTbValueAsInt(this);
        global.scaleSet=true;
        });
    $("#config-yScaleField").on("input",function (){
        global.yScale = getTbValueAsInt(this);
        global.scaleSet=true;
    });
    $("#config-hScaleField").on("input",function (){
        global.hScale = getTbValueAsInt(this);
        global.scaleSet=true;
    });
    document.getElementById("config-xScaleField").value = global.xScale;
    document.getElementById("config-yScaleField").value = global.yScale;    document.getElementById("config-hScaleField").value = global.hScale;

});
