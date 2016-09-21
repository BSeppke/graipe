; NAME:
;	loadmenu.pro
;
;
; CALLING PROCEDURE:
;       sar.pro (ohne Parameter)
;
;
; PURPOSE:
;       Diese Prozedur ermoeglicht das Einlesen der Daten 
;          1. zum Kopieren von Tape auf Festplatte
;          2. zur Auswertung von Festplatte oder CD 
;
;
; COMMON BLOCKS:
;	BLOCK1:	Describe any common blocks here. If there are no COMMON
;		blocks, just delete this entry.
;
;
; MODIFICATION HISTORY:
; 	Written by:	Gerald Fiedler
;       03.04.1997:	Common Blocks eingefuegt, Filerequest funktioniert
;			Graf. Oberflaeche fertig
;	08.04.1997:	Headerdaten einlesen und darstellen
;	15.07.1997:	Lesen von CD-ROM moeglich
;	05.08.1997:	Einlesen der Daten von Tape auf Platte implementiert
;	23.09.1997:	v1.0,	Bugfix
;       09.02.1998:     v1.06   Einlesen des Headers aus Datenbank implementiert
;       09.04.1998:     v1.07   div. Bugfixes und Ausnahmeregelungen
;       16.07.1999:     v1.25   kann jetzt auch die SAR Quicklook-CDs lesen
;       13.08.1999:     v1.3    Windows 9x Unterstuetzung implementiert
 


; ---------------------------------------------------------------------------------------------
;  Thu Apr 23 13:04:50 MET DST 1998
;

pro MAIN88_Event, Event

common enter_widgets, E_FIELD3, E_BUTTON8
common load_widgets, FIELD18, FIELD24, FIELD34, FIELD35, SLIDER54, BGROUP21, BASE33, BASE60, BASE2
common var_names

WIDGET_CONTROL,Event.Id,GET_UVALUE=Ev

case Ev of
  'E_BUTTON8': begin                             ; 'Event for Done'
                 WIDGET_CONTROL,E_FIELD3,GET_VALUE=cdnr
                 file=cdnr+'/'+file
                 WIDGET_CONTROL,FIELD24,SET_VALUE=file
                 WIDGET_CONTROL, event.top, /DESTROY
               end
  'BUTTON10':  begin                            ; 'Event for Cancel'
                 WIDGET_CONTROL, event.top, /DESTROY
               end
endcase
end

pro enter, GROUP=Group
  common enter_widgets

  if (N_ELEMENTS(Group) eq 0) then GROUP=0
  junk   = { CW_PDMENU_S, flags:0, name:'' }
  MAIN88    = WIDGET_BASE(GROUP_LEADER=Group,ROW=1,MAP=1,TITLE='Enter Nr.',UVALUE='MAIN88')
  BASE2     = WIDGET_BASE(MAIN88,COLUMN=1,MAP=1,TITLE='Enter CD-Nr.',UVALUE='BASE2')
  E_FIELD3  = CW_FIELD( BASE2,VALUE='',ROW=1,STRING=1,TITLE='CD-Nr.:',UVALUE='',XSIZE=10)
  BASE6     = WIDGET_BASE(BASE2,ROW=1,MAP=1,TITLE='Buttons',UVALUE='BASE6')
  E_BUTTON8 = WIDGET_BUTTON( BASE6,UVALUE='E_BUTTON8',VALUE='Done',XSIZE=60)
  BUTTON10  = WIDGET_BUTTON( BASE6,UVALUE='BUTTON10',VALUE='Cancel',XSIZE=60)

  WIDGET_CONTROL, MAIN88, /REALIZE
  XMANAGER, 'MAIN88', MAIN88
end



pro MAIN1_Event, Event

common load_widgets
common enter_widgets
common main_widgets
common var_env
common var_names
common var_header
common var_data
common database
common load_path
common variable
common titel


WIDGET_CONTROL,Event.Id,GET_UVALUE=Ev
 
case Ev of

  'BGROUP21': begin
      case Event.Value OF
          0: begin                  ; Print,'Button Harddisk Pressed'
              	ecd='E'
              	if (!version.os_family eq 'unix') then WIDGET_CONTROL,FIELD18,SET_VALUE=''
             end

          1: begin                  ; Print,'Button CD Pressed'
             	ecd='CD'
				if (!version.os_family eq 'unix') then WIDGET_CONTROL,FIELD18,SET_VALUE='/cdrom/'
             end
          ELSE: Message,'Unknown button pressed'
      endcase
      end

  'SLIDER54': begin
      Print, 'Event for Resolution'
      end

  'BGROUP58': begin
      case Event.Value OF
      0: modus=0					; Print,'Button Complete Pressed'
      1: modus=1					; Print,'Button Header only Pressed'
      2: modus=2					; Print,'Button SubImage Pressed'
      ELSE: Message,'Unknown button pressed'
      endcase
      end
      
      
