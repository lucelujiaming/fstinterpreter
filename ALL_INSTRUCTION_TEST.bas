IMPORT MarcuscTest
SUB main ()
    TIMER[1]  = START  
    MOVEL P[1], 250 CNT -1 
    MOVEJ PR[2], 6.25 SV 50 
    MOVEL PR[3], 250 CNT -1 
    MOVEL PR[MR[1] + 1], MR[2] CNT -1 
    R[1]  = 10  
    R[2]  = 8  
    R[3]  = TIMER[1]   
    IF R[1]  = ( 2+R[2]  ) THEN 
        WAIT COND R[1]  = 3 40 SKIP
    ENDIF 
    DI[1]  = OFF   
    IF DI[1]  = OFF  THEN 
        DI[1]  = PULSE  10  
    ELSE 
        DI[1]  = OFF   
        DO[1]  = OFF   
        RI[1]  = OFF   
        RO[1]  = OFF   
    ENDIF 
    WAIT COND DI[1]  = ON  15 Warning
    MR[2] = 2000  
    WHILE R[3]  = 2 
        R[1]  = 2 + PR[2].PJ1   
        R[3]  = R[3]  + 1  
    WEND 
    WAIT  R[4]
    WHILE MR[3] = 4 
        MR[3] = 1  
        SELECT CASE  MR[1] 
            CASE  2 
                MR[3] = MR[3] + 1  
            BREAK 
            CASE  3 
                MR[3] = MR[3] + 1  
            BREAK 
            DEFAULT   
                MR[3] = MR[3] + 1  
            BREAK 
        END SELECT 
    WEND 
    PAUSE 
    CALL MarcuscTest::main () 
    TF_NO. = 3  
    UF_NO. = 3  
    TF_NO. = 0  
    UF_NO. = 0  
    OVC. = 40  
    TIMER[1]  = STOP  
    R[3]  = TIMER[1]   
    TIMER[1]  = RESET  
    ABORT 
    END 
END SUB 
