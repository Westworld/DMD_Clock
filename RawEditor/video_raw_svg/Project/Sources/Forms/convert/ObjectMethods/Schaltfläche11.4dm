If (ds:C1482.Settings.getCount()=0)
	$root:=ds:C1482.Settings.new()
	$root.root:=""
Else 
	$root:=ds:C1482.Settings.all().first()
End if 

$folder:=Select folder:C670("Select folder with 'AVI', 'RGB' and 'clips'"; $root.root; Use sheet window:K24:11)
If (OK=1)
	$root.root:=$folder
	$root.save()
End if 
