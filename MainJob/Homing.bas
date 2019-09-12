IMPORT PushCard
SUB main ()
   DO[501] = 0
   DO[502] = 0
   DO[503] = 0
   DO[504] = 0
   DO[506] = 0
   DO[507] = 0
   OVC.= 100
   OAC. =100
   # move to the safety position
   MOVEJ PR[2], 50 CNT -1
   # push card out if there was one in the arm
   IF RI[1] = 0 OR RI[2] = 0 THEN
       CALL PushCard::main()
   ENDIF
   DO[505] = 0
   # if someon want to test without PLC, open the following annotation
   #MH[1] = 797
   #MH[2] = 197
   #MH[3] = 1000
   #MH[4] = 725
   #MH[5] = 4
   DO[506] = 1
   # make the program run at least 2 seconds, so PLC can have enough time to notice the process
   WAIT 2
END SUB 
