1. SUB main ()
2.     BLDC_CTRL 253
3.     WAIT COND RI[1] = 0 3 SKIP
4.     IF RI[1] = 0 AND RI[2] = 0 THEN
5.         WAIT 0.2
6.         DO[505] = 1
7.     ENDIF
8.     BLDC_CTRL 0
9. END SUB 
