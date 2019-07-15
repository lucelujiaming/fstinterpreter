#IMPORT PullCard
#IMPORT PushCard
SUB main ()
    OVC. = 50
    WHILE 1
        WAIT COND DO[601] = 1 OR DO[602] = 1 
              IF DO[601] = 1 THEN
                   MR[1] = 1
              ELSE
                   MR[1] = 2
              ENDIF
        COMPUTE_DYN_POINT
        SELECT CASE MR[1]
               CASE 1
                      MOVEJ PR[3], 100 CNT -1
                      BREAK
               CASE 2
                      MOVEJ PR[4], 100 CNT -1
                      BREAK
               DEFAULT
                      ABORT
        END SELECT
        IF MR[1] = 1 THEN
            DO[501] = 1
        ELSE
            DO[502] = 1
        ENDIF
    WEND
END SUB 
