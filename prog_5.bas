SUB main ()
    R[1]  = 2 + ( 2+2 )  
    WAIT  3
    WAIT  2
    IF R[1]  == 2 THEN 
        WAIT COND DI[1]  = PE  3 SKIP
    ENDIF 
    IF 2+R[2]  == 20 THEN 
        WAIT COND R[1]  > 10 3 SKIP
    ELSEIF R[1]  == 6 THEN 
        IF 2 == 2 THEN 
            WAIT  3
        ENDIF 
    ENDIF 
    TF_NO. = 2 + ( 2+2 )  
END SUB 
