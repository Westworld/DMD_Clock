//%attributes = {}
// find empty lines on top or bottom

C_BLOB:C604($1)
C_OBJECT:C1216($2)
$bw:=$1
$data:=$2  // $data:=new object("width";128;"height";32;"ytop",0;"ybottom";0)

$bytesline:=$data.width/8
$pos:=0

$topempty:=0
$bottomempty:=0

For ($y; 1; $data.height)
	$color:=0
	For ($i; 1; $bytesline)
		$color+=$bw{$pos}
		$pos+=1
	End for 
	If ($color=0)
		$topempty+=1
	Else 
		break
	End if 
End for 

If ($topempty=$data.height)
	ALERT:C41("Totally empty picture?")
Else 
	$data.ytop:=$topempty
	$bottomempty:=0
	For ($y; $data.height-1; 0; -1)
		$color:=0
		$pos:=$y*$bytesline
		For ($i; 1; $bytesline)
			$color+=$bw{$pos}
			$pos+=1
		End for 
		If ($color=0)
			$bottomempty+=1
		Else 
			break
		End if 
	End for 
	$data.ybottom:=$bottomempty
End if 