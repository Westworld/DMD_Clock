SET TIMER:C645(0)
$data:=New object:C1471("game"; New object:C1471)
$win:=Open form window:C675("avi_list")
DIALOG:C40("avi_list"; $data)
If (String:C10($data.rgb)#"")
	Form:C1466.game:=$data.game.game
	
	Form:C1466.info:=Form:C1466.game.avi_name
	Form:C1466.rgb:=$data.rgb
	Form:C1466.frame:=1
	
	LoadFrame()
	
	
	If (Form:C1466.game.parts#Null:C1517) && (Not:C34(OB Is empty:C1297(Form:C1466.game.parts)))
		Form:C1466.clips:=Form:C1466.game.parts.col
	Else 
		Form:C1466.clips:=New collection:C1472
	End if 
	
End if 
