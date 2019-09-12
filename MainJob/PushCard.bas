SUB main ()
    BLDC_CTRL 127
    # prevent endless pushing card
    WAIT COND RI[1] = 1 3 SKIP
    WAIT 0.2
    BLDC_CTRL 0
    # check if the card is really pushed out
    IF RI[2] = 1 THEN
         DO[505] = 0
    ELSE
         DO[505] = 1
    ENDIF
END SUB 
