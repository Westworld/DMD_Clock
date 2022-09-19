If (Form:C1466.game#Null:C1517)
	//$game:=ds.Games.query("avi_name=:1"; Form.game.name).first()
	$game:=Form:C1466.game.game
	$root:=Convert path system to POSIX:C1106(ds:C1482.Settings.all().first().root)
	If (Not:C34(Bool:C1537($game.converted)))
		$path:=Convert path system to POSIX:C1106(Get 4D folder:C485(Current resources folder:K5:16)+"ffmpeg")
		$in:=$path+" -i "+Char:C90(34)+$root+"/avi/"+$game.avi_name+".avi"+Char:C90(34)+" -vcodec rawvideo -pix_fmt rgb24 "
		$in+=(Char:C90(34)+$root+"rgb/"+$game.avi_name+".rgb"+Char:C90(34))
		$in2:=""
		$error:=""
		$out:=""
		LAUNCH EXTERNAL PROCESS:C811($in; $in2; $out; $error)
		$file:=File:C1566($root+"rgb/"+$game.avi_name+".rgb"; fk posix path:K87:1)
		If ($file.exists)
			$game.framecount:=$file.size/12288
			$game.converted:=True:C214
			$game.save()
			Form:C1466.rgb:=Convert path POSIX to system:C1107($root+"rgb/"+$game.avi_name+".rgb")
		Else 
			ALERT:C41("File not found?")
		End if 
		
	Else 
		Form:C1466.rgb:=Convert path POSIX to system:C1107($root+"rgb/"+$game.avi_name+".rgb")
	End if 
End if 