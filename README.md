# ROVR

Outline of Program flow

Setup:
	LPC4088 Board
	PWM Motor timers and GPIOs
	I2C ADC interface (write to appropriate config registers in ADC)
	Timers for the sleep timer (removes echoes)
	
Continuously:
	Ping all 4 mics in order
		take 10 sample per mic
			square values to get mic power
			scale powers by microphone gain normalization factors
			average 10 samples

		repeat 4 times 
		median filter
		average (mic sample value)
		
	if mic sample value is above threshhold
		colect a value from 3 remaining mics
			
		if processed mic sample value > opposite processed mic sample value * scalar
			and if two same directions in row (w/out opposite direction)
				update direction according to appropriate processed mic sample value
