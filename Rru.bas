SUB main ()
    IF R[1]  = R[1]  THEN 
        WAIT  2
    ELSEIF 2 = 2 THEN 
        WAIT  2
    ENDIF 
    SELECT CASE  R[1]  
        CASE  R[3]  
            R[1]  = R[2]   
        BREAK 
        CASE  R[2]  
            R[1]  = R[3]   
        BREAK 
    END SELECT 
    END 
END SUB 
