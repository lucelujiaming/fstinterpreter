SUB main ()
    BLDC_CTRL DIR=1 VEL=32
    R[1]  = 1234  
    SR[1]  = R[1]   
    R[1]  = 56.78  
    SR[2]  = R[1]   
    SR[3]  = "Hello, World"  
    SR[4]  = "llo"  
    R[2]  = StrLen(SR[3] )  
    R[3]  = FindStr(SR[4] , SR[3] )  
    R[4]  = SubStr(SR[2] , 2, 7)  
    END 
END SUB 
