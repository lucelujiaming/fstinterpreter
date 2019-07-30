SUB main ()
    IF R[3]  AND R[1]  THEN 
        R[4]  = PR[1].PJ2   
        R[5]  = R[1]  + R[4]   
        R[6]  = R[1]  - R[4]   
        R[7]  = R[1]  * R[4]   
        R[8]  = R[1]  / R[4]   
    ENDIF 
            
    IF DO[601] THEN
          MR[1] = 1
    ELSEIF DO[602] = 1 THEN
          MR[1] = 2
    ELSEIF DO[603] = 1 THEN
          MR[1] = 3
    ELSEIF DO[604] = 1 THEN
          MR[1] = 4
    ELSE
          MR[1] = 0
    ENDIF
        
    MOVEJ P[1], 50 CNT -1 
    WHILE 1 
        MOVEL P[2], 650 CNT -1 
        MOVEL P[3], 1500 CNT -1 
        MOVEL P[4], 650 CNT -1 
        MOVEL P[3], 650 CNT -1 
        MOVEL P[2], 1500 CNT -1 
        MOVEL P[1], 650 CNT -1 
    WEND 
    END 
END SUB

