<!doctype html>
<html>
	<head>
		<!--Aaron Nelson
		Last Updated: 9-10-19
		A page written for lab 1 of IT441 that simply allows you to select a stoplight color (or have it automatically rotate through them) and it reflects this change with GPIO pins. -->
		<title>Stoplight</title>
		<link rel="stylesheet" href="Styles/Styles.css" type="text/css">
	</head>
	<body onload="offClicked()"> <!-- When the page first loads, initialize all of the pins by setting their mode to output and turning them all off. -->
		<div id="wrapper">
			<header>
				<h1>Aaron's Class Projects</h1>
			</header>
			<?php include 'navbar.php'; ?>
			<div id="pageTitle">Lab 1 - Stoplight Control</div>
			
		
			<div id="main">	
				<p>Choose the color of the stoplight or select "Auto" to have it cycle through each light. You can set the time increment (in milliseconds) between each change at the bottom of the page. The current status of the stoplight is highlighted blue.  </p>
				
				<!-- This table has the 3 stoplight colors, as well as an associated image, sorted along with an off option and an auto option. JavaScript responds to the page clicks to highlight the table cell currently running, and to run a script that alters the GPIO pins. -->
				<table class=SpotlightColors>
					<tr>
						<td id="off" class="running" colspan=3><h4 class="off">OFF</h4></td>
					</tr>
					<tr>
						<!-- Stoplight images from https://www.freemanhealth.com/blog/the-behavior-traffic-light-colors-of-the-light -->
						<td id="green"><img src="Images/GreenLight.png"><br><h4 class="green">Green</h4></td>
						<td id="yellow" ><img src="Images/YellowLight.png"><br><h4 class="yellow">Yellow</h4></td>
						<td id="red"><img src="Images/RedLight.png"><br><h4 class="red">Red</h4></td>
					</tr>
					<tr>
						<td id="auto" colspan=3><h4 class="auto">Auto</h4></td>
					</tr>
				</table>
				
				
				<!-- This allows the user to input the number of ms they wish to wait until the light changes to the next color. -->
				*** To make changes take place, turn the light "OFF" then select "Auto" again ***<br>
				<input type="number" name="GY" id="GY" class="colorPick" value=1000 /> ms to change from <span class="green">Green</span> -> <span class="yellow">Yellow</span>.
				<br>
				<input type="number" name="YR" id="YR" class="colorPick" value=1000 /> ms to change from <span class="yellow">Yellow</span> -> <span class="red">Red</span>.
				<br>
				<input type="number" name="RG" id="RG" class="colorPick" value=1000 /> ms to change from <span class="red">Red</span> -> <span class="green">Green</span>.
				<br><br>
					
			
			</div>
		
			<footer>Aaron Nelson &emsp; BYU - Embedded Systems - Lab 1 &emsp; Fall 2019
			<br>This page was last modified on: 9/10/2019
			</footer>
		</div>
		
		<script>
			document.getElementById("lab1link").className="active"; <!-- With a dynamic php navigation bar, this will set the page we are on to "active", where css is set to show it differently than the other tabs. -->
		</script>
		
		<script src="Scripts/StoplightScripts.js"></script>
		<script src="http://code.jquery.com/jquery-1.11.0.min.js"></script> <!-- Allows the ajax scripts to work (which pushes a request for the GPIO color scrips to run) -->
	</body>
</html>
