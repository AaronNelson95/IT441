<?php
	system('gpio -g mode 2 out');
	system('gpio -g mode 3 out');
	system('gpio -g mode 4 out');
	system('gpio -g write 2 0');
	system('gpio -g write 3 0');
	system('gpio -g write 4 1');
?>
