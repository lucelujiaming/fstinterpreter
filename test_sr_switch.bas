IMPORT MarcuscTest
SUB main ()
    R[1]  = 1  
    SELECT CASE  R[1]  
        CASE  1 
        CASE  2 
        CASE  3 
            R[1]  = 0 
        BREAK  
        CASE  1
            R[1]  = 0  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
    SR[1]  = "Hello, World"  
    SELECT CASE  SR[1]  
        CASE  "Hello, World" 
            SR[1]  = "Hello"  
        BREAK 
        DEFAULT   
            CALL MarcuscTest::main () 
        BREAK 
    END SELECT 
    END 
END SUB 
