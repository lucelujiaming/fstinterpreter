SUB main ()
    BLDC_CTRL 255
    WAIT COND RI[1] = 1 1 SKIP
    IF RI[1] = 1 THEN
       WAIT 0.3       
    ENDIF
    BLDC_CTRL 0
END SUB 
