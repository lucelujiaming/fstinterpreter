IMPORT prog_1
SUB main (param0, param1)
    MOVEL 11.1 2.2 3.3 4.4 5.5 6.6 50 CNT -1 ;ACC 40 ;OFFSET J_VEC (100, 0, 0, 45, 0, 0);TOOL_OFFSET J_VEC (100, 0, 0, 45, 0, 0);TA 100 ASSIGN R[1] = 5+( 6-7 );TB 100 CALL prog_demo_dec::prog_1 (1, 2)
    MOVEL 12.1 2.2 3.3 4.4 5.5 6.6 30 SD 50 ;ACC 40 ;OFFSET C_VEC_UF (0, 0, 100, 40.5, 0, 0) UF[1];TOOL_OFFSET C_VEC_UF (0, 0, 100, 40.5, 0, 0) UF[1]
    MOVEL 13.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 ;OFFSET PR_UF PR[2] UF[1];TOOL_OFFSET PR_UF PR[2] UF[1]
    MOVEJ 14.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 
    MOVEJ 15.1 2.2 3.3 4.4 5.5 6.6 50 CNT 999 ;ACC 40 ;EV 40 ;Ind_EV 40 
    MOVEJ 16.1 2.2 3.3 4.4 5.5 6.6 30 SD 50 ;ACC 40 ;TA 100 ASSIGN R[1] = 5+( 6-7 );TB 100 CALL prog_demo_dec::prog_1 (1, 2)
    MOVEJ 17.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 ;DB 100 ASSIGN R[1] = 5+( 6-7 )
    MOVEC 18.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 10.10 11.11 12.12 50 CNT 333 ;ACC 40 
    MOVEC 19.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 10.10 11.11 12.12 30 SD 50 ;ACC 40 
    MOVEC 10.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 10.10 11.11 12.12 100 SV 50 ;ACC 40 
    TF_No = 5+R[1]  
    TIMER 1 start 
    USERALARM 1 
    PAUSE 
    ABORT 
    WAIT  3
    WAIT COND DI[1]  = PE 
    WAIT COND R[1]  > 10 3 skip
    R[1] = 5 
    IF R[1]+sin(6+7) == 100 THEN 
        MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
    ELSEIF R[1] == 1 THEN 
        WAIT COND DI[0]  = ON  10 skip
    ENDIF 
    WHILE R[1]+sin(2) <= 100 
        MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
        IF 8+sin(2)+prog_demo_dec::func_test(6+( 6-7 ), 3) == 2 THEN 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
            BREAK 
        ENDIF 
        IF 5+sin(2) == 2 THEN 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
            CONTINUE 
        ENDIF 
        PRINT "R[1] = ", R[1]; "" 
        R[1] = R[1]+5 
    WEND 
    SELECT CASE  R[1] 
        CASE  5 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
        BREAK 
        DEFAULT  5 
            MOVEL 1.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 
        BREAK 
    END SELECT 
    # comment thing 
    CALL prog_demo_dec::func_test (6+( 6-7 ), 2) 
    CALL prog_1::main (1, 2) 
    CALL prog_1::LujiamingParamsSub (1, 2, 3) 
    
    R[1] = 5+( 6-7 ) 
END SUB 

SUB func_test (param0, param1)
    R[6] = param0+( 2+2 ) 
    RETURN R[6] 
END SUB 
