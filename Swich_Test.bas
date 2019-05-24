SUB main ()
    WHILE 1 = 1 
        R[1]  = R[1]  + 1  
        SELECT CASE  R[1]  
            CASE  1 
                WAIT  5
            BREAK 
            CASE  2 
                WAIT  5
            BREAK 
            DEFAULT   
                WAIT  0
            BREAK 
        END SELECT 
        IF R[1]  = 4 THEN 
            R[1]  = 0  
        ENDIF 
    WEND 
    END 
END SUB 
