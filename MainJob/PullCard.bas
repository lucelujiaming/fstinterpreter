SUB main ()
    BLDC_CTRL 221
    #prevent endless pulling
    WAIT COND RI[1] = 0 5 SKIP
    BLDC_CTRL 0
    # check if the card is really pulled in
    IF RI[1] = 0 AND RI[2] = 0 THEN
        DO[505] = 1
    ELSE
        DO[505] = 0
    ENDIF
END SUB 
