SUB main ()
    TIMER[1]  = START  
    WAIT  1
    R[3]  = TIMER[1]   
    TIMER[1]  = RESET  
    WAIT  3
    R[3]  = TIMER[1]   
    TIMER[1]  = STOP  
    R[3]  = TIMER[1]   
END SUB 
