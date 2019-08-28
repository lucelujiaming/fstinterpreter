SUB main (param0, param1)
    R[5]  = MR[3]  * 1000 + R[5]
    WAIT 1
    R[3]  = 5
    WAIT 1
    MR[5]  = R[3]  * 1000 + MR[5]
    WAIT 1
    MR[3]  = 5
    WAIT 1
END SUB
