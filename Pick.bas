IMPORT MarcuscTest
SUB main ()
    MOVEJ PR[1], 100 CNT -1 
    DO[1]  = ON   
    DO[2]  = OFF   
    RO[1]  = ON   
    RO[2]  = OFF   
    WAIT  2
    CALL MarcuscTest::main () 
    END 
END SUB 
