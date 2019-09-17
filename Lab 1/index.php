<!doctype html>
<html>
	<head>
		<!--Aaron Nelson
		Last Updated: 9-8-19
		This is a simple home page on my website. It is not needed for the lab -->
		<meta http-equiv="content-type" content="text/html; charset=utf-8" />
		<link rel="Shortcut Icon" href="/favicon.ico" type="image/x-icon" />
		<meta http-equiv="pragma" content="no-cache" />
		<meta name="robots" content="all" />
		<meta name="MSSmartTagsPreventParsing" content="true" />
		<meta http-equiv="imagetoolbar" content="false" />	
		<title>Aaron's Site</title>
		<link rel="stylesheet" href="Styles/Styles.css" type="text/css">
		<script type="text/javascript" src="Scripts/DateModified.js"></script>
	</head>
	<body>
		<div id="wrapper">
			<header>
				<h1>Aaron's Class Projects</h1>
			</header>
			<?php include 'navbar.php'; ?>
			<div id="pageTitle">Welcome to my Website</div>
			
		
			<div id="main">	
				<p>In the future, I will post class projects that I worked on while attending Brigham Young University. Stay tuned for updates. </p>
				
				
			
			</div>
		
			<footer>Aaron Nelson &emsp; Brigham Young University
			<br><script language="Javascript">document.write("This page was last modified on: "); document.write(writeDateModified())</script> <!-- A script I had in a prior class that simply just writes the date this file was last changed onto the page. -->
			</footer>
		</div>
		
		<script>
			document.getElementById("homelink").className="active";
		</script>
	
	</body>
</html>
