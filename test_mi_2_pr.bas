IMPORT mi_2_pr
SUB main ()
    FOR X=1 TO 9 
        MI[X]  = -10000 * X  
    NEXT 
    MR[1] = 1  
    CALL mi_2_pr::main () 
    MR[1] = 2  
    CALL mi_2_pr::main () 
    MR[1] = 3  
    CALL mi_2_pr::main () 
END SUB 
