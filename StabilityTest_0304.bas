IMPORT Pick
SUB main ()
    R[1]  = 1  
    R[2]  = 1  
    WHILE R[1]  <= 9 
        MR[R[1] ]  = R[1]  + 0  
        R[1]  = R[1]  + 1  
    WEND 
    R[1]  = 1  
    MOVEJ P[1], 100 CNT -1 
    WHILE R[1]  < 4 
        SELECT CASE  R[1]  
            CASE  1 
                WHILE R[2]  <= 9 
                    TF_NO. = 1  
                    UF_NO. = 1  
                    MR[1]  = R[2]   
                    MOVEJ PR[MR[1]], 100 CNT -1 
                    DO[1]  = OFF   
                    DO[2]  = ON   
                    WAIT  5
                    R[2]  = R[2]  + 1  
                    CALL Pick::main () 
                WEND 
                R[1]  = R[1]  + 1  
                R[2]  = 1  
            BREAK 
            CASE  2 
                WHILE R[2]  <= 3 
                    TF_NO. = 2  
                    UF_NO. = 2  
                    CALL Pick::main () 
                    MOVEL P[2], 4000 CNT -1 
                    DO[1]  = OFF   
                    DO[2]  = ON   
                    WAIT  2
                    R[2]  = R[2]  + 1  
                WEND 
                R[1]  = R[1]  + 1  
            BREAK 
        END SELECT 
        TF_NO. = 3  
        UF_NO. = 3  
        IF R[1]  = 3 THEN 
            CALL Pick::main () 
            MOVEL P[3], 4000 CNT -1 
        ENDIF 
        IF R[1]  = 3 THEN 
            R[1]  = 1  
        ENDIF 
    WEND 
    END 
END SUB 
