SUB main ()
    MOVEJ P[1], 100 CNT -1 
    MOVEJ P[2], 2 CNT -1 
    MOVEJ P[3], 100 CNT -1 
    MOVEJ P[4], 1 CNT -1 
    MOVEJ P[5], 100 CNT -1 
    MOVEL P[1], 2000 CNT -1 
    MOVEL P[2], 5 CNT -1 
    MOVEL P[3], 2000 CNT -1 
    MOVEL P[4], 3 CNT -1 
    MOVEL P[5], 2000 CNT -1 
    MOVEJ P[6], 100 CNT -1 
    MOVEC P[7], P[8], 2000 CNT -1 
    MOVEC P[9], P[6], 3 CNT -1 
    MOVEL P[10], 4000 CNT -1 
    MOVEC P[11], P[12], 3 CNT -1 
    MOVEC P[9], P[11], 1000 CNT -1 
    END 
END SUB 
