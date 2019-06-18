SUB main ()
    OVC. = 100  
    WHILE 1 = 1 
        MOVEJ P[1], 57 SD 100 
        MOVEJ P[2], 60 SD 150 
        MOVEJ P[4], 57 SD 100 
        MOVEJ P[5], 49 SD 100 
        MOVEJ P[6], 57 SD 100 
        MOVEJ P[7], 57 SD 100 
        MOVEJ P[8], 53 SD 200 
        MOVEJ P[1], 58 SD 100 
        MOVEJ P[9], 60 SD 200 
        MOVEJ P[10], 60 SD 200 
        MOVEJ P[11], 50 SD 100 
        MOVEJ P[12], 50 SD 100 
        MOVEJ P[13], 51 SD 150 
        MOVEJ P[14], 59 SD 100 
        MOVEJ P[15], 54 SD 300 
        MOVEJ P[1], 54 CNT -1 
        WAIT  5
        MOVEJ P[16], 100 CNT -1 
        MOVEL P[20], 1000 SD 300 
        MOVEL P[21], 1000 SD 300 
        MOVEL P[22], 1000 SD 300 
        MOVEL P[23], 1000 SD 300 
        MOVEL P[20], 1000 CNT -1 
        WAIT  5
        MOVEJ P[16], 100 CNT -1 
        MOVEL P[17], 1000 SD 300 
        MOVEL P[18], 1000 SD 300 
        MOVEL P[19], 1000 SD 300 
        MOVEL P[3], 1000 SD 300 
        MOVEL P[17], 1000 CNT -1 
    WEND 
    END 
END SUB 
