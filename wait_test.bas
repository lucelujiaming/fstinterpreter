IMPORT MarcuscTest
SUB main (param0, param1)
    WAIT COND R[1]  = 3 20 SKIP 
    WAIT  1
    WAIT COND R[1]  = 3 20 Warning 
    WAIT  1
    WAIT COND R[1]  = 3 20 CALL MarcuscTest::main ()
    WAIT  1
    WAIT COND  DI[1] = ON  25 SKIP 
    WAIT  1
    R[1]  = 1  
    WAIT COND  DI[1] = ON  25 Warning 
    WAIT  R[1]
    MR[1]  = 1  
    WAIT COND R[1]  = 7 15 CALL MarcuscTest::main ()
    WAIT  MR[1]
END SUB 
