SUB main (param0, param1)
    PRINT "This program demostrates all commands.", "" 
    FOR X=1 TO 3 
        PRINT "hello continue", "" 
    NEXT 
    
    LOOP 4 
        PRINT "Test LujiamingSub.", "" 
    ENDLOOP 
    CALL prog_1::LujiamingParamsSub (1, 2, 3) 
    RETURN 50 
END SUB 


SUB LujiamingParamsSub (X, Y, Z)
    LOOP 5 
        PRINT "Test LujiamingSub.", X; Y; Z; "" 
        
        MOVEL 11.1, 2.2, 3.3, 4.4, 5.5, 6.6, 5.5 CNT 35 ;ACC 40 ;OFFSET J_VEC (100, 0, 0, 45, 0, 0);TOOL_OFFSET J_VEC (100, 0, 0, 45, 0, 0);TA 100 ASSIGN R[1] = 5 + ( 6-7 ) ;TB 100 CALL prog_1::test (1, 2)
        MOVEL 12.1, 2.2, 3.3, 4.4, 5.5, 6.6, 5.5 SD 50 ;ACC 40 ;OFFSET C_VEC_UF (0, 0, 100, 40.5, 0, 0) UF[1];TOOL_OFFSET C_VEC_UF (0, 0, 100, 40.5, 0, 0) UF[1]
        MOVEL 13.1, 2.2, 3.3, 4.4, 5.5, 6.6, 5.5 SV 50 ;ACC 40 ;OFFSET PR_UF PR[2] UF[1];TOOL_OFFSET PR_UF PR[2] UF[1]
        MOVEJ 14.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.5 SV 50 
        MOVEJ 15.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.5 CNT 20 ;ACC 40 ;EV 40 ;Ind_EV 40 
        MOVEJ 16.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.5 SD 50 ;ACC 40 ;TA 100 ASSIGN R[1] = 5 + ( 6-7 ) ;TB 100 CALL prog_1::test (1, 2)
        MOVEJ 17.1, 2.2, 3.3, 4.4, 5.5, 6.6, 0.5 SV 50 ;ACC 40 ;DB 100 ASSIGN R[1] = 5 + ( 6-7 ) 
        MOVEC 18.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11, 12.12, 0.5 CNT 20 ;ACC 40 
        MOVEC 19.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11, 12.12, 0.5 SD 50 ;ACC 40 
        MOVEC 10.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11, 12.12, 0.5 SV 50 ;ACC 40 
    ENDLOOP 
    RETURN 44.6 
END SUB 

SUB test (X)
    PRINT "Prog1 test.", "" 
END SUB 
