SUB main ()
    R[1]  = 11  
    IF SR[2] = "XXXX"  THEN 
        R[2]  = R[1]   
    ELSE 
        TIMER[1]  = START  
        WAIT  1
        TIMER[1]  = STOP  
        IF R[2]  = 10 THEN 
             DO[1] = ON   
            R[3]  =  DO[1]  
            IF R[3]  AND R[1]  THEN 
                R[4]  = PR[1].PJ2   
                R[5]  = R[1]  + R[4]   
                R[6]  = R[1]  - R[4]   
                R[7]  = R[1]  * R[4]   
                R[8]  = R[1]  / R[4]   
            ENDIF 
            WAIT COND  DI[2] = ON 
        ENDIF 
    ENDIF 
    SR[1]  = "66.66running"  
    R[9]  = SR[1]   
    END 
END SUB 
