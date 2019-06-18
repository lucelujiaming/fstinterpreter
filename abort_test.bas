SUB main ()
    TIMER[1]  = START  
    PAUSE 
    WAIT  1
    R[3]  = TIMER[1]   
    TIMER[1]  = RESET  
    WAIT  3
    ABORT 
    R[3]  = TIMER[1]   
    TIMER[1]  = STOP  
    R[3]  = TIMER[1]   
END SUB 
