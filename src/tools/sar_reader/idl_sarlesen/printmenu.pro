; NAME:
;	printmenu.pro
;
;
; CALLING PROCEDURE:
;       sar.pro (ohne Parameter)
;
;
; PURPOSE:
;       Diese Prozedur erstellt ein Postscript-File (A3 oder A4,
;       einstellbare Farbtiefe (1 bis 8 Bits per Pixel), nur Bild oder mit allen Infos).
;
;	Das fertige PS-File kann mit Ghostview angeschaut werden ('PREVIEW')
;       und auf einen der 3 Drucker PSCI2 (A3/A4), PSI3 (A4) oder PSI9 (A4)
;       ausgedruckt werden.
;
;
; COMMON BLOCKS:
;	BLOCK1:	Describe any common blocks here. If there are no COMMON
;		blocks, just delete this entry.
;
;
; MODIFICATION HISTORY:
; 	Written by:	Gerald Fiedler
;	Juli 1997
;	23.09.1997:	v1.0	Bugfix (ausdrucken im CD-Mode war nicht moeglich)
;       20.01.1998:     v1.01   Headerdaten in Datenbank eintragen
;                               (Das wird erst hier gemacht, da fehlende/fehlerhafte Daten jetzt alle OK sind.)
;       18.05.1999:     v1.21   Speichern der PS-Files in ~uhso/ERS/Bilder/
;                               Neues Feld in Datenbank (PS_CD, Nr. der CD, auf der das PS-File gebrannt wird)
;	02.12.1999:     v1.32   Fehler im Windows-Mode beseitigt
;
;       05.06.2008: printmenu automatisiert 




pro MAIN3_Event, Event

common main_widgets
common var_header
common var_env
common var_names
common var_data
common print_widgets, BASE20, SLIDER9, SLIDER12, BGROUP3, BGROUP17, BGROUP21, BUTTON29, BUTTON31
common var_print, paperfile

;if (!version.os_family eq 'unix') then gif_path='/users/ifmsun12f/uhso/idl_lib/Sar-Tool/uhsologo.gif'  $
; 			          else gif_path='c:\RSI\Sar-Tool\uhsologo.gif' ; Logo fuer UHSO das logo wird nicht gebraucht
lat_lon_edge=strarr(8)


;--------------------------------------------------------------------------------------------------------

WIDGET_CONTROL,Event.Id,GET_UVALUE=Ev

case Ev of

  'BGROUP3': begin
	end

  'BGROUP17': begin
	end

  'BGROUP21': begin
	end

  'SLIDER12': begin
	end

  'SLIDER9': begin
	end

  'BUTTON28': begin						; Postscript-File erzeugen

  	WIDGET_CONTROL, /HOURGLASS
	set_plot,'ps'

		; ----------------------- actual papersize ------------------------------

		WIDGET_CONTROL,BGROUP3,GET_VALUE=papersize
		papersize=papersize+3
    
		if (papersize eq 4) then begin
			a3x=21.
			a3y=29.7
			scale=0.5^.5
                        paperfile=file3+'4'
		endif else begin
			a3x=29.7
			a3y=41.7
			scale=1.0
                        paperfile=file3+'3'
		endelse

		; --------- define device so that the complete paper can be adressed --------------

	if (!version.os_family eq 'unix') then begin
                ; PS-File im entsprechenden Verzeichnis speichern (Region)
               ; regpath=region
                u='_'
                ;while (strpos(regpath,' ') ne -1) do begin
                   ; strput,regpath,u,strpos(regpath,' ')
                    ; strput,regpath,u,strpos(regpath,' ')
                ;endwhile
                paperfile='/'+paperfile
        endif else begin
            	paperfile='c:\RSI\'+paperfile
        endelse

        print,'Erzeuge Postscript-File: '+paperfile

		
		device,xoffset=0,yoffset=0,xsize=a3x,ysize=a3y
		
		WIDGET_CONTROL,Slider12,GET_VALUE=bpp           ; Slider-Stellung auslesen
		device,filename=paperfile,bits_per_pixel=bpp,color=0,/HELVETICA

                WIDGET_CONTROL,BGROUP17,GET_VALUE=pic           

                if (pic eq 0) then begin

                    ; ------------------------------ karte zeichnen -----------------------------------

                    map_set,var_lat_lon(8),var_lat_lon(9),/gnom, $
                      limit=[var_lat_lon(8)-8,var_lat_lon(9)-8,var_lat_lon(8)+8,var_lat_lon(9)+8], $
                      /continent, /hires, pos=[0.66,0.78,0.97,0.98], mlinestyle=0, mlinethick=1, $
                      lonlab=var_lat_lon(8)-7, latlab=var_lat_lon(9)-7,/grid,/label

                    longim=[var_lat_lon(1),var_lat_lon(3),var_lat_lon(7),var_lat_lon(5),var_lat_lon(1)]
                    latim =[var_lat_lon(0),var_lat_lon(2),var_lat_lon(6),var_lat_lon(4),var_lat_lon(0)]
                    plots,longim,latim,linestyle=0,thick=10

                    map_continents,/coast,/hires,/USA,/countries

                    xrand=0.5
                    yrand=0.5
                    titlesize=8.0*scale

                endif else begin

                    xrand=0.5
                    yrand=0.5+4.0*scale
                    titlesize=0
                endelse


		;-------------------- calculate the largest possible imagesize --------------------

		xgr=a3x-2*xrand
		ygr=a3y-2*yrand-titlesize

		HELVETICA=3				; define standard font to be HELVETICA

		imgr=size(im)
		if (imgr(1) gt (imgr(2)*xgr/ygr)) then begin
			xs=xgr
			ys=xgr*imgr(2)/imgr(1)
			x0=a3x-xrand-xs
			y0=a3y-(ygr-ys)/2-yrand-ys-titlesize
		endif else begin
			ys=ygr
			xs=ygr*imgr(1)/imgr(2)
			x0=a3x-xs-(xgr-xs)/2
			y0=a3y-yrand-ys-titlesize
		endelse

		;----------------- Ausgabe des Bildes abhaengig von der Flugrichtung --------------------

		mean=total(im)/n_elements(im)
		if (produkt eq 'GEC') then mean=mean*1.5      ; die schwarze Umrandung zieht den Mittelwert runter!
                minn=0
