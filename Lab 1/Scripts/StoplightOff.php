<?php
	// This script turns off the GPIO pins associated with green, yellow, and red. It keeps the mode as "out" because when set "in", some lights may get random voltage coming through, weakly lighting the light.
	system('gpio -g write 2 0');
	system('gpio -g write 3 0');
	system('gpio -g write 4 0');
	system('gpio -g mode 2 out');
	system('gpio -g mode 3 out');
	system('gpio -g mode 4 out');
?>
