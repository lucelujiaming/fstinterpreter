IMPORT PullCard
IMPORT PushCard
SUB main ()
    OVC. = 30
    OAC. = 100
    WAIT COND RI[1] = 1 OR (DO[602] = 1 AND DO[603] = 1) AND RI[2] = 1
    DO[505] = 0
    WAIT 1
    WHILE 1
        WAIT COND DO[601] = 1 OR (DO[602] = 1 AND DO[603] = 1) OR DO[604] = 1
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
        WAIT COND DO[605] = 1
        DO[505] = 1
        #CALL PullCard::main()
        IF DO[505] = 0 THEN
             ABORT
        ENDIF
        WAIT COND DO[606] = 1
        FKJ_ONE()
        SELECT CASE MH[5]
              # target is out of the arm range
              CASE 1
                    IF MH[2] > 0 THEN
                         MOVEJ PR[25], 100 CNT -1
                    ELSE
                         MOVEX PR[7], PR[8], PR[9], PR[26], 100 CNT -1
                    ENDIF
                    BREAK
              # target is not on the road 
              CASE 2
                    IF MH[2] > 0 THEN
                         MOVEJ PR[25], 100 CNT -1
                    ELSE
                         MOVEX PR[7], PR[8], PR[9], PR[26], 100 CNT -1
                    ENDIF
                    BREAK
              # target is in the range of the arm
              CASE 3
                    IF MH[2] > 0 THEN
                         MOVEJ PR[27], 100 CNT -1
                    ELSE
                         MOVEX PR[7], PR[8], PR[9], PR[27], 100 CNT -1
                    ENDIF
                    MOVEL PR[28], 1000 CNT -1
                    BREAK
              # target is in the range of the armmmm but too close to reach
              CASE 4
                    IF MH[2] > 0 THEN
                          MOVEJ PR[10], 50 CNT -1
                          MOVEJ PR[27], 100 CNT -1
                    ELSE
                          MOVEX PR[7], PR[8], PR[9], PR[27], PR[11], 100 CNT -1
                    ENDIF
                    BREAK
              DEFAULT
                    ABORT
        END SELECT
        #CALL PushCard::main()
        WAIT COND RI[1] = 1 AND RI[2] = 1
        DO[505] = 0
        WAIT 2
        DO[507] = 0
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
        FKJ_TWO()
        SELECT CASE MH[5]
               CASE 1
                     IF MH[2] > 0 THEN
                         MOVEJ PR[29], 50 CNT -1
                     ELSE 
                         MOVEX PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     DO[501] = 1
                     BREAK
               CASE 2
                     IF MH[2] > 0 THEN
                         MOVEJ PR[29], 50 CNT -1
                     ELSE
                         MOVEX PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     DO[502] = 1
                     BREAK
               CASE 3
                     MOVEL PR[27], 1000 CNT -1
                     IF MH[2] > 0 THEN
                         MOVEJ PR[29], 50 CNT -1
                     ELSE
                         MOVEX PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     DO[503] = 1
                     BREAK
                CASE 4
                     IF MH[2] > 0 THEN
                         MOVEJ PR[29], 50 CNT -1
                     ELSE
                         MOVEX PR[9], PR[8], PR[7], PR[29], 50 CNT -1
                     ENDIF
                     DO[504] = 1
                     BREAK
                DEFAULT
                      ABORT
        END SELECT
   WEND
END SUB 
