pro pstopdf
;******************************************************************************************************************************
; Dieses Programm wurde geschrieben um die Postscriptfiles die von sar.pro (printmenu.pro) 
; erzaeugt werden in Pdf-Dokumente umzuwandeln. Die erzeugten Quicklooks werden in Ordner sortiert wo sie zur Weiterverarbeitung
; bereit liegen.
; Wenn das Program nicht vom Hauptprogram gestartet werden soll, muss der Befehl commen saving als Kommentart markiert 
; und speiort aus dem Kommentar rausgenommen werden.
;******************************************************************************************************************************

common saving                                
;uebernimmt von sar.pro den Speicherort

  ;speiort='/pf/u/u241112/Arbeit/ERS1neu/'
  ;speiort='/users/ifmlinux20c/ifmrs/u241028/FEW3O/DeMarine/Vorlaufigeergebnisse/'  ;Speicherort der Daten (wenn das Programm ohne 
  cd, speiort
  ;cd,'/users/ifmlinux20c/ifmrs/u241028/FEW3O/DeMarine/Vorlaufigeergebnisse'
  print,'Bin im dem Ordner '+speiort
  
for ersnr = 1,2,1 do begin 
  print, ersnr
  ordner='ERS'
  ordner=ordner+strtrim(ersnr,1)+'pdf/'  
  print, ordner
  if (ersnr eq '2') then begin
    cd, speiort
   endif
  cd,ordner
  spawn, 'pwd', ort
  print, ort
  spawn, 'ls *.ps4', neuedaten
  n_neuedaten=N_ELEMENTS(neuedaten)
  print, n_neuedaten
   if (n_neuedaten ne '1') then begin    
    spawn,'ls *.ps4', namepdfvar
      print, namepdfvar
      b=N_ELEMENTS(namepdfvar)
      ;b=1
      print, b
        For laufvar = 0,b-1,1 do begin  
        name=strmid(namepdfvar(laufvar),0,22)
         print, 'Erstelle PDF nr. '
         print, laufvar+1
         print, namepdfvar(laufvar)
         print, name
         spawn,"ps2pdf12 -sPAPERSIZE=a4 "+name+".ps4 "+name+".pdf"
        endfor
        
   print, "Die Postscript-Files wurden zu Pdf-Files umgeschrieben."
   spawn,'rm *.ps4'                                         ;loescht die vorhandenen Postscript-Files (.ps4)
   print, "Die Postscript-Files wurden geloescht."
   spawn, 'chmod 777 ERS*'     ;damit die berechtigungen fuers verschieben da sind
   endif
endfor   
 print, 'Die erstellten Dateien wurden in pdf umgewandelt.'

end
