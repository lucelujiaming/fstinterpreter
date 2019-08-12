IMPORT PushCard
SUB main ()
   DO[501] = 0
   DO[502] = 0
   DO[503] = 0
   DO[504] = 0
   DO[506] = 0
   DO[507] = 0
   OVC. = 50
   OAC. = 100
   MOVEJ PR[2], 50 CNT -1
   IF RI[1] = 0 OR RI[2] = 0 THEN
       CALL PushCard::main()
   ENDIF
   DO[505] = 0
   #MH[1] = 730
   #MH[2] = -400
   #MH[3] = 500
   #MH[4] = 725
   #MH[5] = 4
   DO[506] = 1
   WAIT 1
