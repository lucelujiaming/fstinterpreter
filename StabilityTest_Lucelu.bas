IMPORT Pick
SUB main ()
    R[1]  = 1  
    R[2]  = 1  
    CALL Pick::main () 
    R[1]  = 1  
    MOVEJ P[1], 100 CNT -1 
    END 
END SUB 
