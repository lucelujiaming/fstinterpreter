IMPORT prog_1
SUB main (param0, param1)
    PR[1].POSE = 5.9 6.7 7.4 8.5 12.4 15.3  
    PR[1] = 35.9 46.7 76.4 18.5 152.4 145.3  
    R[1].JOINT = 5  
    R[1] = R[1] + 5  
    IF R[1]+sin(6+7) == 100 THEN 
        MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
    ELSEIF R[1] == 1 THEN 
        WAIT COND DI[0]  = ON  10 skip
    ENDIF 
    WHILE R[1]+sin(2) <= 100 
        MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
        IF 8+sin(2) == 2 THEN 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
            BREAK 
        ENDIF 
        IF 5+sin(2) == 2 THEN 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
            CONTINUE 
        ENDIF 
        PRINT "R[1] = ", R[1]; "" 
        R[1] = R[1] + 5  
    WEND 
    SELECT CASE  R[2+( 6-7 )]  
        CASE  5 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
        BREAK 
        DEFAULT  5 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
        BREAK 
    END SELECT 
    # comment thing 
    CALL prog_1::main (1, 2) 
    CALL prog_1::LujiamingParamsSub (1, 2, 3) 
    
    R[1] = 5 + ( 6-7 )  
END SUB 

SUB func_test (param0, param1)
    R[6] = param0 + ( 2+2 )  
    RETURN R[6] 
END SUB 