;----------------------------------------------------Filerequest--------------------------------------------------------------------------------------------------
 
 
 
  'BUTTON51': begin							; Filerequest
	WIDGET_CONTROL,BGROUP21,GET_VALUE=cd
	if (!version.os_family eq 'unix') then begin        ; auf der Sun
            if (cd eq 1) then begin
                WIDGET_CONTROL, /HOURGLASS
                spawn,'pwd',currentdir
                lw=string(currentdir(0))                ; merken, wo ich bin
                WIDGET_CONTROL,BUTTON177,GET_VALUE=mount
                if (mount eq 'Mount') then begin
                    print,'mounte CD-ROM...'
                    spawn,'npmount hsfs'                 ; CD-ROM mounten
                    Widget_Control,BUTTON177,Sensitive=1 ; Mount
                    WIDGET_CONTROL,BUTTON177,SET_VALUE='Unmount'
                endif
                cd,'/cdrom/'
		spawn,'ls',cddir       	; CD-Name einlesen 
		cd,cddir(1)		
                print,'lese Directory...'
                complete_file=DIALOG_PICKFILE(/READ, FILTER = '*')
                print,'Fertig.'
            endif else begin
                cd,lw_path
                ;complete_file=DIALOG_PICKFILE(/READ, FILTER = '*.sar')
                ;complete_file='/pf/u/u241112/'+namevar(laufvar) 
                complete_file=lw_path+namevar(laufvar) ; Hier wurde die Variable eingetragen !!! 
                print, complete_file 
                print, lw_path
            endelse
        endif else begin                                 ; wenns unter Windows95 laeuft
            if (cd eq 1) then begin
                 complete_file=DIALOG_PICKFILE(/READ, FILTER = 'Dat*')
            endif else begin
                complete_file=DIALOG_PICKFILE(/READ, FILTER = '*.sar')
            endelse
        endelse

	if (!version.os_family eq 'unix') then begin        ; auf der Sun
            slash=strpos(complete_file,'/')
            fname=strmid(complete_file,slash+1,strlen(complete_file)) ; Filename
            fpath=strmid(complete_file,0,slash+1) ; Pfadname
            lw_path=fpath       ; Pfad merken fuer naechsten PICKFILE
            
            if (complete_file eq '/cdrom/PRI_16598_0405') then begin
                fpath='/cdrom/SCENE1/'
            endif
            WIDGET_CONTROL,FIELD18,SET_VALUE=fpath
            
            if (cd eq 0) then begin ; Festplatte
                file=strmid(fname,0,strpos(fname,'.'))
                WIDGET_CONTROL,FIELD24,SET_VALUE=file
            endif else begin    ; CD
                if (complete_file eq '/cdrom/PRI_16598_0405') then begin
                    cdnr='193'
                    file='1'
                    file=cdnr+'/'+file
                    WIDGET_CONTROL,FIELD24,SET_VALUE=file
                endif else begin
                    if (strupcase(strmid(fpath,7,5)) eq 'SCENE') then begin ; SAR-PRI
                        file=strmid(complete_file,slash-1,1) ; wievieltes Bild auf dieser CD
                        enter   ; Prozedur zum Einlesen der Cd-Nr.
                    endif else begin ; SAR-QL
                        file=strmid(fname,1,4) ; wievieltes Bild auf dieser CD
                        cdnr=strmid(fname,5,3) ; CD-Nummer
                        file=cdnr+'/'+file
                        WIDGET_CONTROL,FIELD24,SET_VALUE=file
                    endelse
                endelse
            endelse
        endif else begin        ; Windows 9x
            
            backslash=rstrpos(complete_file,'\')
            fname=strmid(complete_file,backslash+1,strlen(complete_file)) ; Filename
            fpath=strmid(complete_file,0,backslash+1) ; Pfadname
            lw_path=fpath       ; Pfad merken fuer naechsten PICKFILE
            
            WIDGET_CONTROL,FIELD18,SET_VALUE=fpath
            
            if (cd eq 0) then begin ; Festplatte
                file=strmid(fname,0,strpos(fname,'.'))
                WIDGET_CONTROL,FIELD24,SET_VALUE=file
            endif else begin    ; CD
                if (strupcase(strmid(fpath,3,5)) eq 'SCENE') then begin ; SAR-PRI
                    file=strmid(complete_file,backslash-1,1) ; wievieltes Bild auf dieser CD
                    enter       ; Prozedur zum Einlesen der Cd-Nr.
                endif else begin ; SAR-QL
                    file=strmid(fname,1,4) ; wievieltes Bild auf dieser CD
                    cdnr=strmid(fname,5,3) ; CD-Nummer
                    file=cdnr+'/'+file
                    WIDGET_CONTROL,FIELD24,SET_VALUE=file
                endelse
            endelse
        endelse
    end

  'BUTTON44': begin							; starte Einlesen
  print, 'load'
      WIDGET_CONTROL, /HOURGLASS
      WIDGET_CONTROL,BGROUP21,GET_VALUE=cd

      produkt='PRI'
      WIDGET_CONTROL,FIELD18,GET_VALUE=fpath
      WIDGET_CONTROL,FIELD24,GET_VALUE=filename
      fpath=fpath(0)
      filename=filename(0)

      if (fpath ne '') and (filename ne '') then begin
          if (cd eq 1) then begin
              slash=strpos(filename,'/')
              cdnr=strmid(filename,0,slash)
              file=strmid(filename,slash+1,strlen(filename)-slash)
              
              WIDGET_CONTROL,BUTTON177,GET_VALUE=mount
              if (!version.os_family eq 'unix') and (mount eq 'Mount') then begin
                  spawn,'pwd',currentdir
                  lw=string(currentdir(0)) ; merken, wo ich bin
                  print,'mounte CD-ROM...'
                  spawn,'npmount hsfs' ; CD-ROM mounten
                  Widget_Control,BUTTON177,Sensitive=1 ; Mount
                  WIDGET_CONTROL,BUTTON177,SET_VALUE='Unmount'
              end

              if ((!version.os_family eq 'unix')    and (strupcase(strmid(fpath,7,5)) eq 'SCENE')) or $
                 ((!version.os_family eq 'Windows') and (strupcase(strmid(fpath,3,5)) eq 'SCENE')) then begin              ; nur SAR-PRI
                  
                  if (filename eq '193/1') then fpath='/cdrom/'

                  cd,fpath

                  files=findfile('*',COUNT=count) ; Verzeichnis einlesen

                  if (!version.os_family eq 'Windows') and (count eq 6) then begin
                      tmp=files
                      files=strarr(4)
                      files=tmp(2:5)
                      count=4
                  endif
                  if (count eq 4) then begin ; wenn genau 4 Eintraege...

                      fa=''
                      for i=0,3 do begin
                          fa=fa+strmid(files(i),0,1) ; Anfangsbuchstaben der Files einlesen
                      endfor
                      if (strupcase(fa) eq 'DLNV') then begin ; wenn Standard-'DLNV', dann
                          file0=files(3) ; eingelesene Files zuordnen
                          file1=files(1)
                          file2=files(0)
                      endif else begin ; ansonsten: Fehler!
                          file0=''
                          file1=''
                          file2=''
                          print,'Konnte Files nicht identifizieren!'
                      endelse
                  endif else if (filename eq '193/1') then file2=files(0)   ; fuer CD 193 (nur 1 Datenfile, keine Headerfiles)
                      
                  file3=cdnr+'_'+file+'.ps'
                  ecd='CD'
                

              endif else begin  ; SAR-QL
                  file0='S'+file+cdnr+'.txt'
                  file2='S'+file+cdnr+'.ql'
                  file3='S'+file+cdnr+'.ps'
                  ecd='QL'
                  produkt='QL'
              endelse

          endif else begin      ; von Harddisk einlesen
              if (strupcase(strmid(fpath,strlen(fpath)-7,5)) eq 'SCENE') then begin ;  CD auf Platte kopiert
                  print,fpath
                  
                  stop
                

              endif else begin  ; Exabyte auf Platte kopiert
                  file0=file+'.h1'
                  file1=file+'.h2'
                  file2=file+'.sar'
                  file3=file+'.ps'
                  ecd='E'
              endelse
          endelse

          uline=strpos(filename,'_')
          if (uline ne -1) then strput,filename,'/',uline

          file=filename
;          exabyte=ecd+file
          exabyte=file


          if (ecd ne 'QL') then begin            ; QL nicht in Datenbank

              ; -----Variable 'exabyte' so formatieren, dass sie sich als Suchindex eignet--------------
              tape_form=exabyte
              if (strmid(tape_form,0,1) eq 'E') then begin
                  tape_form=' '+tape_form
                  tl=strlen(tape_form)+1
                  if (uline ne -1) then tl=tl-(strlen(tape_form)-uline)+2
              endif else begin
                  tl=strlen(tape_form)-1
              endelse

              for sp=0,7-tl do tape_form=strmid(tape_form,0,2)+' '+(strmid(tape_form,2,strlen(tape_form)-2))

              ; ------------- Pruefen, ob schon in Datenbank erfasst --------------------------------

              restore,dbpath    ; initialisiert in sar.pro

              s=size(datenbank) ; Infos �ber Datenbank einlesen
              db_laenge=s(1)     ; L�nge eines Datensatzes
              db_zaehler=s(2)   ; Anzahl Datens�tze
              db_zaehler=fix(db_zaehler) ; Long -> Int
              print,'Datenbank eingelesen ('+str(db_zaehler)+' Saetze).'
; ----------------------------------Datenbank eintrag nur noch auf den Namen reduzieren (SAR_ORBIT_FRAME)

print, tape_form
print, datenbank(0,1118)




              new=1             ; ist dieser Datensatz schon erfasst?
 ;             if (!version.os_family eq 'unix') then begin
                  i=0
                  while (i lt db_zaehler-1 and tape_form ne datenbank(0,i)) do i=i+1
                  if (i ne db_zaehler-1) then new=0 & index=i
                  if (i eq db_zaehler-1) then $
                  if (tape_form ne datenbank(0,i)) then new=1 else new=0 & index=i
 ;             endif
              if (new eq 0) then begin ; Daten schon vorhanden
                  print,'Lese Header aus Datenbank.'
                  tape_form         = datenbank( 0,index)
                  title             = datenbank( 1,index)
                  var_orbit         = datenbank( 2,index)
                  var_frame         = datenbank( 3,index)
                  date              = datenbank( 4,index)
                  time              = datenbank( 5,index)
                  var_platform      = datenbank( 6,index)
                  var_heading       = datenbank( 7,index)
                  var_linelength    = datenbank( 8,index)
                  var_numberoflines = datenbank( 9,index)
                  var_offset        = datenbank(10,index)
                  center_lon        = datenbank(11,index)
                  center_lat        = datenbank(12,index)
                  orient            = datenbank(13,index)
                  region            = datenbank(14,index)
                  var_lat_lon(0:7)  = datenbank(15:22,index)
                  version           = datenbank(23,index)
                  PS_CD             = datenbank(24,index)
   ;               produkt           = datenbank(25,index)

                  if (strpos(center_lat,'N') eq -1) then begin
                      var_lat_lon(8)='-'+strtrim(strmid(center_lat,0,strlen(center_lat)-2),2)
                  endif else begin
                      var_lat_lon(8)=strmid(center_lat,0,strlen(center_lat)-2)
                  endelse
                  if (strpos(center_lon,'E') eq -1) then begin
                      var_lat_lon(9)='-'+strtrim(strmid(center_lon,0,strlen(center_lon)-2),2)
                  endif else begin
                      var_lat_lon(9)=strmid(center_lon,0,strlen(center_lon)-2)
                  endelse

                  noheading=0   ;in der Datenbank gibt es IMMER eine Flugrichtung.
              endif else begin  ; Daten aus Header einlesen
                  print,'Lese Header aus Header-Files ein.'

                  sarhead,file2,file0,file1,INPATH=fpath

                                ;--------------------------- Ascending oder Decending ? ----------------------------

                  if (string(var_heading) ne 'unknown') then begin
                      noheading=0
                      if (var_heading gt 90) and (var_heading lt 270) then orient=1 else orient=0
                  endif else begin
                      noheading=1
                  endelse

                                ;---------------------------- Date und Time extrahieren ----------------------------

                  if (var_aqu_dat ne 'unknown') then begin
                      date=strmid(var_aqu_dat,6,2)+'.'+strmid(var_aqu_dat,4,2)+'.'+strmid(var_aqu_dat,0,4)
                  endif else begin
                      date='DD.MM.YYYY'
                  endelse

                  if (var_aqu_time ne 'unknown') then begin
                      time=strmid(var_aqu_time,0,2)+':'+strmid(var_aqu_time,2,2)
                  endif else begin
                      time='HH:MM'
                  endelse
                                ;-------------------------- Center Coordinates: +/- -> NSEW  -----------------------

                  if (var_lat_lon(8)lt 0) then ns=' S' else ns=' N'
                  center_lat=string(abs(var_lat_lon(8)))+ns
                  if (var_lat_lon(9) lt 0) then ew=' W' else ew=' E'
                  center_lon=string(abs(var_lat_lon(9)))+ew

                  title='somewhere' ; Titel unbekannt


              endelse
          endif else begin                    ; Quicklook Header einlesen
              header1=' '
              openr,1,fpath+file0
              readf,1,header1
              readf,1,format='(a7,a8,a7,a8,a7,a8,a7,a8,a7,a8)',v0,v1,v2,v3,v4,v5,v6,v7,v8,v9
              close,1

              header1=strtrim(header1,2)
              if (strmid(header1,0,2) eq 'E2') then begin
                  var_platform=2
                  header1=strmid(header1,2,strlen(header1)-2)
              endif else begin
                  var_platform=1
              endelse

              date=strmid(header1,0,11)
              time=strmid(header1,12,8)
              header1=strmid(header1,50,strlen(header1)-50)
              var_frame=strmid(header1,strlen(header1)-10,4)
              var_orbit=strmid(header1,strlen(header1)-5,5)
              title=strmid(header1,0,strlen(header1)-13)
              region='Quicklook'

              var_lat_lon=strarr(10)
              var_lat_lon=[v2,v3,v4,v5,v6,v7,v8,v9,v0,v1]

              if (var_lat_lon(8)lt 0) then ns=' S' else ns=' N'
              center_lat=string(abs(var_lat_lon(8)))+ns
              if (var_lat_lon(9) lt 0) then ew=' W' else ew=' E'
              center_lon=string(abs(var_lat_lon(9)))+ew
              new=0             ; nicht in Datenbank aufnehmen
         
          endelse

          if (var_heading eq 0) and (var_lat_lon(1) ne 'unknown') then begin         ; ermitteln der Flugrichtung 

              xa=var_lat_lon(1) & xb=var_lat_lon(5)
              ya=var_lat_lon(0) & yb=var_lat_lon(4)

              x=xa-xb & y=ya-yb

              x=x*cos(var_lat_lon(8)/180.*3.1415926)                ; geogr. Breite beruecksichtigen

              var_heading=asin(x/y)*180/3.1415926

              if (var_heading lt 0) then degree=360. else degree=180.
              var_heading=degree+var_heading
              if (var_heading gt 90) and (var_heading lt 270) then orient=1 else orient=0
              noheading=0

              print,'Flugrichtung: ',var_heading
          endif

          ;------------ Header eingelesen -> Hauptmenu-Felder aktivieren -----------------

          Widget_Control,BUTTON117,Sensitive=1 ; Save
          Widget_Control,BUTTON118,Sensitive=1 ; Print
;         Widget_Control,BUTTON119,Sensitive=1		; Scan
          Widget_Control,BUTTON135,Sensitive=1 ; Map
;	  Widget_Control,BUTTON92,Sensitive=1		; More Attr.

          Widget_Control,BASE34,Sensitive=1 ; Orbit/Frame
          Widget_Control,BASE40,Sensitive=1 ; Direction
          Widget_Control,BASE38,Sensitive=1 ; Platform
          Widget_Control,BASE71,Sensitive=1 ; Koordinaten
          Widget_Control,BASE84,Sensitive=1 ; Image Size

          ;-------------------- Header-Daten in Hauptmenu-Widgets eintragen -------------------

          WIDGET_CONTROL,FIELD37,SET_VALUE=title
          WIDGET_CONTROL,FIELD139,SET_VALUE=file
          WIDGET_CONTROL,FIELD30,SET_VALUE=var_orbit
          WIDGET_CONTROL,FIELD31,SET_VALUE=var_frame
          WIDGET_CONTROL,FIELD32,SET_VALUE=date
          WIDGET_CONTROL,FIELD33,SET_VALUE=time
          WIDGET_CONTROL,FIELD104,SET_VALUE=var_heading
          WIDGET_CONTROL,FIELD85,SET_VALUE=var_linelength/2
          WIDGET_CONTROL,FIELD86,SET_VALUE=var_numberoflines
          WIDGET_CONTROL,FIELD87,SET_VALUE=var_offset/2
          WIDGET_CONTROL,FIELD72,SET_VALUE=center_lon
          WIDGET_CONTROL,FIELD73,SET_VALUE=center_lat
          
          if (string(var_platform) ne 'unknown') then WIDGET_CONTROL,BGROUP43,SET_VALUE=var_platform-1
          if (noheading eq 0) then WIDGET_CONTROL,BGROUP59,SET_VALUE=orient

          print,'Kalibrierungs-Konstante: ',var_calibrationconst

          ;------------------------------- Bilddaten reduzieren ------------------------------

          if (modus ne 1) then begin ; auch Bilddaten lesen

              if (ecd ne 'QL') then begin              ; nur SAR-PRI
                  Widget_Control,BUTTON126,Sensitive=1 ; Show

                  WIDGET_CONTROL,SLIDER54,GET_VALUE=res ; Reduktionsfaktor

                  if (res eq 8) and (produkt eq 'FDC') then begin         ; FDC hat nur 5000 * 6000 Pixel
                      res=5
                      WIDGET_CONTROL,SLIDER54,SET_VALUE=res
                      print,'FDC erkannt.'
                  endif

                  ll=var_linelength/2
                  nl=var_numberoflines

                  openr,1,fpath+file2 ; Datenfile oeffnen


                  if var_offset gt 0 then begin ; Offset-laenge beachten
                      fheader=bytarr(var_offset)
                      readu,1,fheader ; entsprechend vorfahren
                  endif

                  print,'reduziere Bild:'

                  ll=long(ll) & nl=long(nl)
                  im=intarr((ll-(ll mod res))/res, (nl-(nl mod res))/res)
                  line=intarr(ll,res)
                  prozent=0
                  if (!version.arch eq 'x86') or (file eq '5') $             ; bei diesen Bildern ist Byteswap notwendig (+ Intel-Rechner)
                                              or (file eq '6') $
                                              or (file eq '7') $
                                              or (file eq '193/1') then begin 

                      print,'(-> Byteswapping)'
                      for i=0,nl/res-1 do begin
                          readu,1,line
                          byteorder,line,/lswap
                          im(*,i)=rebin(line(0:(ll-(ll mod res))-1,*), (ll-(ll mod res))/res, 1)
                          if (i mod ((nl/res-1)/10) eq 0) then begin
                              print,strtrim(string(prozent),2)+'% fertig'
                              if (prozent lt 100) then prozent=prozent+10
                          endif
                      endfor
                  endif else begin
                      for i=0,nl/res-1 do begin
                          readu,1,line
                          im(*,i)=rebin(line(0:(ll-(ll mod res))-1,*), (ll-(ll mod res))/res, 1)
                          if (i mod ((nl/res-1)/100) eq 0) then begin
                              print,strtrim(string(prozent),2)+'% fertig'+string(13b),format='(A,$)'
                              if (prozent lt 100) then prozent=prozent+1
                          endif
                      endfor
                  endelse
                  close,1
                  
                  if (produkt eq 'FDC') then begin           ; wenn FDC, dann Bild quadratisieren...
                      im=congrid(im,ll/res,ll/res)
                      nl=ll
                  endif


                  if (produkt eq 'GEC') then begin           ; wenn GEC, dann Bild spiegeln
                      im=rotate(im,5)
                  endif


                  if (res eq 5) and (file eq '1') then begin ; oberer Bildteil ist verschoben
                      tmp=intarr(1001,166)
                  
                      tmp(0:727,*)=im(273:1000,835:1000)
                      tmp(728:1000,*)=im(0:272,835:1000)
                      im(*,834:998)=tmp(*,1:165)
                      tmp=0
                  endif

                  print,'Bild ist reduziert auf '+strtrim(string((ll-(ll mod res))/res),2)+' mal ' $
						       +strtrim(string((nl-(nl mod res))/res),2)+'.'
              endif else begin
                  print,'lese Bild ein'
                  read_srf,fpath+file2,im
                  im=rotate(im,7)
                  if (orient eq 1) then im=rotate(im,2)
                  Widget_Control,BUTTON126,Sensitive=1 ; Show

                  s=size(im)                                   ; Infos �bers Bild einlesen
                  ll=s(1)                                      ; Laenge einer Zeile
                  nl=s(2)                                      ; Anzahl Zeilen
                  var_linelength=fix(ll)                       ; Long -> Int
                  var_numberoflines=fix(nl)
                  WIDGET_CONTROL,FIELD85,SET_VALUE=var_linelength
                  WIDGET_CONTROL,FIELD86,SET_VALUE=var_numberoflines

              endelse
          endif

          if (!version.os_family eq 'unix') and ((ecd eq 'CD') or (ecd eq 'QL')) then begin ; wenn von CD gelesen,
              cd,lw             ; dann Verzeichnis wechseln
          endif

          WIDGET_CONTROL, event.top, /DESTROY
          Widget_Control,MAIN13,Sensitive=1                      ; Hauptmenu Sar-Tools wieder aktivieren
          RETURN
      endif                     ; fpath und filename leer
  end

  'BUTTON27': begin		  				  ; Cancel load-menue
    		WIDGET_CONTROL, event.top, /DESTROY
                Widget_Control,MAIN13,Sensitive=1       ; Hauptmenu Sar-Tools wieder aktivieren
                RETURN
	      end
  'BUTTON71': begin                                	;'Event for Copy File'

	WIDGET_CONTROL,BASE33,Sensitive=1		; Destination-Path
	WIDGET_CONTROL,BASE60,Sensitive=1		; Buttons Copy/Cancel
	WIDGET_CONTROL,BASE2 ,Sensitive=0		;
	spawn,'pwd',currentdir
	lw=string(currentdir(0))
	WIDGET_CONTROL,FIELD34,SET_VALUE=lw             ; aktuellen Pfad eintragen

      end

  'BUTTON69': begin
	;      Print, 'Event for Copy'

        print,'Starte Einlesen vom Tape...'
	WIDGET_CONTROL,FIELD35,GET_VALUE=f		; Filenamen einlesen
	file=f(0)
	if (file ne '')	then begin			; wenn Filename nicht leer:
		WIDGET_CONTROL, /HOURGLASS
		WIDGET_CONTROL,FIELD34,GET_VALUE=l	; (geaenderten) Pfad einlesen
		lw=l(0)
                if (strmid(lw,strlen(lw)-1,1) eq '/') then lw=strmid(lw,0,strlen(lw)-1)
		print,lw
                cd,lw           ; und dorthin wechseln.

                spawn,'dd of='+strtrim(string(file),2)+'.h1 if=/dev/nrst4 bs=32k'
                spawn,'dd of='+strtrim(string(file),2)+'.h2 if=/dev/nrst4 bs=32k
                spawn,'dd of='+strtrim(string(file),2)+'.sar if=/dev/nrst4 bs=32k
                spawn,'dd of='+strtrim(string(file),2)+'.tlr if=/dev/nrst4 bs=32k
                  
                spawn,'mt -f /dev/nrst4 status'
                spawn,'mt -f /dev/nrst4 offline'

                file0=file+'.h1'                        ; header-files etc. benennen
		file1=file+'.h2'
		file2=file+'.sar'
		file3=file+'.ps'
		ecd='E'
	endif

	WIDGET_CONTROL,FIELD24,SET_VALUE=file		; Filenamen und
	WIDGET_CONTROL,FIELD18,SET_VALUE=lw+'/'		; Pfad an die Felder im oberen Teil uebergeben.

	WIDGET_CONTROL,BASE2 ,Sensitive=1		;
	WIDGET_CONTROL,BASE33,Sensitive=0		; Destination-Path
    WIDGET_CONTROL,BASE60,Sensitive=0		; Buttons Copy/Cancel

      end
  'BUTTON68': begin
    Print, 'Event for Cancel'
	WIDGET_CONTROL,BASE2 ,Sensitive=1		;
	WIDGET_CONTROL,BASE33,Sensitive=0		; Destination-Path
        WIDGET_CONTROL,BASE60,Sensitive=0               ; Buttons Copy/Cancel
      end
  endcase
end














pro loadmenukl, GROUP=Group
common load_widgets
common var_env
common var_names
common variable
common titel
;common load_path
common enter_widgets
common main_widgets
common var_header
common var_data
common database
common abbruch_ende, abbruchvar
common loading

print, datenort


;------------------------------------------------------(
;common enter_widgets, E_FIELD3, E_BUTTON8
;common load_widgets, FIELD18, FIELD24, FIELD34, FIELD35, SLIDER54, BGROUP21, BASE33, BASE60, BASE2
;common var_names
;--------------------------------------------------------)

if (win_map ne 0) then wdelete,0
if (win_show ne 0) then wdelete,1



IF N_ELEMENTS(Group) EQ 0 THEN GROUP=0

  junk   = { CW_PDMENU_S, flags:0, name:'' }


  MAIN1 = WIDGET_BASE(GROUP_LEADER=Group, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Load-Menu', $
      UVALUE='MAIN1')

  BASE2 = WIDGET_BASE(MAIN1, $
      COLUMN=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='Load', $
      UVALUE='BASE2')

  LABEL50 = WIDGET_LABEL( BASE2, $
      FONT='6x13bold', $
      UVALUE='LABEL50', $
      VALUE='Load File')

  BASE10 = WIDGET_BASE(BASE2, $
      ROW=1, $
      SPACE=40, $
      MAP=1, $
      TITLE='CD/Tape/Header', $
      UVALUE='BASE10')

  Btns985 = [ $
    'Harddisk', $
    'CD' ]
  BGROUP21 = CW_BGROUP( BASE10, Btns985, $
      ROW=1, $
      EXCLUSIVE=1, $
      LABEL_LEFT='Medium:', $
      SET_VALUE=0, $                ; Startwert = Harddisk
      UVALUE='BGROUP21')

  SLIDER54 = WIDGET_SLIDER( BASE10, $			; fuer Reduktion von N x N auf 1 Pixel
      MAXIMUM=20, $
      MINIMUM=1, $
      TITLE='Reduktion from N x N to 1 Pixel', $
      UVALUE='SLIDER54', $
    ;------------------------------------------------hier die bildgroesse einstellbar------------------------------------------------------------------------------
      ;VALUE=8)    ;fuer postscripts und grosse Quicklooks
      VALUE=20)   ;fuer kleine quicklooks


  BASE17 = WIDGET_BASE(BASE2, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Path', $
      UVALUE='BASE17')


  BASE18 = WIDGET_BASE(BASE17, $
      ROW=1, $
      MAP=1, $
      TITLE='Buttons Modus', $
      UVALUE='BASE18')

  BASE19 = WIDGET_BASE(BASE17, $
      ROW=1, $
      MAP=1, $
      TITLE='Path', $
      UVALUE='BASE19')

  BASE20 = WIDGET_BASE(BASE17, $
      ROW=1, $
      MAP=1, $
      TITLE='Filename + Requester', $
      UVALUE='BASE20')

  Btns989 = [ $
    'Complete', $
    'Header only', $
    'SubImage' ]
  BGROUP58 = CW_BGROUP( BASE18, Btns989, $
      ROW=1, $
      EXCLUSIVE=1, $
      LABEL_LEFT='Modus:', $
      SET_VALUE=modus, $				; Startwert: modus=0 -> Complete / modus=2 -> SubImage
      UVALUE='BGROUP58')

  FIELD18 = CW_FIELD( BASE19,VALUE='', $
      ROW=1, $
      STRING=1, $
      TITLE='Path:', $
      UVALUE='FIELD18', $
      XSIZE=58, $
      YSIZE=1)

  FIELD24 = CW_FIELD( BASE20, $
      ROW=1, $
      STRING=1, $
      TITLE='Filename:', $
      UVALUE='FIELD24', $
      XSIZE=10, $
      YSIZE=1)

  BUTTON51 = WIDGET_BUTTON( BASE20, $
      UVALUE='BUTTON51', $
      VALUE='Filerequest')


  BASE25 = WIDGET_BASE(BASE2, $
      ROW=1, $
      MAP=1, $
      TITLE='Load / Cancel', $
      UVALUE='BASE25')

  BUTTON44 = WIDGET_BUTTON( BASE25, $
      FONT='6x13bold', $
      UVALUE='BUTTON44', $
      VALUE='Load', $
      XSIZE=60)

  BUTTON27 = WIDGET_BUTTON( BASE25, $
      UVALUE='BUTTON27', $
      VALUE='Cancel', $
      XSIZE=60)

