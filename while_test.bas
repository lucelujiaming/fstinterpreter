SUB main (param0, param1)
    WHILE MR[1] <= 90 
        MOVEJ 0, 0, 0, 0, -1.5708, 0, 0.5 CNT -1 ;ACC 100 
        MOVEL 400, -150, 425, 0, 0, 3.1416, 1000 CNT 0 ;ACC 100 
        MR[1] = MR[1] + 1  
    WEND 
END SUB 
