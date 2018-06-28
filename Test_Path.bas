SUB main ()
    
    R[1]  = 1  
    R[2]  = 1  
    MOVEL P[20], 1600 CNT -1 
    WHILE R[1]  < 10 
        MOVEL P[20], 1600 CNT -1 
        MOVEL P[30], 1600 CNT -1 
        R[1]  = R[1]  + 1  
    WEND 
    MOVEL P[20], 1600 CNT -1 
END SUB 
