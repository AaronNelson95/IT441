/* Lab 1 Spotlight Control Scripts */

document.getElementById("off").addEventListener("click", offClicked);
function offClicked(){
	/* Help with removing a class from multiple elements is here: https://clubmate.fi/remove-a-class-name-from-multiple-elements-with-pure-javascript/ */
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("off").className="running";
}


document.getElementById("green").addEventListener("click", greenClicked);
function greenClicked(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("green").className="running";
}


document.getElementById("yellow").addEventListener("click", yellowClicked);
function yellowClicked(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("yellow").className="running";
}


document.getElementById("red").addEventListener("click", redClicked);
function redClicked(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("red").className="running";
}


document.getElementById("auto").addEventListener("click", autoClicked);
function autoClicked(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("auto").className="running";
}