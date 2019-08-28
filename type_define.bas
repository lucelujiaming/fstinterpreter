Private Type Date 
	year  As Integer 
	month As Integer 
	day   As Integer 
End Type

Dim cLuceDate As Date

Private Type Person 
	name      As String 
	gender    As Integer 
	birthday  As Date 
End Type

Dim cPersonInfo As Person   

SUB main ()
    cLuceDate.year   = 2019
    cLuceDate.month  = 7 + MR[1]
    cLuceDate.year   = 2019 + cLuceDate.month
    cLuceDate.day    = 28
    MR[2]  = cLuceDate.day
    R[1]   = cLuceDate.month  
    END 
END SUB 
