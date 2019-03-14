SUB main ()
    SKR[1]  = "sr1 test function "  
    SKR[2]  = "test"  
    SKR[3]  = SKR[1]   
    SKR[4]  = SKR[1]  + SKR[1]   
    SKR[4]  = SKR[1]  + R[1]   
    SKR[4]  = SKR[1]  + MR[1]   
    R[1]  = 1  
    R[2]  = StrLen(SKR[1] )  
    R[3]  = FindStr(SKR[1] , SKR[2] )  
    SKR[5]  = SubStr(SKR[3] , R[1] , R[2] )  
    SKR[6]  = "sr test function"  
    END 
END SUB 