if (!version.arch ne 'x86') then begin

  BASE9 = WIDGET_BASE(MAIN1, $
      COLUMN=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='tape', $
      UVALUE='BASE9')

  BASE31 = WIDGET_BASE(BASE9, $
      ROW=1, $
      MAP=1, $
      TITLE='Copy tape -> Harddisk', $
      UVALUE='BASE31')

  BUTTON71 = WIDGET_BUTTON( BASE31, $
      FONT='6x13bold', $
      UVALUE='BUTTON71', $
      VALUE='Copy File:', $
      XSIZE=80)

  LABEL74 = WIDGET_LABEL( BASE31, $
      FONT='6x13bold', $
      UVALUE='LABEL74', $
      VALUE='  Tape -> Harddisk')


  BASE33 = WIDGET_BASE(BASE9, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Dest-Path', $
      UVALUE='BASE33')

  FIELD34 = CW_FIELD( BASE33,VALUE='', $
      ROW=1, $
      STRING=1, $
      TITLE='Destination-Path:', $
      UVALUE='FIELD34', $
      XSIZE=46)

  FIELD35 = CW_FIELD( BASE33,VALUE='', $
      ROW=1, $
      STRING=1, $
      TITLE='Filename:', $
      UVALUE='FIELD35', $
      XSIZE=10, $
      YSIZE=1)

  BASE60 = WIDGET_BASE(BASE9, $
      ROW=1, $
      MAP=1, $
      TITLE='Copy/Cancel', $
      UVALUE='BASE60')

  BUTTON69 = WIDGET_BUTTON( BASE60, $
      FONT='6x13bold', $
      UVALUE='BUTTON69', $
      VALUE='Copy', $
      XSIZE=60)

  BUTTON68 = WIDGET_BUTTON( BASE60, $
      UVALUE='BUTTON68', $
      VALUE='Cancel', $
      XSIZE=60)


  WIDGET_CONTROL,BASE33,Sensitive=0		; Destination-Path
  WIDGET_CONTROL,BASE60,Sensitive=0		; Buttons Copy/Cancel
