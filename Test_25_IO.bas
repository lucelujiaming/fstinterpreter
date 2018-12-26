SUB main ()
    WHILE 2 = 2 
        FOR X=1 TO 25 
            DI[X]  = ON   
            WAIT  1
        NEXT 
        WAIT  5
        FOR X=1 TO 25 
            DI[X]  = OFF   
            WAIT  1
        NEXT 
        WAIT  5
    WEND 
END SUB 
