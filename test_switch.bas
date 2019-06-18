IMPORT MarcuscTest
SUB main ()
     DO[1] = 1  
    SELECT CASE   DO[1] 
        CASE  1 
             DO[1] = 0  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
     RO[1] = 1  
    SELECT CASE   RO[1] 
        CASE  1 
             RO[1] = 0  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
     UO[1] = 1  
    SELECT CASE   UO[1] 
        CASE  1 
             UO[1] = 0  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
    R[1]  = 1  
    SELECT CASE  R[1]  
        CASE  1 
            R[1]  = 0  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
    MR[1]  = 1  
    SELECT CASE  MR[1]  
        CASE  1 
            MR[1]  = 0  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
    SR[1]  = "Hello, World"  
    SELECT CASE  MR[1]  
        CASE  "Hello, World" 
            SR[1]  = "Hello"  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
    END 
END SUB 
