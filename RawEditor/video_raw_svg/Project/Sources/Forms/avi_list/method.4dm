Case of 
	: (FORM Event:C1606.code=On Load:K2:1)
		// scan folder, update table data, load in listbox
		
		Form:C1466.list:=New collection:C1472
		
		If (ds:C1482.Settings.getCount()=0)
			ALERT:C41("please select root first")
			CANCEL:C270
		Else 
			
			var $folder : 4D:C1709.Folder
			$folder:=Folder:C1567(ds:C1482.Settings.all().first().root; fk platform path:K87:2).folder("avi")
			If ($folder.exists)
				$files:=$folder.files()
				var $file : 4D:C1709.File
				
				For each ($file; $files)
					If ($file.extension=".avi")
						$datas:=ds:C1482.Games.query("avi_name=:1"; $file.name)
						If ($datas.length=0)
							$data:=ds:C1482.Games.new()
							$data.avi_name:=$file.name
							$data.filesize:=$file.size
							$data.save()
						Else 
							$data:=$datas.first()
							If ($data.filesize#$file.size)
								$data.filesize:=$file.size
								$data.parts:=New object:C1471
								$data.converted:=False:C215
								$data.segmented:=False:C215
								$data.framecount:=0
								$data.save()
							End if 
						End if 
						
						Form:C1466.list.push(New object:C1471("name"; $data.avi_name; "size"; Round:C94($data.filesize/1024/1024; 1); \
							"frames"; $data.framecount; "converted"; $data.converted; "segmented"; $data.segmented; "game"; $data))
						
					End if   // .avi
				End for each 
			Else 
				ALERT:C41("Folder root does not contain folder avi - wrong root selected?")
				CANCEL:C270
			End if 
			
			
		End if 
		
End case 