IMPORT MR_regTest
SUB main ()
    R[1]  = 1  
    WHILE R[1]  = 21 
        WAIT 
         RO[R[1] ] = ON   
        WAIT  5
        R[1]  = R[1]  + 1  
        WAIT 
         RI[R[1] ] = OFF   
        WAIT  5
         RO[R[1] ] = PULSE  5  
        SELECT CASE  R[1]  
            CASE  2 
                R[1]  = 5  
            BREAK 
            CASE  10 
                 RO[1] = ON   
            BREAK 
            CASE  15 
                 RI[1] = ON   
            BREAK 
            DEFAULT   
                CALL MR_regTest::main () 
            BREAK 
        END SELECT 
    WEND 
    END 
END SUB 
