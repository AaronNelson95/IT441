<?php
	system('gpio -g write 2 0');
	system('gpio -g write 3 0');
	system('gpio -g write 4 0');
	system('gpio -g mode 2 in');
	system('gpio -g mode 3 in');
	system('gpio -g mode 4 in');
?>
