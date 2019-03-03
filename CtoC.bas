SUB main ()
    WHILE 1 = 1 
        MOVEJ P[1], 100 CNT -1 
        MOVEJ P[6], 100 CNT -1 
        WAIT  1
        MOVEC P[2], P[3], 4000 SV 50 
        MOVEC P[3], P[4], 4000 CNT -1 
    WEND 
    END 
END SUB 
