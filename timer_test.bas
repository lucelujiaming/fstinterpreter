SUB main ()
    TIMER[1]  = START  
    WAIT  1
    TIMER[1]  = RESET  
    WAIT  3
    TIMER[1]  = STOP  
    R[3]  = TIMER[1]   
END SUB 
