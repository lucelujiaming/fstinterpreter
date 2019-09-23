SUB main ()
    MOVEJ P[1], 100 CNT -1 
    MOVEL P[2], 4000 CNT -1 
    MOVEL P[3], 4000 CNT -1 
    MOVEL P[4], 4000 CNT -1 
    IF R[1]  = 100 THEN 
        PAUSE 
    ENDIF 
    MOVEJ P[5], 100 CNT -1 
    END 
END SUB 
