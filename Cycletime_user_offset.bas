SUB main ()
    MI[1] = rand()
    IF PR[1] THEN
		MOVEJ P[1], 50 CNT -1 
	ENDIF
    fkj_one()
    fkj_two()
    MOVEJ P[1], 50 CNT -1 
    WHILE 1 
        MOVEL P[1], 1500 CNT -1 ;ACC 40 ;TOOL_OFFSET C_VEC_TF (100, 0, 0, 45, 0, 0) TF[1]
        MOVEL P[2], 650 CNT -1 ;ACC 40 ;TOOL_OFFSET PR_TF PR[2] TF[1]
        MOVEL P[2], 650 CNT -1 ;ACC 40 ;OFFSET PR PR[2]
        MOVEL P[2], 650 CNT -1 ;ACC 40 ;OFFSET PR_UF PR[2] UF[1]
        MOVEL P[3], 1500 CNT -1 ;ACC 40 ;OFFSET C_VEC_UF (100, 0, 0, 45, 0, 0) UF[1]
        MOVEL P[4], 650 CNT -1 ;ACC 40 ;OFFSET J_VEC (100, 0, 0, 45, 0, 0)
        MOVEL P[3], 650 CNT -1 ;ACC 40 ;TOOL_OFFSET PR PR[2]
        MOVEL P[4], 650 CNT -1 ;ACC 40 ;TOOL_OFFSET J_VEC (100, 0, 0, 45, 0, 0)
    WEND 
    END 
END SUB

