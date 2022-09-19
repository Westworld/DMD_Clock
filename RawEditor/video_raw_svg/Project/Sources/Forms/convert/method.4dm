If (FORM Event:C1606.code=On Load:K2:1)
	C_PICTURE:C286($pict)
	mypicture:=$pict
	Form:C1466.frame:=1
	
End if 

If (FORM Event:C1606.code=On Timer:K2:25)
	//Form.frame+=1  // 3 frames per timer
	LoadFrame
End if 