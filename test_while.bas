IMPORT MR_regTest
SUB main ()
     DO[1] = 1  
    WHILE  DO[1] = 0 
         DO[1] = 0  
    WEND 
     RO[1] = 1  
    WHILE  RO[1] = 0 
         RO[1] = 0  
    WEND 
     UO[1] = 1  
    WHILE  UO[1] = 0 
         UO[1] = 0  
    WEND 
    R[1]  = 1  
    WHILE R[1]  <= 2 
        R[1]  = 5  
    WEND 
    MR[1]  = 1  
    WHILE MR[1]  <= 2 
        MR[1]  = 5  
    WEND 
    SR[1]  = "Hello, World"  
    WHILE SR[1]  = "Hello, World" 
        SR[1]  = "Hello"  
    WEND 
    END 
END SUB 
