IMPORT MR_regTest
SUB main ()
     DO[1] = 1  
    IF  DO[1] = 0 THEN 
         DO[1] = 0  
    ENDIF 
     RO[1] = 1  
    IF  RO[1] = 0 THEN 
         RO[1] = 0  
    ENDIF 
     UO[1] = 1  
    IF  UO[1] = 0 THEN 
         UO[1] = 0  
    ENDIF 
    R[1]  = 1  
    IF R[1]  <= 2 THEN 
        R[1]  = 5  
    ENDIF 
    MR[1]  = 1  
    IF MR[1]  <= 2 THEN 
        MR[1]  = 5  
    ENDIF 
    SR[1]  = "Hello, World"  
    IF SR[1]  = "Hello, World" THEN 
        SR[1]  = "Hello"  
    ENDIF 
    END 
END SUB 
