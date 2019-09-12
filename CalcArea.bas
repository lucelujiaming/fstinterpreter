SUB main ()
#   XV = MR[20]
#   YV = MR[21]
#   ZV = MR[22]
   
   FLAG1 = (XV - 403 - 222)^2 + YV^2
   IF FLAG1 <= 695^2 THEN
       FLAG1 = TRUE
   ELSE
       FLAG1 = FALSE
   ENDIF
   
   FLAG2 =  (XV - 403)^2 + YV^2 
   IF FLAG2 <= (695 + 0.707 * 222 + 1000)^2 THEN
       FLAG2 = TRUE
   ELSE
       FLAG2 = FALSE
   ENDIF
   
   IF YV <= 725 THEN
       FLAG3 = TRUE
   ELSE
       FLAG3 = FALSE
   ENDIF
   
   FLAG4 = 1.5708 + ATAN(YV / (XV - 403)) 
   IF FLAG4 >= 0.524  THEN
       FLAG4 = TRUE
   ELSE
       FLAG4 = FALSE
   ENDIF
   
   IF XV >= -500 AND YV <= 300 AND XV <= 900 THEN
       FLAG5 = TRUE
   ELSE
       FLAG5 = FALSE
   ENDIF
   
   IF YV >= -695 AND YV <= 695 THEN
       FLAG6 = TRUE
   ELSE
       FLAG6 = FALSE
   ENDIF

   IF FLAG3 = TRUE THEN
      MH[1] = 725
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 2
   ELSEIF FLAG5 = TRUE THEN
      MH[1] = XV
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 4
   ELSEIF FLAG1 = TRUE THEN
      MH[1] = XV
	  MH[2] = YV
	  MH[3] = ZV
	  MH[4] = 725
	  MH[5] = 3
   ELSEIF FLAG2 = TRUE AND FLAG4 = TRUE AND FLAG6 = TRUE THEN
      MH[1] = 403 + 222 + SQRT(695^2 - YV^2)
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
