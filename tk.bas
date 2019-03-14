SUB main ()
	DO[8] = OFF
	DO[9] = OFF
	WAIT COND DO[0]  = ON
	WAIT COND DO[1]  = ON
	DO[9] = OFF
	DO[505] = ON
END SUB 