//%attributes = {}
var $file; $target : 4D:C1709.File
$file:=Folder:C1567(fk resources folder:K87:11).file("timezones.json")
If ($file.exists)
	$root:=Convert path system to POSIX:C1106(ds:C1482.Settings.all().first().root)
	$source:=$root+"/TimeZones/"
	
	$text:=$file.getText("UTF-8")
	$data:=JSON Parse:C1218($text)
	$group:=""
	$entries:=""
	For each ($entry; $data)
		$parts:=Split string:C1554($entry; "/")
		If ($parts[0]#$group)
			If ($entries#"")
				$target:=File:C1566($source+$group+".txt")
				$target.setText($entries; "UTF-8"; Document with LF:K24:22)
			End if 
			$entries:=""
			$group:=$parts[0]
		End if 
		
		$line:=$parts[1]
		If ($parts.length>2)
			$line+=("/"+$parts[2])
		End if 
		$line+=Char:C90(9)
		$line+=($data[$entry]+Char:C90(10))
		$entries+=$line
		
	End for each 
	If ($entries#"")
		$target:=File:C1566($source+$group+".txt")
		$target.setText($entries; "UTF-8"; Document with LF:K24:22)
	End if 
	
Else 
	ALERT:C41("timezones file missing")
	// get it from https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.json
End if 