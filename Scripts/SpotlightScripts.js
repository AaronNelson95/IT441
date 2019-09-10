/* Lab 1 Spotlight Control Scripts */
var autoIsOn = 0;

document.getElementById("off").addEventListener("click", offClicked);
function offClicked(){
	/* Help with removing a class from multiple elements is here: https://clubmate.fi/remove-a-class-name-from-multiple-elements-with-pure-javascript/ */
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("off").className="running";
	stopAuto();
}


document.getElementById("green").addEventListener("click", greenClicked);
function greenClicked(){
	runGreen();
	stopAuto();
}


document.getElementById("yellow").addEventListener("click", yellowClicked);
function yellowClicked(){
	runYellow();
	stopAuto();
}


document.getElementById("red").addEventListener("click", redClicked);
function redClicked(){
	runRed();
	stopAuto();
}


document.getElementById("auto").addEventListener("click", autoClicked);
function autoClicked(){
	if(autoIsOn == 1){return;};	// For some reason if auto is clicked multiple times while it is already running, the timing gets extremely messed up.
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("auto").className="autoOn";
	autoIsOn = 1;
	runCycle();
}

function runCycle(){
	runGreen();
	timey = setTimeout(function(){
		runYellow();
	}, 1000);	
	timer = setTimeout(function(){
		runRed();
	}, 2000);
	timeg = setTimeout(function(){
		runCycle();
	}, 3000);
}

function stopAuto(){
	document.getElementById("auto").classList.remove("autoOn");
	autoIsOn = 0;
	clearTimeout(timey);
	clearTimeout(timer);
	clearTimeout(timeg);
}

function runGreen(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("green").className="running";
}

function runYellow(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("yellow").className="running";
}

function runRed(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("red").className="running";
}