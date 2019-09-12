SUB main (param0, param1)
    PRINT "This program demostrates all commands."
    A[0] = 1.6
    FOR X = 1 TO 100
        A[X] = X % 10
    NEXT
    PRINT "This program demostrates all commands."
    A[5] = A[A[3] + 1]
    PRINT "This program demostrates all commands."
    A[A[A[A[9]]]] = A[A[A[A[8]]]]
    X = 1
    X = LujiamingParamsSub(1,2,3)
    CALL LujiamingSub 
    LOOP 5
       PRINT "Test WHILE."
    ENDLOOP
    WHILE X <= 300 
       X = X+5
       PRINT X; X*X
        IF X > 50 AND X < 60 THEN
           PRINT "X > 50 "
            IF X > 80 THEN
               PRINT "X > 80"
               CONTINUE
            ENDIF
        ELSE 
            IF X > 30 THEN
               PRINT "X > 30"
            ENDIF    
        ENDIF
    WEND
    PRINT "Test WHILE OVER."
    FOR X = 1 TO 100
        PRINT X; X/2; X; X*X
        IF X > 80 THEN
           PRINT "hello break"
           BREAK
        ELSEIF X > 50 THEN
           PRINT "hello continue"
           CONTINUE
        ENDIF
    NEXT
    PRINT X
    GOSUB 300
    PRINT "hello"
    INPUT H
    IF H<11 THEN 
      GOTO 200 
    ENDIF
    PRINT 12-4/2
    PRINT 100
    SELECT CASE H
       CASE 5
            PRINT "5 of H = "; H*H
            BREAK
       CASE 11
            PRINT "11 of H = "; H*H
            BREAK
    END SELECT
    RETURN
200 A = 100/2
    IF A>10 THEN 
       PRINT "this is ok" 
    ENDIF
    PRINT A
    PRINT A+34
    INPUT H
    PRINT H
    INPUT "this is as test ",Y
    PRINT H+Y
    END
300 PRINT "this is a subroutine"
    RETURN
END SUB 

SUB LujiamingSub ()
    LOOP 5
       PRINT "Test LujiamingSub."
    ENDLOOP
    RETURN 50.6
END SUB


SUB LujiamingParamsSub (X, Y, Z)
    LOOP 5
        PRINT "Test LujiamingSub.", X; Y; Z; ""
       
        MOVEL 11.1 2.2 3.3 4.4 5.5 6.6 50 CNT;ACC 40 ;OFFSET J_VEC (100, 0, 0, 45, 0, 0);TOOL_OFFSET J_VEC (100, 0, 0, 45, 0, 0);TA 100 ASSIGN R[1] = 5 + ( 6 - 7 ) ;TB 100 CALL prog_demo_dec::prog_1 (1, 2)
        MOVEL 12.1 2.2 3.3 4.4 5.5 6.6 30 SD 50 ;ACC 40 ;OFFSET C_VEC_UF (0, 0, 100, 40.5, 0, 0) UF[1];TOOL_OFFSET C_VEC_UF (0, 0, 100, 40.5, 0, 0) UF[1]
        MOVEL 13.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 ;OFFSET PR_UF PR[2] UF[1];TOOL_OFFSET PR_UF PR[2] UF[1]
 
        MOVEJ 14.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 
        MOVEJ 15.1 2.2 3.3 4.4 5.5 6.6 50 CNT;ACC 40 ;EV 40 ;Ind_EV 40 
        MOVEJ 16.1 2.2 3.3 4.4 5.5 6.6 30 SD 50 ;ACC 40 ;TA 100 ASSIGN R[1] = 5 + ( 6 - 7 ) ;TB 100 CALL prog_demo_dec::prog_1 (1, 2)
        MOVEJ 17.1 2.2 3.3 4.4 5.5 6.6 100 SV 50 ;ACC 40 ;DB 100 ASSIGN R[1] = 5 + ( 6 - 7 ) 
        
        MOVEC 18.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 10.10 11.11 12.12 50 CNT;ACC 40 
        MOVEC 19.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 10.10 11.11 12.12 30 SD 50 ;ACC 40 
        MOVEC 10.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 10.10 11.11 12.12 100 SV 50 ;ACC 40 
        
#        moveL 1 0.4 0 0.45 0 0 3.141593
#        WAIT 100 8000
#        moveL 2 0.25 0.15 0.3 0 0 3.141593
#        WAIT 100 8000
#        moveL 3 0.25 -0.15 0.3 0 0 3.141593
#        WAIT 100 8000
#        moveL 4 0.55 -0.15 0.6 0 0 3.141593
#        WAIT 100 8000
#        moveL 5 0.55 0.15 0.6 0 0 3.141593
#        WAIT 100 8000 
    ENDLOOP
    RETURN 44.6
END SUB

