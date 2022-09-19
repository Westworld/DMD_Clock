If (Form:C1466.clips.length=0)
	ALERT:C41("Create new clip line first")
End if 

If (Form:C1466.theclip=Null:C1517)
	ALERT:C41("Select clip line first")
Else 
	Form:C1466.theclip.from:=Form:C1466.frame
	Form:C1466.theclip:=Form:C1466.theclip
End if 