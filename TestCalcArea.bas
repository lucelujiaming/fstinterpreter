IMPORT PushCard
IMPORT CalcArea
SUB main ()
	# init vitual camera input
    # MR[20] = 720
    # MR[21] = -800
    # MR[22] = 500
    XV = 720
    YV = -800
    ZV = 500
    # main loop
    WHILE 1
	    CALL CalcArea::main()
		
		YV = YV + 10
		IF YV > 800 THEN
		    YV = -800
			XV = XV + 10			
			IF XV > 1400 THEN
				BREAK
			ENDIF
		ENDIF
    WEND
END SUB 
