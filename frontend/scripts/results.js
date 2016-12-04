var fs = require("fs");

function ready(fn) {
    if (document.readyState != "loading"){
        fn();
    } else {
        document.addEventListener("DOMContentLoaded", fn);
    }
}

function onStart() {
    var data = fs.readFileSync("./frontend/temp/results.json", "utf8");
    document.getElementById("json").innerHTML = data;
}

ready(onStart());
