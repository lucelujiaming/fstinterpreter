SUB main (param0, param1)
    PRINT "hello"
    INPUT H
    IF H < 10 THEN 
      GOTO test_goto::200 
	ELSE 
	  IF H > 20 THEN 
        GOTO test_goto::300 
      ENDIF
    ENDIF
100 PRINT "this is END" 
    NOP
200 PRINT "this is H < 10" 
    END
300 PRINT "this is a H > 20"
    END
END SUB 


