IMPORT MarcuscTest
SUB main ()
    R[1]  = 10  
    WHILE R[1]  = 2 
        WAIT  R[3]
        R[2]  = R[2]  + 1  
    WEND 
END SUB 
