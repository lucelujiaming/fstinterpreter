IMPORT MR_regTest
SUB main ()
    R[1]  = 1  
    WHILE R[1]  <= 21 
        WAIT COND  DI[R[1] ] = ON 
         DO[R[1] ] = ON   
        WAIT  5
        R[1]  = R[1]  + 1  
        WAIT COND  DI[R[1] ] = OFF 
         DI[R[1] ] = OFF   
        WAIT  R[1]
         DO[R[1] ] = PULSE  5  
        SELECT CASE  R[1]  
            CASE  2 
                R[1]  = 5  
            BREAK 
            CASE  10 
                 DO[1] = ON   
            BREAK 
            CASE  15 
                 DI[1] = ON   
            BREAK 
            DEFAULT   
                CALL MR_regTest::main () 
                CALLMACRO MR_regTest::main () 
            BREAK 
        END SELECT 
    WEND 
    END 
END SUB 