;                minn=min(im)*5.

		if (orient eq 0) then tvscl,im<2*mean>minn,x0,y0+.3,xsize=xs,ysize=ys,/centimeters
		if (orient eq 1) then tvscl,rotate(im<2*mean>minn,2),x0,y0+.3,xsize=xs,ysize=ys,/centimeters

                if (pic eq 0) then begin


                    sar='ERS-'+strtrim(string(var_platform),2)+' SAR '+strtrim(produkt,2)     ; Ueberschrift

                    ;------------------------------- Ausgabe der Daten --------------------------------------

                    xyouts,0.02,0.97,/normal,'printed with SAR-Tool v'+version+' (by G. Fiedler) '+systime(0) $
                      ,charsize=0.4*scale^.5
                    xyouts,0.02,0.94,charsize=3.0*scale,charthick=8.0*scale,/normal,sar,font=3
                    xyouts,0.02,0.91,charsize=3.0*scale,charthick=8.0*scale,/normal,title,font=3
                    xyouts,0.02,0.88,/normal,charsize=1.5*scale,'orbit: '+strtrim(string(var_orbit),2),font=3
                    xyouts,0.02,0.86,/normal,charsize=1.5*scale,'frame: '+strtrim(string(var_frame),2),font=3
                    xyouts,0.02,0.84,/normal,charsize=1.5*scale,'date:  '+strtrim(date,2),font=3
                    xyouts,0.02,0.82,/normal,charsize=1.5*scale,'time:  '+strtrim(time,2)+' UTC',font=3
                    xyouts,0.02,0.79,/normal,charsize=1.2*scale,'tape:  '+exabyte,font=3


                    for edge=0,6,2 do begin
                        if (var_lat_lon(edge)   lt 0) then ns=' S' else ns=' N'         ; +/- umwandeln in N/S
                        lat_lon_edge(edge)   = string(abs(var_lat_lon(edge)))+ns

			if (var_lat_lon(edge+1) lt 0) then ew=' W' else ew=' E'		; +/- umwandeln in E/W
			lat_lon_edge(edge+1) = string(abs(var_lat_lon(edge+1)))+ew
                    endfor

                    edge=strarr(8)
                    for ii=0,7 do begin
                        vor=floor(float(strmid(lat_lon_edge(ii),0,13)))            ;Nachkommastellen abschneiden
			nach=float(strmid(lat_lon_edge(ii),0,13))-vor		   ;und in 'nach' ablegen
			quad=strmid(lat_lon_edge(ii),strlen(lat_lon_edge(ii))-1,1) ; NSEW auslesen

			mins=nach*60					;Minuten = Nachkommaanteil * 60
			mins=round(mins)				;diese runden zu glatten Minuten
			if (mins eq 60) then begin
				vor=vor+1			; falls auf 60 hochgerundet -> 1 Grad mehr
				mins=0				; und die Minuten auf Null
			endif

			edge(ii)= strtrim(string(vor),2)+'!Uo!N'+ $
				  strtrim(string(mins),2)+''' '+quad    ; Eck-Koordinate (Grad+Min)basteln
                    endfor

                    lat_lon_edge=0 ; Array-Speicher wieder freigeben

                    xyouts,0.5 ,0.88 ,/normal,charsize=1.0*scale,alignment=0.5,font=3,'corner coordinates:'
                    xyouts,0.5 ,0.865,/normal,charsize=0.85*scale,alignment=0.5,font=3, $
                      edge(0)+', '+edge(1)+'  -  '+edge(2)+', '+edge(3)
                    xyouts,0.5 ,0.85 ,/normal,charsize=0.85*scale,alignment=0.5,font=3, $
                      edge(4)+', '+edge(5)+'  -  '+edge(6)+', '+edge(7)
                    xyouts,0.5 ,0.83,/normal,charsize=0.85*scale,alignment=0.5,font=3, $
                      'satellite heading : '+strtrim(string(round(float(var_heading))),2)+'!Uo!N'

                    plots,[0.42,0.46,0.46,0.42,0.42],[0.79,0.79,0.82,0.82,0.79],/norm
                    xyouts,0.44,0.78 ,/normal,charsize=0.85*scale,alignment=0.5,font=3,'North'

                    plots,[0.48,0.52,0.52,0.48,0.48],[0.79,0.79,0.82,0.82,0.79],/norm
                    xyouts,0.50,0.78 ,/normal,charsize=0.85*scale,alignment=0.5,font=3,'Flight'

                    plots,[0.54,0.58,0.58,0.54,0.54],[0.79,0.79,0.82,0.82,0.79],/norm
                    xyouts,0.56,0.78 ,/normal,charsize=0.85*scale,alignment=0.5,font=3,'Look'

                    ;einbau von Kaestchen fuer Norden usw
		    ;====================================

		    ;Norden
                    !p.position=[0.42,0.79,0.46,0.82]
                    plot,[-1,1],[-1,1],/nodata,xstyle=5,ystyle=5, /noerase
                    if (produkt ne 'GEC') then begin
                        if (orient eq 0) then pfeild,360.-var_heading
                        if (orient eq 1) then pfeild,360.0-(var_heading-180.)
                    endif else begin
                        pfeild,360.
                    endelse

		    ;Flugrichung
                    !p.position=[0.48,0.79,0.52,0.82]
                    plot,[-1,1],[-1,1],/nodata,xstyle=5,ystyle=5,/noerase
                    if (produkt ne 'GEC') then begin
                        if (orient eq 0) then pfeild,0.0
                        if (orient eq 1) then pfeild,180.
                    endif else begin
                        pfeild,var_heading
                    endelse

		    ;Blickrichtung
                    !p.position=[0.54,0.79,0.58,0.82]
                    plot,[-1,1],[-1,1],/nodata,xstyle=5,ystyle=5, /noerase
                    if (produkt ne 'GEC') then begin
                        if (orient eq 0) then pfeild,90.
                        if (orient eq 1) then pfeild,270.
                    endif else begin
                        pfeild,var_heading+90.
                    endelse

                    ;read_gif,gif_path,logo

                   ; tvscl,logo,13.*scale,37.7*scale,ysize=3.2*scale,/centimeters

		    ;========================================

                endif

		device,/close

		if (!version.os_family eq 'unix') then set_plot,'X'
		if (!version.os_family eq 'Windows') then set_plot,'Win'
		
		WIDGET_CONTROL,SLIDER9,GET_VALUE=copies		; Anzahl der Bilder eintragen
                if (copies gt 1) then begin
                    stelle=130
                    csave=bytarr(stelle)
                    cinsert=byte('copypageP')
                    cinsert(8)=byte(10)
                    openu,4,paperfile,/append
                    point_lun,-4,endpos
                    point_lun,4,endpos-stelle
                    readu,4,csave
                    point_lun,4,endpos-stelle
                    for i=1,copies-1 do begin
                        writeu,4,cinsert
                    end
                    writeu,4,csave
                    close,4
                endif

		;-----------------------------------------------------------------------------

		Widget_Control,BUTTON31,Sensitive=1		; Print ein
 		Widget_Control,BASE20,Sensitive=1		; Druckerauswahl ein

        	if (!version.os_family eq 'unix') then begin
			spawn,'chmod 777 '+paperfile
			Widget_Control,BUTTON29,Sensitive=1 ; Preview ein
		endif

        	print,'Fertig.'
  	      end

  'BUTTON29': begin						; preview mit Ghostview (?)
		WIDGET_CONTROL, /HOURGLASS
		WIDGET_CONTROL,BGROUP3,GET_VALUE=papersize
		papersize=papersize+3
 
		spawn,'ghostview -magstep -2 -a'+strtrim(string(papersize),2)+' '+paperfile+' &'

              end

  'BUTTON31': begin						; ausdrucken
                WIDGET_CONTROL, /HOURGLASS

		WIDGET_CONTROL,BGROUP21,GET_VALUE=printer 
		case printer of
			0: printaddress='psci2'
			1: printaddress='psi3'
			2: printaddress='psi9'
		endcase

		print,'Bild wird auf '+printaddress+' ausgedruckt.'
		spawn,'lpr -P'+printaddress+' '+paperfile
		print,'Fertig.'

              end

  'BUTTON30': begin						; Cancel print-menue
		WIDGET_CONTROL, event.top, /DESTROY
                Widget_Control,MAIN13,Sensitive=1               ; Hauptmenu Sar-Tools wieder aktivieren 
                RETURN
   	      end

  endcase
end


; -------------------------------------------------------------------------------------------------

pro printmenu, GROUP=Group

common var_header
common var_env
common var_names
common database
common main_widgets
common var_data
common print_widgets
common var_print, paperfile
common variable
common titel
common saving, speiort               ;zusaetzlich eingesetzt


;---------------- alle Headerdaten fuer die Datenbank wegschreiben -----------------

if (new eq 1) then begin             ; nur wenn Bild neu ist
    new=0
    restore,dbpath                   ; initialisiert in sar.pro
    s=size(datenbank)                ; Infos �ber Datenbank einlesen
    db_laenge=s(1)                   ; L�nge eines Datensatzes
    db_zaehler=s(2)                  ; Anzahl Datens�tze
    db_zaehler=fix(db_zaehler)       ; Long -> Int
    print,'Datenbank eingelesen ('+str(db_zaehler)+' Saetze).'

    print,'Title : ',title
    print,'Region: ',region
    print,'Trage Daten in Datenbank ein.'
    dbak=datenbank                              ; Daten sichern
    db_zaehler=db_zaehler+1                     ; Anzahl Datens�tze um 1 erh�hen
    datenbank=strarr(db_laenge,db_zaehler)             ; Datenbank initialisieren

    ; -----------------  Stelle ermitteln, an die neuer Satz kommt --------------------

    if (dbak(0,db_zaehler-2) lt tape_form) then begin ; Ganz hinten?
        index=db_zaehler-1
    endif else begin                             ; sonst durchsuchen
        i=0
        while (dbak(0,i) lt tape_form) do i=i+1
        index=i
    endelse
    print,index+1                              ; Ausgabe der Datenbank-Position

    if (index gt 0) then datenbank(0:db_laenge-1,0:index-1)=dbak(*,0:index-1) ; Daten zur�ckschreiben
    if (index lt db_zaehler-1) then datenbank(0:db_laenge-1,index+1:db_zaehler-1)=dbak(*,index:db_zaehler-2)

    datenbank( 0,index) =tape_form             ; neuen Datensatz eintragen
    datenbank( 1,index) =title
    datenbank( 2,index) =var_orbit
    datenbank( 3,index) =var_frame
    datenbank( 4,index) =date
    datenbank( 5,index) =time
    datenbank( 6,index) =var_platform
    datenbank( 7,index) =var_heading
    datenbank( 8,index) =var_linelength
    datenbank( 9,index) =var_numberoflines
    datenbank(10,index) =var_offset
    datenbank(11,index) =center_lon
    datenbank(12,index) =center_lat
    datenbank(13,index) =orient
    datenbank(14,index) =region
    datenbank(15,index) =var_lat_lon(0:7)    ; 8 Eckkoordinaten belegen Datenbank(15:22,*)
    datenbank(23,index) =version             ; Zeigt an, unter welcher SAR-Tool Version die Daten geschrieben wurden
                                             ; (f�r sp�tere Erweiterungen).
    datenbank(24,index) =PS_CD               ; Auf welcher CD ist das PS-File?
   ; datenbank(25,index) =produkt             ; PRI, GEC, FDC, QL
    save,datenbank,filename=dbpath           ; neue Datenbank auf Platte sichern

    dbak=0
endif



  IF N_ELEMENTS(Group) EQ 0 THEN GROUP=0

  junk   = { CW_PDMENU_S, flags:0, name:'' }


  MAIN3 = WIDGET_BASE(GROUP_LEADER=Group, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Print - Menu', $
      UVALUE='MAIN3', $
      XSIZE=300, $
      YSIZE=430)

  BASE2 = WIDGET_BASE(MAIN3, $
      ROW=1, $
      SPACE=50, $
      FRAME=2, $
      MAP=1, $
      TITLE='Groesse / Aufloesung', $
      UVALUE='BASE2')

  Btns170 = [ $
    'A3', $
    'A4' ]
  BGROUP3 = CW_BGROUP( BASE2, Btns170, $
      COLUMN=1, $
      EXCLUSIVE=1, $
      FRAME=2, $
      LABEL_TOP='Print-Size:', $
      UVALUE='BGROUP3', $
      SET_VALUE=1, $
      XSIZE=100, $
      YSIZE=60)

  BASE7 = WIDGET_BASE(BASE2, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Bits per Pixel / No. of Copies', $
      UVALUE='BASE7')

  SLIDER12 = WIDGET_SLIDER( BASE7, $
      MAXIMUM=16, $
      MINIMUM=1, $
      TITLE='Bits per Pixel', $
      UVALUE='SLIDER12', $
      VALUE=8)

  SLIDER9 = WIDGET_SLIDER( BASE7, $
      MAXIMUM=4, $
      MINIMUM=1, $
      TITLE='No. of Copies', $
      UVALUE='SLIDER9', $
      VALUE=1)



  BASE16 = WIDGET_BASE(MAIN3, $
      ROW=1, $
      MAP=1, $
      TITLE='Ausdruck mit oder ohne Data', $
      UVALUE='BASE16')

  Btns1338 = [ $
    'Picture with complete Data', $
    'Picture with Orbit/Frame, Date/Time', $
    'Picture without Data' ]
  BGROUP17 = CW_BGROUP( BASE16, Btns1338, $
      COLUMN=1, $
      EXCLUSIVE=1, $
      SET_VALUE=0, $
      UVALUE='BGROUP17')

  BASE27 = WIDGET_BASE(MAIN3, $
      ROW=1, $
      SPACE=90, $
      MAP=1, $
      TITLE='make PS - cancel', $
      UVALUE='BASE27')

  BUTTON28 = WIDGET_BUTTON( BASE27, $
      UVALUE='BUTTON28', $
      VALUE='Make PS-File', $
      XSIZE=100)

  BUTTON30 = WIDGET_BUTTON( BASE27, $
      UVALUE='BUTTON30', $
      VALUE='Cancel', $
      XSIZE=100)

  BASE20 = WIDGET_BASE(MAIN3, $
      COLUMN=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='Druckerauswahl', $
      UVALUE='BASE20')

  LABEL26 = WIDGET_LABEL( BASE20, $
      UVALUE='LABEL26', $
      VALUE='Destination Printer:')

  Btns1640 = [ $
    'psci2 - CLC-300', $
    'psi3  - Laserjet 5', $
    'psi9  - Laserjet 4' ]
  BGROUP21 = CW_BGROUP( BASE20, Btns1640, $
      COLUMN=1, $
      EXCLUSIVE=1, $
      SET_VALUE=1, $
      UVALUE='BGROUP21')


  BASE32 = WIDGET_BASE(MAIN3, $
      ROW=1, $
      SPACE=90, $
      MAP=1, $
      TITLE='print - preview', $
      UVALUE='BASE32')

  BUTTON29 = WIDGET_BUTTON( BASE32, $
      UVALUE='BUTTON29', $
      VALUE='Preview', $
      XSIZE=100)

  BUTTON31 = WIDGET_BUTTON( BASE32, $
      FONT='6x13bold', $
      UVALUE='BUTTON31', $
      VALUE='Print', $
      XSIZE=100)


  WIDGET_CONTROL, MAIN3, /REALIZE

  Widget_Control,BUTTON29,Sensitive=0		; Preview aus
  Widget_Control,BUTTON31,Sensitive=0		; Print aus

  Widget_Control,BASE20,Sensitive=0		; Druckerauswahl aus
  
  
  
  
  
  ;------------------------------------------------ab hier automatisierung----------------------------------------------------------------------
  
   
  
  
print,'erstelle PDF-file'
lat_lon_edge=strarr(8)


begin           ; Postscript-File erzeugen

    WIDGET_CONTROL, /HOURGLASS
  set_plot,'ps'

    ; ----------------------- actual papersize ------------------------------

    WIDGET_CONTROL,BGROUP3,GET_VALUE=papersize
    papersize=papersize+3
    
    if (papersize eq 4) then begin
      a3x=21.
      a3y=29.7
      scale=0.5^.5
                        paperfile=file3+'4'
    endif else begin
      a3x=29.7
      a3y=41.7
      scale=1.0
                        paperfile=file3+'3'
                        print, paperfile
    endelse

    ; --------- define device so that the complete paper can be adressed --------------

  if (!version.os_family eq 'unix') then begin
                ; PS-File im entsprechenden Verzeichnis speichern (Region)
               ; regpath=region
                u='_'
                ;while (strpos(regpath,' ') ne -1) do begin
                   ; strput,regpath,u,strpos(regpath,' ')
                    ; strput,regpath,u,strpos(regpath,' ')
; Hier wurden der Orbit und der Frame aus dem Bildnahmen genommen da hier keine luecken beim auslesen auftreten
                pictur=namevar(laufvar)
                noending=strmid(pictur,0,22) 
                orbitnumber=strmid(noending,10,6)
                framenumber=strmid(noending,17,5)
      print,pictur
                print, speiort+'/'+noending+'.ps4'
                print, 'wird da gespeichert'
                sat=strmid(noending,0,4)
if (sat eq 'ERS1')then paperfile=speiort+'/ERS1pdf/'+noending+'.ps4'
if (sat eq 'ERS2') then paperfile=speiort+'/ERS2pdf'+noending+'.ps4'
                

                print, paperfile
        endif else begin
              paperfile='c:\RSI\'+paperfile
        endelse

        print,'Erzeuge Postscript-File: '+paperfile

    
    device,xoffset=0,yoffset=0,xsize=a3x,ysize=a3y
    
    WIDGET_CONTROL,Slider12,GET_VALUE=bpp           ; Slider-Stellung auslesen
    device,filename=paperfile,bits_per_pixel=bpp,color=0,/HELVETICA

                WIDGET_CONTROL,BGROUP17,GET_VALUE=pic           

                if (pic eq 0) then begin

                    ; ------------------------------ karte zeichnen -----------------------------------

                    map_set,var_lat_lon(8),var_lat_lon(9),/gnom, $
                      limit=[var_lat_lon(8)-8,var_lat_lon(9)-8,var_lat_lon(8)+8,var_lat_lon(9)+8], $
                      /continent, /hires, pos=[0.66,0.78,0.97,0.98], mlinestyle=0, mlinethick=1, $
                      lonlab=var_lat_lon(8)-7, latlab=var_lat_lon(9)-7,/grid,/label

                    longim=[var_lat_lon(1),var_lat_lon(3),var_lat_lon(7),var_lat_lon(5),var_lat_lon(1)]
                    latim =[var_lat_lon(0),var_lat_lon(2),var_lat_lon(6),var_lat_lon(4),var_lat_lon(0)]
                    plots,longim,latim,linestyle=0,thick=10

                    map_continents,/coast,/hires,/USA,/countries

                    xrand=0.5
                    yrand=0.5
                    titlesize=8.0*scale

                endif else begin

                    xrand=0.5
                    yrand=0.5+4.0*scale
                    titlesize=0
                endelse


    ;-------------------- calculate the largest possible imagesize --------------------

    xgr=a3x-2*xrand
    ygr=a3y-2*yrand-titlesize

    HELVETICA=3       ; define standard font to be HELVETICA

    imgr=size(im)
    if (imgr(1) gt (imgr(2)*xgr/ygr)) then begin
      xs=xgr
      ys=xgr*imgr(2)/imgr(1)
      x0=a3x-xrand-xs
      y0=a3y-(ygr-ys)/2-yrand-ys-titlesize
    endif else begin
      ys=ygr
      xs=ygr*imgr(1)/imgr(2)
      x0=a3x-xs-(xgr-xs)/2
      y0=a3y-yrand-ys-titlesize
    endelse

    ;----------------- Ausgabe des Bildes abhaengig von der Flugrichtung --------------------

    mean=total(im)/n_elements(im)
    if (produkt eq 'GEC') then mean=mean*1.5      ; die schwarze Umrandung zieht den Mittelwert runter!
                minn=0
;                minn=min(im)*5.

    if (orient eq 0) then tvscl,im<2*mean>minn,x0,y0+.3,xsize=xs,ysize=ys,/centimeters
    if (orient eq 1) then tvscl,rotate(im<2*mean>minn,2),x0,y0+.3,xsize=xs,ysize=ys,/centimeters

                if (pic eq 0) then begin


                    sar='ERS-'+strtrim(string(var_platform),2)+' SAR '+strtrim(produkt,2)     ; Ueberschrift

                    ;------------------------------- Ausgabe der Daten --------------------------------------

                    xyouts,0.02,0.97,/normal,'printed with SAR-Tool v'+version+' (by G. Fiedler) change by B. Golebiowski'+systime(0) $
                      ,charsize=0.4*scale^.5
                    xyouts,0.02,0.94,charsize=3.0*scale,charthick=8.0*scale,/normal,sar,font=3
                    ;xyouts,0.02,0.91,charsize=3.0*scale,
                     xyouts,0.02,0.91,/normal,charsize=2.0*scale,'University of Hamburg, Institute of Oceanography',font=3                ;-------wir haben keine region
                    xyouts,0.02,0.88,/normal,charsize=1.5*scale,'orbit: '+orbitnumber,font=3
                    xyouts,0.02,0.86,/normal,charsize=1.5*scale,'frame: '+framenumber,font=3
                    xyouts,0.02,0.84,/normal,charsize=1.5*scale,'date:  '+strtrim(date,2),font=3
                    xyouts,0.02,0.82,/normal,charsize=1.5*scale,'time:  '+strtrim(time,2)+' UTC',font=3
                    xyouts,0.02,0.765,/normal,charsize=1.3*scale,'name:  '+noending+'.sar',font=3


                    for edge=0,6,2 do begin
                        if (var_lat_lon(edge)   lt 0) then ns=' S' else ns=' N'         ; +/- umwandeln in N/S
                        lat_lon_edge(edge) = string(abs(var_lat_lon(edge)))+ns

      if (var_lat_lon(edge+1) lt 0) then ew=' W' else ew=' E'   ; +/- umwandeln in E/W
      lat_lon_edge(edge+1) = string(abs(var_lat_lon(edge+1)))+ew
                    endfor

                    edge=strarr(8)
                    for ii=0,7 do begin
                        vor=floor(float(strmid(lat_lon_edge(ii),0,13)))            ;Nachkommastellen abschneiden
      nach=float(strmid(lat_lon_edge(ii),0,13))-vor      ;und in 'nach' ablegen
      quad=strmid(lat_lon_edge(ii),strlen(lat_lon_edge(ii))-1,1) ; NSEW auslesen

      mins=nach*60          ;Minuten = Nachkommaanteil * 60
      mins=round(mins)        ;diese runden zu glatten Minuten
      if (mins eq 60) then begin
        vor=vor+1     ; falls auf 60 hochgerundet -> 1 Grad mehr
        mins=0        ; und die Minuten auf Null
      endif

      edge(ii)= strtrim(string(vor),2)+'!Uo!N'+ $
          strtrim(string(mins),2)+''' '+quad    ; Eck-Koordinate (Grad+Min)basteln
                    endfor

                    lat_lon_edge=0 ; Array-Speicher wieder freigeben

                    xyouts,0.5 ,0.88 ,/normal,charsize=1.2*scale,alignment=0.5,font=3,'corner coordinates:'
                    xyouts,0.5 ,0.865,/normal,charsize=1.05*scale,alignment=0.5,font=3, $
                      edge(0)+', '+edge(1)+'  -  '+edge(2)+', '+edge(3)
                    xyouts,0.5 ,0.85 ,/normal,charsize=1.05*scale,alignment=0.5,font=3, $
                      edge(4)+', '+edge(5)+'  -  '+edge(6)+', '+edge(7)
                    xyouts,0.5 ,0.83,/normal,charsize=1.05*scale,alignment=0.5,font=3, $
                      'satellite heading : '+strtrim(string(round(float(var_heading))),2)+'!Uo!N'

                    plots,[0.42,0.46,0.46,0.42,0.42],[0.79,0.79,0.82,0.82,0.79],/norm
                    xyouts,0.44,0.78 ,/normal,charsize=1.05*scale,alignment=0.5,font=3,'North'

                    plots,[0.48,0.52,0.52,0.48,0.48],[0.79,0.79,0.82,0.82,0.79],/norm
                    xyouts,0.50,0.78 ,/normal,charsize=1.05*scale,alignment=0.5,font=3,'Flight'

                    plots,[0.54,0.58,0.58,0.54,0.54],[0.79,0.79,0.82,0.82,0.79],/norm
                    xyouts,0.56,0.78 ,/normal,charsize=1.05*scale,alignment=0.5,font=3,'Look'

                    ;einbau von Kaestchen fuer Norden usw
        ;====================================

        ;Norden
                    !p.position=[0.42,0.79,0.46,0.82]
                    plot,[-1,1],[-1,1],/nodata,xstyle=5,ystyle=5, /noerase
                    if (produkt ne 'GEC') then begin
                        if (orient eq 0) then pfeild,360.-var_heading
                        if (orient eq 1) then pfeild,360.0-(var_heading-180.)
                    endif else begin
                        pfeild,360.
                    endelse

        ;Flugrichung
                    !p.position=[0.48,0.79,0.52,0.82]
                    plot,[-1,1],[-1,1],/nodata,xstyle=5,ystyle=5,/noerase
                    if (produkt ne 'GEC') then begin
                        if (orient eq 0) then pfeild,0.0
                        if (orient eq 1) then pfeild,180.
                    endif else begin
                        pfeild,var_heading
                    endelse

        ;Blickrichtung
                    !p.position=[0.54,0.79,0.58,0.82]
                    plot,[-1,1],[-1,1],/nodata,xstyle=5,ystyle=5, /noerase
                    if (produkt ne 'GEC') then begin
                        if (orient eq 0) then pfeild,90.
                        if (orient eq 1) then pfeild,270.
                    endif else begin
                        pfeild,var_heading+90.
                    endelse

                    ;read_gif,gif_path,logo

                   ; tvscl,logo,13.*scale,37.7*scale,ysize=3.2*scale,/centimeters

        ;========================================

                endif

    device,/close

    if (!version.os_family eq 'unix') then set_plot,'X'
    if (!version.os_family eq 'Windows') then set_plot,'Win'
    
    WIDGET_CONTROL,SLIDER9,GET_VALUE=copies   ; Anzahl der Bilder eintragen
                if (copies gt 1) then begin
                    stelle=130
                    csave=bytarr(stelle)
                    cinsert=byte('copypageP')
                    cinsert(8)=byte(10)
                    openu,4,paperfile,/append
                    point_lun,-4,endpos
                    point_lun,4,endpos-stelle
                    readu,4,csave
                    point_lun,4,endpos-stelle
                    for i=1,copies-1 do begin
                        writeu,4,cinsert
                    end
                    writeu,4,csave
                    close,4
                endif

    ;-----------------------------------------------------------------------------

    Widget_Control,BUTTON31,Sensitive=1   ; Print ein
    Widget_Control,BASE20,Sensitive=1   ; Druckerauswahl ein

          if (!version.os_family eq 'unix') then begin
      spawn,'chmod 777 '+paperfile
      Widget_Control,BUTTON29,Sensitive=1 ; Preview ein
    endif

          print,'Fertig.'
          ;-------------------------------------quicklook speichern--------------------------------------

          
  print, 'Speicher Bild als Quicklook.'        
          mean=total(im)/n_elements(im)
    if (produkt eq 'GEC') then mean=mean*1.5      ; die schwarze Umrandung zieht den Mittelwert runter!
                minn=0


    bild=bytscl(im<2*mean>minn) 
  
  ; if (orient eq 1) then  begin 
   ; bild=bytscl, rotate(im<2*mean>minn,2) 
   ; end

