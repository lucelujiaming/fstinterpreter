IMPORT PullCard
IMPORT PushCard
IMPORT CalcArea
SUB main ()
    # set global vel ratio and acc ratio
    OVC. = 100
    OAC. = 100
    # wait until no card in arm
    WAIT COND RI[1] = 1 AND RI[2] = 1
    DO[505] = 0
    WAIT 1
	# init vitual camera input
    MR[20] = 720
    MR[21] = -800
    MR[22] = 500
    # main loop
    WHILE 1
	    CALL CalcArea::main()
		
		MR[21] = MR[21] + 10
		IF MR[21] > 800 THEN
		    MR[21] = -800
			MR[20] = MR[20] + 10			
			IF MR[20] > 1400 THEN
				BREAK
			ENDIF
		ENDIF
		
        # wait plc signal to move to the pull-card position 
        WAIT COND DO[601] = 1 OR DO[602] = 1  OR DO[603] = 1  OR DO[604] = 1 
        IF DO[601] = 1 THEN
              MR[1] = 1
        ELSEIF DO[602] = 1 THEN
              MR[1] = 2
        ELSEIF DO[603] = 1 THEN
              MR[1] = 3
        ELSEIF DO[604] = 1 THEN
              MR[1] = 4
        ELSE
              MR[1] = 0
        ENDIF
        DO[506] = 0
        # move to the expected pull-card position
        SELECT CASE MR[1]
                CASE 1
                      MOVEJ PR[3], 30 CNT -1
                      DO[501] = 1
                      BREAK
                CASE 2
                      MOVEJ PR[4], 30 CNT -1
                      DO[502] = 1
                      BREAK
                CASE 3
                      MOVEJ PR[5], 30 CNT -1
                      DO[503] = 1
                      BREAK
                CASE 4
                      MOVEJ PR[6], 30 CNT -1
                      DO[504] = 1
                      BREAK
               DEFAULT
                      ABORT
        END SELECT
# Open the annotation of the next line if you don't want to do pull-card operation
#DO[505] = 1
        # wait plc signal to pull card
        WAIT COND DO[605] = 1
        # do pull-card operation
        CALL PullCard::main()
        # make sure the card is in arm
        IF DO[505] = 0 THEN
            ABORT
        ENDIF
        # wait plc signal to send card
        WAIT COND DO[606] = 1
        # make sure all data from visual system comming
        WAIT COND MH[5] > 0
        # compute all necessary dynamic position by processing visual data
        FKJ_ONE()
        DO[501] = 0
        DO[502] = 0
        DO[503] = 0
        DO[504] = 0
        # move to the expected position according to the region information
        SELECT CASE MH[5]
              # target is out of the arm range
              CASE 1
                    IF MH[2] > 0 THEN
                         MOVEJ PR[25], 80 CNT -1
                    ELSE
                         MOVEX PR[7], PR[8], PR[9], PR[10], PR[26], 80 CNT -1
                    ENDIF
                    BREAK
              # target is not on the road, only big trunk may happen 
              CASE 2
                    IF MH[2] > 300 THEN
                         MOVEJ PR[11], 80 CNT -1
                         MOVEJ PR[27], 80 CNT -1
                    ELSEIF MH[2] < -500 THEN
                         MOVEX PR[7], PR[8], PR[9], PR[10], PR[27], 80 CNT -1
                    ELSEIF MH[2] > 0 THEN
                         MOVEX PR[7], PR[13], PR[14], PR[27], 80 CNT -1
                    ELSEIF MH[2] < -350 THEN
                         MOVEX PR[7], PR[8], PR[9], PR[10], PR[15], PR[27], 80 CNT -1
                    ELSE
                         MOVEX PR[7], PR[8], PR[9], PR[10], PR[15], 80 CNT -1
                         MOVEL PR[27], 1000 CNT -1
                    ENDIF
                    BREAK
              # target is in the range of the arm
              CASE 3
                    IF MH[2] > 0 THEN
                         MOVEJ PR[27], 80 CNT -1
                    ELSE
                         MOVEX PR[7], PR[8], PR[9], PR[10], PR[27], 80 CNT -1
                    ENDIF
                    MOVEL PR[28], 1000 CNT -1
                    BREAK
              # target is in the range of the arm but too close to reach
              CASE 4
                    IF MH[2] > 0 THEN
                         MOVEX PR[7], PR[13], PR[14], PR[27], 80 CNT -1
                    ELSEIF MH[2] < -350 THEN
                         MOVEX PR[7], PR[8], PR[9], PR[10], PR[15], PR[27], 80 CNT -1
                    ELSE
                         MOVEX PR[7], PR[8], PR[9], PR[10], PR[15], 80 CNT -1
                         MOVEL PR[27], 1000 CNT -1
                    ENDIF
                    MOVEL PR[28], 1000 CNT -1
                    BREAK
              DEFAULT
                    ABORT
        END SELECT
        # do push-card operation
        CALL PushCard::main()
        DO[507] = 1
        # wait the card in arm is taken by someone
        WAIT COND RI[1] = 1 AND RI[2] = 1
        DO[505] = 0
        WAIT 2
        DO[507] = 0
        # get the position of which the arm will go to for next cycle
        IF DO[601] = 1 THEN
             MR[1] = 1
        ELSEIF DO[602] = 1 THEN
             MR[1] = 2
        ELSEIF DO[603] = 1 THEN
             MR[1] = 3
        ELSEIF DO[604] = 1 THEN
             MR[1] = 4
        ELSE
             MR[1] = 0
        ENDIF
        # compute the dynamic position for going back
        FKJ_TWO()
        # move to the expected position according to the region information
        SELECT CASE MH[5]
               CASE 1
                     IF MH[2] > 0 THEN
                         MOVEJ PR[29], 50 CNT -1
                     ELSE 
                         MOVEX PR[10], PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     BREAK
               CASE 2
                     IF MH[2] > 300 THEN
                         MOVEJ PR[29], 50 CNT -1
                     ELSEIF MH[2] < -500 THEN
                         MOVEX PR[10], PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ELSEIF MH[2] > 0 THEN
                         MOVEX PR[14], PR[13], PR[7], PR[29], 50 CNT -1
                     ELSEIF MH[2] < -350 THEN
                         MOVEX PR[15], PR[10], PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ELSE
                         MOVEL PR[15], 1000 CNT -1
                         MOVEX PR[10], PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     BREAK
               CASE 3
                     MOVEL PR[27], 1000 CNT -1
                     IF MH[2] > 0 THEN
                         MOVEJ PR[29], 50 CNT -1
                     ELSE
                         MOVEX PR[10], PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     BREAK
                CASE 4
                     MOVEL PR[27], 1000 CNT -1
                     IF MH[2] > 0 THEN
                         MOVEX PR[14], PR[13], PR[7], PR[29], 50 CNT -1
                     ELSEIF MH[2] < -350 THEN
                         MOVEX PR[15], PR[10], PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ELSE
                         MOVEL PR[15], 1000 CNT -1
                         MOVEX PR[10], PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     BREAK
                DEFAULT
                      ABORT
        END SELECT
        # if no region is assigned, go to the safety position
        IF MR[1] = 0 THEN
                MOVEJ PR[2], 50 CNT -1
                DO[506] = 1
        ENDIF
   WEND
END SUB 
