SUB main ()
    MR[1]  = 11  
    MR[2]  = 111.2222  
    MR[3]  = "66.66running"  
    MR[4]  = R[1]   
    MR[5]  =  DI[1]  
    MR[6]  =  RI[1]  
     DO[1] = OFF   
    MR[6]  =  DO[1]  
    MR[7]  = PR[1].PJ5   
    MR[8]  = R[1]  / 2.99  
    MR[9]  = R[1]  + 2.99  
    MR[10]  = R[1]  - 2.99  
    MR[11]  = R[1]   %  3  
    MR[12]  = R[1]  +  DO[1]  
    MR[1+R[2] ]  = R[1]  + SR[1]   
    MR[13]  = MR[1]   
    END 
END SUB 
