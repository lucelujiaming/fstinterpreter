SUB main (param0, param1)
    SELECT CASE  R[2]  
        CASE  5 
            MOVEJ 0, 0, 0, 0, -1.5708, 0, 0.5 SV 100 ;ACC 100 
            WAIT  1
            MOVEL 400, -150, 425, 0, 0, 3.1416, 1000 CNT 0 ;ACC 100 
        BREAK 
        DEFAULT   
            WAIT  1
        BREAK 
    END SELECT 
END SUB 
