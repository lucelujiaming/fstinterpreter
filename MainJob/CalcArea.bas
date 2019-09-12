IMPORT PushCard
SUB main ()
#   XV = MR[20]
#   YV = MR[21]
#   ZV = MR[22]
    XV = 200
    YV = 200
    ZV = 200
   
   FLAG1 = ( XV - 403 - 222 )^2 + YV^2
   IF FLAG1 <= 695^2 THEN
       FLAG1 = 1
   ELSE
       FLAG1 = 0
   ENDIF
   
   FLAG2 =  ( XV – 403 )^2 + YV^2 
   IF FLAG2 <= (695 + 0.707 * 222 + 1000)^2 THEN
       FLAG2 = 1
   ELSE
       FLAG2 = 0
   ENDIF
   
   IF YV <= 725 THEN
       FLAG3 = 1
   ELSE
       FLAG3 = 0
   ENDIF
   
   FLAG4 = 1.5708 + ATAN(YV / (XV - 403)) 
   IF FLAG4 >= 0.524  THEN
       FLAG4 = 1
   ELSE
       FLAG4 = 0
   ENDIF
   
   IF XV >= -500 AND YV <= 300 AND XV <= 900 THEN
       FLAG5 = 1
   ELSE
       FLAG5 = 0
   ENDIF
   
   IF YV >= -695 AND YV <= 695 THEN
       FLAG6 = 1
   ELSE
       FLAG6 = 0
   ENDIF

   IF FLAG3 = 1 THEN
      MH[1] = 725
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 2
   ELSEIF FLAG5 = 1 THEN
      MH[1] = XV
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 4
   ELSEIF FLAG1 = 1 THEN
      MH[1] = XV
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 3
   ELSEIF FLAG2 = 1 AND FLAG4 = 1 AND FLAG6 = 1 THEN
      MH[1] = 403 + 222 + SQRT(695^2 – YV^2)
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 3
   ELSE
      MH[1] = XV
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 1
   ENDIF

END SUB 
