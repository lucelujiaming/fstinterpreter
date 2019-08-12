IMPORT PullCard
IMPORT PushCard
SUB main ()
    OVC. = 30
    OAC. = 100
#    WAIT COND RI[1] = 1 AND RI[2] = 1
    DO[505] = 0
    WAIT 1
    WHILE 1
        WAIT COND DO[601] = 1 OR DO[602] = 1 OR DO[603] = 1 OR DO[604] = 1
        IF DO[601] = 1 THEN
              MR[1] = 1
   WEND
END SUB