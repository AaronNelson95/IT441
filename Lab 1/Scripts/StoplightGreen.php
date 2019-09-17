<?php
	// This script turns on the green light (if set to pin 2) and turns off the other ones.
	system('gpio -g mode 2 out');
	system('gpio -g mode 3 out');
	system('gpio -g mode 4 out');
	system('gpio -g write 2 1');
	system('gpio -g write 3 0');
	system('gpio -g write 4 0');
?>
