SUB main (param0, param1)
    MOVEJ 0, 0, 0, 0, -1.5708, 0, 0.5 SV 100 ;ACC 100 
    CALL call_test::func_test (1) 
    CALLMACRO call_test::func_test (1) 
    CALL call_test::func_noparam_test () 
END SUB 
SUB func_noparam_test ()
    MOVEL 400, -150, 425, 0, 0, 3.1416, 1000 CNT 0 ;ACC 100 
END SUB 
SUB func_test (param0)
    MOVEL 400, -150, 425, 0, 0, 3.1416, 1000 CNT 0 ;ACC 100 
END SUB 
