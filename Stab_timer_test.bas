SUB main ()
    TIMER[1]  = START  
    WAIT  5
    TIMER[2]  = START  
    R[3]  = TIMER[1]   
    R[4]  = TIMER[2]   
    R[5]  = R[4]  - R[3]   
END SUB 
