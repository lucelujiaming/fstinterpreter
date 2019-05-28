SUB main ()
    MOVEJ P[1], 100 CNT -1 
    MOVEJ P[2], 2 SV 50 
    MOVEJ P[3], 100 SD 100 
    MOVEJ P[4], 1 SV 75 
    MOVEJ P[5], 100 SD 25 
    MOVEL P[1], 2000 CNT -1 
    MOVEL P[2], 5 SD 50 
    MOVEL P[3], 2000 SV 100 
    MOVEL P[4], 3 SD 75 
    MOVEL P[5], 2000 SV 25 
    MOVEJ P[6], 100 CNT -1 
    MOVEC P[7], P[8], 2000 SV 50 
    MOVEC P[9], P[6], 3 SD 100 
    MOVEL P[10], 4000 SD 50 
    MOVEC P[11], P[12], 3 SV 75 
    MOVEC P[9], P[11], 1000 SD 25 
    END 
END SUB 
