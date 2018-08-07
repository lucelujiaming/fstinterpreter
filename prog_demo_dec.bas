IMPORT prog_luce
SUB main (param0, param1)
    DI[2] = 1  
    DI[3] = DI[2]  
    PR[1].JOINT = 35.9 46.7 76.4 18.5 152.4 145.3  
    PR[1].ID = 123  
    PR[1].TYPE = 123  
    PR[1].COMMENT = "lujiaming test for comment"  
    R[1] = 5  
    R[1] = R[1] + 5  
    IF R[1]+sin(6+7) == 100 THEN 
        MOVEL 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.0 SV 50 ;ACC 40 
    ELSEIF R[1] == 1 THEN 
        WAIT COND DI[0]  = ON  10 skip
    ENDIF 
    RRRRR = 5  
    WHILE RRRRR+sin(2) <= 100 
        MOVEL 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.0 SV 50 ;ACC 40 
        IF R[1]+sin(2) == 2 THEN 
            MOVEL 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.0 SV 50 ;ACC 40 
            BREAK 
        ENDIF 
        IF RRRRR+sin(2) == 2 THEN 
            MOVEL 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.0 SV 50 ;ACC 40 
            CONTINUE 
        ENDIF 
        PRINT "RRRRR = ", RRRRR; "" 
        RRRRR = RRRRR + 1  
    WEND 
    SELECT CASE  R[2+( 6-7 )]  
        CASE  5 
            MOVEL 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.0 SV 50 ;ACC 40 
        BREAK 
        DEFAULT  5 
            MOVEL 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.0 SV 50 ;ACC 40 
        BREAK 
    END SELECT 
    CALL prog_luce::main (1, 2) 
    CALL prog_luce::LujiamingParamsSub (1, 2, 3) 
    R[1] = 5 + ( 6-7 )  
END SUB 
SUB func_test (param0, param1)
    R[6] = param0 + ( 2+2 )  
    RETURN R[6] 
END SUB 
