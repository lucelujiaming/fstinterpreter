SUB main ()
    BLDC_CTRL 253
    WAIT COND RI[1] = 0 3 SKIP
    IF RI[1] = 0 AND RI[2] = 0 THEN
        WAIT 0.2
        DO[505] = 1
    ENDIF
    BLDC_CTRL 0
END SUB 