endif

  WIDGET_CONTROL, MAIN1, /REALIZE
  
  
  
  
  
  

  
  
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
  
;------------------------------------------ab hier selbst geschrieben zum automatiesieren--------------------------------  





  begin
  print, 'fange an'

WIDGET_CONTROL,BGROUP21,GET_VALUE=cd
  if (!version.os_family eq 'unix') then begin        ; auf der Sun
            if (cd eq 1) then begin
                WIDGET_CONTROL, /HOURGLASS
                spawn,'pwd',currentdir
                lw=string(currentdir(0))                ; merken, wo ich bin
                WIDGET_CONTROL,BUTTON177,GET_VALUE=mount
                if (mount eq 'Mount') then begin
                    print,'mounte CD-ROM...'
                    spawn,'npmount hsfs'                 ; CD-ROM mounten
                    Widget_Control,BUTTON177,Sensitive=1 ; Mount
                    WIDGET_CONTROL,BUTTON177,SET_VALUE='Unmount'
                endif
                cd,'/cdrom/'
    spawn,'ls',cddir        ; CD-Name einlesen 
    cd,cddir(1)   
                print,'lese Directory...'
                complete_file=DIALOG_PICKFILE(/READ, FILTER = '*')
                print,'Fertig.'
            endif else begin
                cd, datenort              ;Quellverzeichnis
                ;complete_file=DIALOG_PICKFILE(/READ, FILTER = '*.sar')
                print, namevar(laufvar)
                ;print, sarname
                complete_file=datenort+namevar(laufvar)             ;hier wird der komplette pfad mit name angegeben
                ;print, complete_file                    ;um bei fehlern den pfad zu sehen
                ;print, datenort                         ;ort der eingelesenen daten
            endelse
        endif else begin                                 ; wenns unter Windows95 l�uft
            if (cd eq 1) then begin
                 complete_file=DIALOG_PICKFILE(/READ, FILTER = 'Dat*')
            endif else begin
                complete_file=DIALOG_PICKFILE(/READ, FILTER = '*.sar')
            endelse
        endelse

  if (!version.os_family eq 'unix') then begin        ; auf der Sun
            slash=strpos(complete_file,'/')
            fname=strmid(complete_file,slash+1,strlen(complete_file)) ; Filename
            fpath=strmid(complete_file,0,slash+1) ; Pfadname
            lw_path=fpath       ; Pfad merken fuer naechsten PICKFILE
            
            if (complete_file eq '/cdrom/PRI_16598_0405') then begin
                fpath='/cdrom/SCENE1/'
            endif
            WIDGET_CONTROL,FIELD18,SET_VALUE=fpath
            
            if (cd eq 0) then begin ; Festplatte
                file=strmid(fname,0,strpos(fname,'.'))
                WIDGET_CONTROL,FIELD24,SET_VALUE=file
            endif else begin    ; CD
                if (complete_file eq '/cdrom/PRI_16598_0405') then begin
                    cdnr='193'
                    file='1'
                    file=cdnr+'/'+file
                    WIDGET_CONTROL,FIELD24,SET_VALUE=file
                endif else begin
                    if (strupcase(strmid(fpath,7,5)) eq 'SCENE') then begin ; SAR-PRI
                        file=strmid(complete_file,slash-1,1) ; wievieltes Bild auf dieser CD
                        enter   ; Prozedur zum Einlesen der Cd-Nr.
                    endif else begin ; SAR-QL
                        file=strmid(fname,1,4) ; wievieltes Bild auf dieser CD
                        cdnr=strmid(fname,5,3) ; CD-Nummer
                        file=cdnr+'/'+file
                        WIDGET_CONTROL,FIELD24,SET_VALUE=file
                    endelse
                endelse
            endelse
        endif else begin        ; Windows 9x
            
            backslash=rstrpos(complete_file,'\')
            fname=strmid(complete_file,backslash+1,strlen(complete_file)) ; Filename
            fpath=strmid(complete_file,0,backslash+1) ; Pfadname
            lw_path=fpath       ; Pfad merken fuer naechsten PICKFILE
            
            WIDGET_CONTROL,FIELD18,SET_VALUE=fpath
            
            if (cd eq 0) then begin ; Festplatte
                file=strmid(fname,0,strpos(fname,'.'))
                WIDGET_CONTROL,FIELD24,SET_VALUE=file
            endif else begin    ; CD
                if (strupcase(strmid(fpath,3,5)) eq 'SCENE') then begin ; SAR-PRI
                    file=strmid(complete_file,backslash-1,1) ; wievieltes Bild auf dieser CD
                    enter       ; Prozedur zum Einlesen der Cd-Nr.
                endif else begin ; SAR-QL
                    file=strmid(fname,1,4) ; wievieltes Bild auf dieser CD
                    cdnr=strmid(fname,5,3) ; CD-Nummer
                    file=cdnr+'/'+file
                    WIDGET_CONTROL,FIELD24,SET_VALUE=file
                endelse
            endelse
        endelse
    end
print,'habe die datei rausgesucht sie wird jetzt eingelesen.'


begin             ; starte Einlesen
  print, 'load'
      WIDGET_CONTROL, /HOURGLASS
      WIDGET_CONTROL,BGROUP21,GET_VALUE=cd

      produkt='PRI'
      WIDGET_CONTROL,FIELD18,GET_VALUE=fpath
      WIDGET_CONTROL,FIELD24,GET_VALUE=filename
      fpath=fpath(0)
      filename=filename(0)

      if (fpath ne '') and (filename ne '') then begin
          if (cd eq 1) then begin
              slash=strpos(filename,'/')
              cdnr=strmid(filename,0,slash)
              file=strmid(filename,slash+1,strlen(filename)-slash)
              
              WIDGET_CONTROL,BUTTON177,GET_VALUE=mount
              if (!version.os_family eq 'unix') and (mount eq 'Mount') then begin
                  spawn,'pwd',currentdir
                  lw=string(currentdir(0)) ; merken, wo ich bin
                  print,'mounte CD-ROM...'
                  spawn,'npmount hsfs' ; CD-ROM mounten
                  Widget_Control,BUTTON177,Sensitive=1 ; Mount
                  WIDGET_CONTROL,BUTTON177,SET_VALUE='Unmount'
              end

              if ((!version.os_family eq 'unix')    and (strupcase(strmid(fpath,7,5)) eq 'SCENE')) or $
                 ((!version.os_family eq 'Windows') and (strupcase(strmid(fpath,3,5)) eq 'SCENE')) then begin              ; nur SAR-PRI
                  
                  if (filename eq '193/1') then fpath='/cdrom/'

                  cd,fpath

                  files=findfile('*',COUNT=count) ; Verzeichnis einlesen

                  if (!version.os_family eq 'Windows') and (count eq 6) then begin
                      tmp=files
                      files=strarr(4)
                      files=tmp(2:5)
                      count=4
                  endif
                  if (count eq 4) then begin ; wenn genau 4 Eintraege...

                      fa=''
                      for i=0,3 do begin
                          fa=fa+strmid(files(i),0,1) ; Anfangsbuchstaben der Files einlesen
                      endfor
                      if (strupcase(fa) eq 'DLNV') then begin ; wenn Standard-'DLNV', dann
                          file0=files(3) ; eingelesene Files zuordnen
                          file1=files(1)
                          file2=files(0)
                      endif else begin ; ansonsten: Fehler!
                          file0=''
                          file1=''
                          file2=''
                          print,'Konnte Files nicht identifizieren!'
                      endelse
                  endif else if (filename eq '193/1') then file2=files(0)   ; fuer CD 193 (nur 1 Datenfile, keine Headerfiles)
                      
                  file3=cdnr+'_'+file+'.ps'
                  ecd='CD'
                

              endif else begin  ; SAR-QL
                  file0='S'+file+cdnr+'.txt'
                  file2='S'+file+cdnr+'.ql'
                  file3='S'+file+cdnr+'.ps'
                  ecd='QL'
                  produkt='QL'
              endelse

          endif else begin      ; von Harddisk einlesen
              if (strupcase(strmid(fpath,strlen(fpath)-7,5)) eq 'SCENE') then begin ;  CD auf Platte kopiert
                  print,fpath
                  
                  stop
                

              endif else begin  ; Exabyte auf Platte kopiert
                  file0=file+'.h1'
                  file1=file+'.h2'
                  file2=file+'.sar'
                  file3=file+'.ps'
                  ecd='E'
              endelse
          endelse

          uline=strpos(filename,'_')
          if (uline ne -1) then strput,filename,'/',uline

          file=filename
;          exabyte=ecd+file
          exabyte=file


          if (ecd ne 'QL') then begin            ; QL nicht in Datenbank

              ; -----Variable 'exabyte' so formatieren, dass sie sich als Suchindex eignet--------------
              tape_form=exabyte
              if (strmid(tape_form,0,1) eq 'E') then begin
                  tape_form=' '+tape_form
                  tl=strlen(tape_form)+1
                  if (uline ne -1) then tl=tl-(strlen(tape_form)-uline)+2
              endif else begin
                  tl=strlen(tape_form)-1
              endelse

              for sp=0,7-tl do tape_form=strmid(tape_form,0,2)+' '+(strmid(tape_form,2,strlen(tape_form)-2))
              
              ;print, 'tapeform1:'+tape_form
              ersnr=strmid(tape_form,66,1)
              ;print,'ersnr'+ersnr
              tape_form=strmid(tape_form,63,23)
              tape_form='users/ifmsun27b/ifmrs/u241068/pool/satellites/ERS-'+ersnr+'/'+tape_form
              print, tape_form

; Zu dem muessten noch neue eintraege untersucht werden (Schleife bauen!)
; --------------------------------- Pruefen, ob schon in Datenbank erfasst --------------------------------
;print,dbpath
              restore,dbpath    ; initialisiert in sar.pro

              s=size(datenbank) ; Infos �ber Datenbank einlesen
              db_laenge=s(1)     ; L�nge eines Datensatzes
              db_zaehler=s(2)   ; Anzahl Datens�tze
              db_zaehler=fix(db_zaehler) ; Long -> Int
              print,'Datenbank eingelesen ('+str(db_zaehler)+' Saetze).'

;------------------------------------------------------------------------------
                ;print,'tape_form ist :'+tape_form   
                ;print,'datenbankeintrag:'+datenbank(0,1120)
                tape_form=strmid(tape_form,57,17)                             ;Die tape_form wird von der Adresse users/ifmsun27b/ifmrs/u241068/pool/satellites/ERS-1/ERS1/ ausgelesen
               ;dbname=strmid(datenbank(0,1120),68,17)
                ;print,'tape_form ist :'+tape_form  
                ;print,'datenbankeintrag:'+dbname




              new=1             ; ist dieser Datensatz schon erfasst?
;              if (!version.os_family eq 'unix') then begin
                  i=1251        ;ab hier fangen die Dateneintraege fuer die Datenbank an
                  dbname=strmid(datenbank(0,i),68,17)         ;kuerzen des Datenbank eintrage auf Sar_123456_12345 zum Vergleich
                  while (i lt db_zaehler-1 and tape_form ne dbname) do begin
                   dbnamepfad=strmid(datenbank(0,i),6,11)      ; um zu ueberpruefen welcher pfad in der Datenbank gespeichert ist
                   if (dbnamepfad eq 'ifmlinux20c') then begin
                    dbname=strmid(datenbank(0,i),68,17) 
                   endif 
                   if (dbnamepfad eq 'ifmsun27b/i') then begin 
                    dbname=strmid(datenbank(0,i),57,17)  
                   endif                
                   i=i+1
                      if (i ne db_zaehler-1) then new=0 & index=i
                      if (i eq db_zaehler-1) then $
                      if (tape_form ne datenbank(0,i)) then new=1 else new=0 & index=i
                  endwhile     
;             endif
;---------------------------------------------- Pruefung ob daten schon vorhanden -----------------------------------------
                abbruchvar=0                              
              if (new eq 0) then begin ; Daten schon vorhanden?
               abbruchvar=1
                if (abbruchvar eq 1) then begin          ;wenn daten schon vorhanden dann hier abbrechen und zu sar.pro zurueck
                 WIDGET_CONTROL,MAIN1, /DESTROY
                 Widget_Control,MAIN13,Sensitive=1                      ; Hauptmenu Sar-Tools wieder aktivieren
                 RETURN
                 XMANAGER, 'MAIN1', MAIN1
                endif
              endif else begin
              print, 'Daten noch nicht vorhanden'
              print, new+","+index
              endelse           
               
              if (new eq 0) then begin ; Daten schon vorhanden
                  print,'Lese Header aus Datenbank.'
                  tape_form         = datenbank( 0,index)
                  title             = datenbank( 1,index)
                  var_orbit         = datenbank( 2,index)
                  var_frame         = datenbank( 3,index)
                  date              = datenbank( 4,index)
                  time              = datenbank( 5,index)
                  var_platform      = datenbank( 6,index)
                  var_heading       = datenbank( 7,index)
                  var_linelength    = datenbank( 8,index)
                  var_numberoflines = datenbank( 9,index)
                  var_offset        = datenbank(10,index)
                  center_lon        = datenbank(11,index)
                  center_lat        = datenbank(12,index)
                  orient            = datenbank(13,index)
                  region            = datenbank(14,index)
                  var_lat_lon(0:7)  = datenbank(15:22,index)
                  version           = datenbank(23,index)
                  PS_CD             = datenbank(24,index)
   ;               produkt           = datenbank(25,index)

                  if (strpos(center_lat,'N') eq -1) then begin
                      var_lat_lon(8)='-'+strtrim(strmid(center_lat,0,strlen(center_lat)-2),2)
                  endif else begin
                      var_lat_lon(8)=strmid(center_lat,0,strlen(center_lat)-2)
                  endelse
                  if (strpos(center_lon,'E') eq -1) then begin
                      var_lat_lon(9)='-'+strtrim(strmid(center_lon,0,strlen(center_lon)-2),2)
                  endif else begin
                      var_lat_lon(9)=strmid(center_lon,0,strlen(center_lon)-2)
                  endelse

                  noheading=0   ;in der Datenbank gibt es IMMER eine Flugrichtung.
              endif else begin  ; Daten aus Header einlesen
                  print,'Lese Header aus Header-Files ein.'

                  sarhead,file2,file0,file1,INPATH=fpath               
               
           ; fpath und filename leer
            
                 
                                ;--------------------------- Ascending oder Decending ? ----------------------------

                  if (string(var_heading) ne 'unknown') then begin
                      noheading=0
                      if (var_heading gt 90) and (var_heading lt 270) then orient=1 else orient=0
                  endif else begin
                      noheading=1
                  endelse
                                ;---------------------------- Date und Time extrahieren ----------------------------

                       if (var_aqu_dat ne 'unknown') then begin
                      date=strmid(var_aqu_dat,6,2)+'.'+strmid(var_aqu_dat,4,2)+'.'+strmid(var_aqu_dat,0,4)
                  endif else begin
                      date='DD.MM.YYYY'
                  endelse

                  if (var_aqu_time ne 'unknown') then begin
                      time=strmid(var_aqu_time,0,2)+':'+strmid(var_aqu_time,2,2)
                  endif else begin
                      time='HH:MM'
                  endelse
                            
                                ;-------------------------- Center Coordinates: +/- -> NSEW  -----------------------

                  if (var_lat_lon(8)lt 0) then ns=' S' else ns=' N'
                  center_lat=string(abs(var_lat_lon(8)))+ns
                  if (var_lat_lon(9) lt 0) then ew=' W' else ew=' E'
                  center_lon=string(abs(var_lat_lon(9)))+ew

                  title='somewhere' ; Titel unbekannt


              endelse
          endif else begin                    ; Quicklook Header einlesen
              header1=' '
              openr,1,fpath+file0
              readf,1,header1
              readf,1,format='(a7,a8,a7,a8,a7,a8,a7,a8,a7,a8)',v0,v1,v2,v3,v4,v5,v6,v7,v8,v9
              close,1

              header1=strtrim(header1,2)
              if (strmid(header1,0,2) eq 'E2') then begin
                  var_platform=2
                  header1=strmid(header1,2,strlen(header1)-2)
              endif else begin
                  var_platform=1
              endelse
;print,header1
              date=strmid(header1,0,11)
              time=strmid(header1,12,8)
              header1=strmid(header1,50,strlen(header1)-50)
              var_frame=strmid(header1,strlen(header1)-10,4)
              var_orbit=strmid(header1,strlen(header1)-5,5)
              title=strmid(header1,0,strlen(header1)-13)
              region='Quicklook'

              var_lat_lon=strarr(10)
              var_lat_lon=[v2,v3,v4,v5,v6,v7,v8,v9,v0,v1]

              if (var_lat_lon(8)lt 0) then ns=' S' else ns=' N'
              center_lat=string(abs(var_lat_lon(8)))+ns
              if (var_lat_lon(9) lt 0) then ew=' W' else ew=' E'
              center_lon=string(abs(var_lat_lon(9)))+ew
              new=0             ; nicht in Datenbank aufnehmen
         
          endelse

          if (var_heading eq 0) and (var_lat_lon(1) ne 'unknown') then begin         ; ermitteln der Flugrichtung 

              xa=var_lat_lon(1) & xb=var_lat_lon(5)
              ya=var_lat_lon(0) & yb=var_lat_lon(4)

              x=xa-xb & y=ya-yb

              x=x*cos(var_lat_lon(8)/180.*3.1415926)                ; geogr. Breite beruecksichtigen

              var_heading=asin(x/y)*180/3.1415926

              if (var_heading lt 0) then degree=360. else degree=180.
              var_heading=degree+var_heading
              if (var_heading gt 90) and (var_heading lt 270) then orient=1 else orient=0
              noheading=0

              print,'Flugrichtung: ',var_heading
          endif

          ;------------ Header eingelesen -> Hauptmenu-Felder aktivieren -----------------

          Widget_Control,BUTTON117,Sensitive=1 ; Save
          Widget_Control,BUTTON118,Sensitive=1 ; Print
;         Widget_Control,BUTTON119,Sensitive=1    ; Scan
          Widget_Control,BUTTON135,Sensitive=1 ; Map
;   Widget_Control,BUTTON92,Sensitive=1   ; More Attr.

          Widget_Control,BASE34,Sensitive=1 ; Orbit/Frame
          Widget_Control,BASE40,Sensitive=1 ; Direction
          Widget_Control,BASE38,Sensitive=1 ; Platform
          Widget_Control,BASE71,Sensitive=1 ; Koordinaten
          Widget_Control,BASE84,Sensitive=1 ; Image Size

          ;-------------------- Header-Daten in Hauptmenu-Widgets eintragen -------------------

          WIDGET_CONTROL,FIELD37,SET_VALUE=title
          WIDGET_CONTROL,FIELD139,SET_VALUE=file
          WIDGET_CONTROL,FIELD30,SET_VALUE=var_orbit
          WIDGET_CONTROL,FIELD31,SET_VALUE=var_frame
          WIDGET_CONTROL,FIELD32,SET_VALUE=date
          WIDGET_CONTROL,FIELD33,SET_VALUE=time
          WIDGET_CONTROL,FIELD104,SET_VALUE=var_heading
          WIDGET_CONTROL,FIELD85,SET_VALUE=var_linelength/2
          WIDGET_CONTROL,FIELD86,SET_VALUE=var_numberoflines
          WIDGET_CONTROL,FIELD87,SET_VALUE=var_offset/2
          WIDGET_CONTROL,FIELD72,SET_VALUE=center_lon
          WIDGET_CONTROL,FIELD73,SET_VALUE=center_lat
          
          if (string(var_platform) ne 'unknown') then WIDGET_CONTROL,BGROUP43,SET_VALUE=var_platform-1
          if (noheading eq 0) then WIDGET_CONTROL,BGROUP59,SET_VALUE=orient

          print,'Kalibrierungs-Konstante: ',var_calibrationconst

          ;------------------------------- Bilddaten reduzieren ------------------------------

          if (modus ne 1) then begin ; auch Bilddaten lesen

              if (ecd ne 'QL') then begin              ; nur SAR-PRI
                  Widget_Control,BUTTON126,Sensitive=1 ; Show

                  WIDGET_CONTROL,SLIDER54,GET_VALUE=res ; Reduktionsfaktor

                  if (res eq 8) and (produkt eq 'FDC') then begin         ; FDC hat nur 5000 * 6000 Pixel
                      res=5
                      WIDGET_CONTROL,SLIDER54,SET_VALUE=res
                      print,'FDC erkannt.'
                  endif

                  ll=var_linelength/2
                  nl=var_numberoflines

                  openr,1,fpath+file2 ; Datenfile oeffnen
;print, fpath+file2

                 if var_offset gt 0 then begin ; Offset-laenge beachten
                      fheader=bytarr(var_offset)
                      print, fheader
                      readu,1,fheader ; entsprechend vorfahren
                  endif

                  print,'reduziere Bild:'

                  ll=long(ll) & nl=long(nl)
                  im=intarr((ll-(ll mod res))/res, (nl-(nl mod res))/res)
                  line=intarr(ll,res)
                  prozent=0
                  if (!version.arch eq 'x86') or (file eq '5') $             ; bei diesen Bildern ist Byteswap notwendig (+ Intel-Rechner)
                                              or (file eq '6') $
                                              or (file eq '7') $
                                              or (file eq '193/1') then begin 

                      print,'(-> Byteswapping)'
                      for i=0,nl/res-1 do begin
                          readu,1,line
                          byteorder,line,/lswap
                          im(*,i)=rebin(line(0:(ll-(ll mod res))-1,*), (ll-(ll mod res))/res, 1)
                          if (i mod ((nl/res-1)/10) eq 0) then begin
                              print,strtrim(string(prozent),2)+'% fertig'
                              if (prozent lt 100) then prozent=prozent+10
                          endif
                      endfor
                  endif else begin
                      for i=0,nl/res-1 do begin
                          readu,1,line
                          im(*,i)=rebin(line(0:(ll-(ll mod res))-1,*), (ll-(ll mod res))/res, 1)
                          if (i mod ((nl/res-1)/100) eq 0) then begin
                              print,strtrim(string(prozent),2)+'% fertig'+string(13b),format='(A,$)'
                              if (prozent lt 100) then prozent=prozent+1
                          endif
                      endfor
                  endelse
                  close,1
                  
                  if (produkt eq 'FDC') then begin           ; wenn FDC, dann Bild quadratisieren...
                      im=congrid(im,ll/res,ll/res)
                      nl=ll
                  endif


                  if (produkt eq 'GEC') then begin           ; wenn GEC, dann Bild spiegeln
                      im=rotate(im,5)
                  endif


                  if (res eq 5) and (file eq '1') then begin ; oberer Bildteil ist verschoben
                      tmp=intarr(1001,166)
                  
                      tmp(0:727,*)=im(273:1000,835:1000)
                      tmp(728:1000,*)=im(0:272,835:1000)
                      im(*,834:998)=tmp(*,1:165)
                      tmp=0
                  endif

                  print,'Bild ist reduziert auf '+strtrim(string((ll-(ll mod res))/res),2)+' mal ' $
                   +strtrim(string((nl-(nl mod res))/res),2)+'.'
              endif else begin
                  print,'lese Bild ein'
                  read_srf,fpath+file2,im
                  im=rotate(im,7)
                  if (orient eq 1) then im=rotate(im,2)
                  Widget_Control,BUTTON126,Sensitive=1 ; Show

                  s=size(im)                                   ; Infos �bers Bild einlesen
                  ll=s(1)                                      ; Laenge einer Zeile
                  nl=s(2)                                      ; Anzahl Zeilen
                  var_linelength=fix(ll)                       ; Long -> Int
                  var_numberoflines=fix(nl)
                  WIDGET_CONTROL,FIELD85,SET_VALUE=var_linelength
                  WIDGET_CONTROL,FIELD86,SET_VALUE=var_numberoflines

              endelse
          endif

          if (!version.os_family eq 'unix') and ((ecd eq 'CD') or (ecd eq 'QL')) then begin ; wenn von CD gelesen,
              cd,lw             ; dann Verzeichnis wechseln
          endif
begin
          WIDGET_CONTROL,MAIN1, /DESTROY
          Widget_Control,MAIN13,Sensitive=1                      ; Hauptmenu Sar-Tools wieder aktivieren
          RETURN
      end           
  end

  XMANAGER, 'MAIN1', MAIN1
           ; fpath und filename leer
  end
  
end
