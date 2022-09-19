If (Form:C1466.game#Null:C1517)
	Form:C1466.game.parts:=New object:C1471("col"; Form:C1466.clips)
	Form:C1466.game.segmented:=True:C214
	Form:C1466.game.save()
	$root:=Convert path system to POSIX:C1106(ds:C1482.Settings.all().first().root)
	$source:=$root+"/RGB/"+Form:C1466.game.avi_name+".rgb"
	
	var $file : 4D:C1709.File
	var $blob : 4D:C1709.Blob
	For each ($clip; Form:C1466.clips)
		$target:=$root+"/clips/"+$clip.name+".rgb"
		$file:=File:C1566($target; fk posix path:K87:1)
		If ($file.exists)
			$file.delete()
		End if 
		
		// using new file handlers - syntax?
		$frame:=128*32*3
		$refin:=Open document:C264(Convert path POSIX to system:C1107($source); ""; Read mode:K24:5)
		$refout:=Create document:C266(Convert path POSIX to system:C1107($target))
		
		SET DOCUMENT POSITION:C482($refin; ($clip.from-1)*$frame; 1)
		$bytes:=($clip.till-$clip.from)*$frame
		RECEIVE PACKET:C104($refin; $blob; $bytes)
		SEND PACKET:C103($refout; $blob)
		
		CLOSE DOCUMENT:C267($refout)
		CLOSE DOCUMENT:C267($refin)
	End for each 
	
Else 
	ALERT:C41("Game undefined")
End if 