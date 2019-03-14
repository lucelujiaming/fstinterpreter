IMPORT suckcard
IMPORT tk
SUB main ()
	# 初始化交互信号
	DO[8] = OFF
	DO[9] = OFF
	DO[501] = OFF
	DO[502] = OFF
	DO[503] = OFF
	DO[504] = OFF
	DO[505] = OFF
	DO[506] = OFF
	DO[507] = OFF
	DO[605] = OFF
	
	# 跳转Label
	WHILE 1 == 1
		# 运动至起始点
		# mov2 
		MOVEJ PR[99], 100 CNT 0
		# card 
		# 取卡位1
		IF DO[601] == ON THEN
			MOVEJ PR[1], 100 CNT 0
			DO[501] = ON 
			# 调用吸卡子程序
			CALL suckcard::main () 
			MOVEJ PR[11], 100 CNT 0
			MOVEJ PR[12], 100 CNT 0
			MOVEJ PR[13], 100 CNT 0
			# WAIT M#(605)==ON DT=0 CT=0
			WAIT COND DO[605]  = ON
			# card N24 
			# 初始化递卡位数据
			PR[24] = PR[23]
			PR[24].PJ1  = MI[4] / 1000 
			PR[24].PJ2  = MI[2] / 1000 
			PR[24].PJ3  = MI[3] / 1000
			# 运动至递卡位 
			MOVEL PR[24], 1500 CNT 0
			PR[24].PJ1  = MI[1] / 1000 
			MOVEL PR[24], 1500 CNT 0
			DO[507] = ON
			# 调用吐卡子程序
			CALL tk::main () 
			DO[507] = OFF 
			PR[24].PJ1  = MI[4] / 1000 
			MOVEL PR[4], 1500 CNT 0
			R[5] = PR[4].PJ4
			if R[5] < 0.0001 THEN
				MOVEL PR[23], 100 CNT 0
				MOVEL PR[22], 100 CNT 0
			ENDIF
			
			MOVEL P[1], 100 CNT 0
			MOVEL PR[99], 100 CNT 0
		ENDIF
	WEND
END SUB 