sat=strmid(noending,0,4)
if (sat eq 'ERS1')then bildort=speiort+'/ERS1quicklook/'+noending+'.png'
if (sat eq 'ERS2') then bildort=speiort+'/ERS2quicklook/'+noending+'.png'


print, bildort


write_png, bildort, bild
spawn,'chmod 777 '+bildort
print, 'Bild wurde gespeichert.'
          end
;--------------------abbrechen da wir nicht drucken wollen


;--------------speichern des Bildes als quicklook im png
;begin
;mean=total(im)/n_elements(im)
   ; if (produkt eq 'GEC') then mean=mean*1.5      ; die schwarze Umrandung zieht den Mittelwert runter!
           ;     minn=0

;if (orient eq 0) then bild=bytscl(im<2*mean>minn)
   ;if (orient eq 1) then bild=bytscl,rotate(im<2*mean>minn,2)

;bildort=speiort+'/'+noending+'.png'
;print, bildort

;write_png, bildort, bild
;end
;------------------------------------------------------

begin           ; Cancel print-menue
    WIDGET_CONTROL, MAIN3, /DESTROY
                Widget_Control,MAIN13,Sensitive=1               ; Hauptmenu Sar-Tools wieder aktivieren 
                RETURN
          end

  XMANAGER, 'MAIN3', MAIN3
end
