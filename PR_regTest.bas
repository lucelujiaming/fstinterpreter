SUB main ()
    PR[1]  = P[1]   
    PR[2]  = J_POS  
    PR[3]  = L_POS  
    PR[6]  = PR[1]  + PR[2]   
    PR[7]  = PR[1]  - PR[2]   
    PR[8]  = PR[R[1] ]   
    R[10]  = 10  
    R[9]  = 1  
    PR[R[10] ].PJ1  = R[1]   
    PR[5].PJ6  = 400.68 - R[1]   
    MOVEJ PR[1], 100 CNT -1 
    MOVEJ PR[2], 5 CNT -1 
    MOVEJ PR[1], 5 SV 50 
    MOVEJ PR[2], 100 SD 100 
    MOVEL PR[3], 1000 CNT -1 
    MOVEL PR[4], 8 CNT -1 
    MOVEL PR[3], 3 SV 100 
    MOVEL PR[4], 2000 SD 75 
    MOVEC PR[2], PR[3], 1000 CNT -1 
    MOVEC PR[6], PR[7], 10 CNT -1 
    MOVEC PR[3], PR[2], 1000 SD 100 
    MOVEC PR[6], PR[7], 10 SV 50 
    END 
END SUB 
