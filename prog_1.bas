SUB main ()
    MOVEL P[5], 2000 SV 100 ;ACC 40 
    MOVEL P[5], 2000 CNT -1 
    MOVEL P[5], 20 CNT -1 
    R[1]  = 2 + ( 33.65 % 7.2 )  
    IF R[1]  == 2 THEN 
        WAIT COND DI[1]  = PE  3 skip
    ENDIF 
    IF 2+R[2]  == 2 THEN 
        WAIT COND R[1]  > 10 3 skip
    ELSEIF R[1]  == 1 THEN 
        IF 2 == 2 THEN 
        ENDIF 
        MOVEL PR[MR[3]], 2000 SV 100 
    ENDIF 
END SUB 
