SUB main ()
    R[1]  = 0  
    R[2]  = 0  
    MOVEJ P[2], 100 CNT -1 
    MOVEJ P[2], 100 CNT -1 ;TOOL_OFFSET PR PR[2]
    MOVEJ P[2], 100 CNT -1 ;TOOL_OFFSET PR_TF PR[2] TF[0]
    MOVEJ P[2], 100 CNT -1 ;OFFSET PR_UF PR[2] UF[0]
    MOVEJ P[2], 100 CNT -1 ;OFFSET PR PR[2]
    MOVEJ P[2], 100 CNT -1 ;OFFSET J_VEC_UF (100, 0, 0, 0, 0, 0) UF[0]
    MOVEJ P[2], 100 CNT -1 ;OFFSET J_VEC (50, 100, 150, 200, 0, 0)
    MOVEJ P[2], 100 CNT -1 ;TOOL_OFFSET J_VEC_TF (50, 100, 150, 200, 0, 0) TF[0]
    MOVEJ P[2], 100 CNT -1 ;TOOL_OFFSET J_VEC (50, 100, 150, 200, 0, 0)
    R[1]  = 1  
    R[2]  = R[1]  + 2  
    END 
END SUB 
