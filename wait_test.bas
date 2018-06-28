SUB main (param0, param1)
    uf_no = 5  
    WHILE R[1] >= 30 
        MOVEJ 0, 0, 0, 0, -1.5708, 0, 0.5 CNT -1 ;ACC 100 
        MOVEL 400, -150, 425, 0, 0, 3.1416, 1000 CNT 0 ;ACC 100 
        R[1] = R[1] - 1  
    WEND 
END SUB 
