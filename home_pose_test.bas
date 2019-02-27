SUB main ()
    SR[1]  = "sr1 test function "  
    SR[2]  = "test"  
    SR[3]  = SR[1]   
    SR[4]  = SR[1]  + SR[1]   
    SR[4]  = SR[1]  + R[1]   
    SR[4]  = SR[1]  + MR[1]   
    R[1]  = 1  
    R[2]  = StrLen(SR[1] )  
    R[3]  = FindStr(SR[1] , SR[2] )  
    SR[5]  = SubStr(SR[3] , R[1] , R[2] )  
    SR[6]  = "sr test function"  
    END 
END SUB 
