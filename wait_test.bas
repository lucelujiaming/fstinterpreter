IMPORT MarcuscTest
SUB main (param0, param1)
    WAIT COND R[1]  = 3 20 SKIP 
    WAIT  5
    WAIT COND DI[1]  = ON  25 Warning 
    WAIT  5
    WAIT COND R[1]  = 7 15 CALL MarcuscTest::main ()
END SUB 
