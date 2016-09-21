
; NAME:
;	Sar.pro
;
; PURPOSE:
;	Tell what your routine does here.  I like to start with the words:
;	"This function (or procedure) ..."
;	Try to use the active, present tense.
;
; OUTPUTS:
;	Describe any outputs here.  For example, "This function returns the
;	foobar superflimpt version of the input array."  This is where you
;	should also document the return value for functions.
;
; COMMON BLOCKS:
;	BLOCK1:	Describe any common blocks here. If there are no COMMON
;		blocks, just delete this entry.
;
; EXTERNE PROZEDUREN:
;      Loadmenukl.pro        zum einlesen der Daten fuer das kleine Quicklook
;
;      Printmenukl.pro        zum erzeugen eines kleinen Quicklooks
;
;       Loadmenu.pro      zum einlesen von Daten (von Festplatte, CD-ROM oder Tape) 
;           Sarhead.pro   Headerdaten aus Headerfiles einlesen ((C) Asmus Michelsen)
;               blanks.pro           
;               sreadu.pro
;               str.pro
;               strisdigit.pro
;
;       Printmenu.pro     zum erzeugen eines Postscript-Files aus den Daten, das dann gedruckt werden kann
;           Pfeild.pro    erzeugt Kaestchen mit Richtungspfeil (Nord, Flight, Look) 
;         benoetigt:      Datenbank    (s. dpath)
;
;      pstopdf.pro          wandelt alle Postscript-Files in PDF-Dokumente um.
;
;      dbtxt.pro          schreibt die daten aus der Datenbank in ein textfile
;
;
; MODIFICATION HISTORY:
; 	Written by:	Gerald Fiedler
;	03.04.1997:	v0.1    Common Blocks eingefuegt, SubImage-Bereich aktiviert sich nach 'SubImage'
;	23.09.1997:	v1.0
;       20.01.1998:     v1.01   Datenbank-File einlesen
;       02.02.1998:     v1.05   Datens�tze sortiert (nach Tape-Nr.) speichern
;       16.04.1998:     v1.08   Save speichert Bild-Array in ~/cd/bilder/___.arr
;       26.11.1998:     v1.11   Title aus Pulldown-Menu waehlbar
;       23.02.1999:     v1.2    1. �ffentliche Version �ber Account UHSO (IDL 5.03, 5.11)
;       08.01.2009:     Automatisierung und umstellung auf neue Speicherort
;       16.01.2009:     Fertige automatisierung fehlerfrei (bis auf daten die nicht gelesen werden koennen

function PDMENU5_Event, Event
Full_Event=Event.Value
Event_Name=strmid(Full_Event,strpos(Full_event,'.',8)+1,strlen(Full_Event))

  case Event_Name OF
      'unknown'  :  print,'unknown'
      else       :  begin
                        return,Full_Event
                    end
  endcase
end



pro MAIN13_Event, Event

common main_widgets, 	MAIN13, BASE34, BASE40, BASE38, BASE71, BASE84, BASE185, $
		     	BUTTON92, BUTTON117, BUTTON118, BUTTON119, BUTTON126, BUTTON135, BUTTON177, $
		     	FIELD30, FIELD31, FIELD32, FIELD33, FIELD37, FIELD72, FIELD73, $
		     	FIELD85, FIELD86, FIELD87, FIELD104, FIELD139, $
		     	BGROUP43, BGROUP59, PDMENU5

common var_names,    	file, file0, file1, file2, file3, fpath, exabyte, title, fname, region

common var_env,      	modus, res, orient, noheading, date, time, center_lon, center_lat, version, $
                        win_map, win_show, lw, comp

common var_header,    var_linelength, var_numberoflines, var_offset, var_orbit, var_frame, var_platform, $
		     	var_procdatestr, var_paf, var_aqu_dat, var_aqu_time, var_heading, var_lat_lon,$
		     	 var_calibrationconst, produkt

common var_data,     	im

common database,        datenbank, db_laenge, db_zaehler, tape_form, new, PS_CD, dbpath

common load_path,       lw_path


;---------------------------- Reaktionen auf Eingaben -------------------------------


WIDGET_CONTROL,Event.Id,GET_UVALUE=Ev

case Ev OF

  'BUTTON137': begin
        Widget_Control,MAIN13,Sensitive=0      
       	loadmenu					; Load-Menue starten
      end

  'BUTTON117': begin	                                ; Array speichern
      if (strmid(exabyte,strlen(exabyte)-2,1) ne '/') then begin
      print, exabyte
;          saveim='~/'+exabyte+'.im'
          saveim='/'+exabyte+'.im'
      endif else begin
;          saveim='~/'+strmid(exabyte,0,strlen(exabyte)-2)+'_'+strmid(exabyte,strlen(exabyte)-1,1)+'.im'
          saveim=strmid(exabyte,0,strlen(exabyte)-2)+'_'+strmid(exabyte,strlen(exabyte)-1,1)+'.im'
     endelse
      print,'Array speichern unter '+saveim
      save,im,filename=saveim
      print,'Fertig.'
      end

  'BUTTON118': begin					; PS-File erzeugen und ausdrucken
        Widget_Control,MAIN13,Sensitive=0               ; Hauptmenu deaktivieren
	WIDGET_CONTROL,FIELD104,GET_VALUE=var_heading
	WIDGET_CONTROL,FIELD139,GET_VALUE=file
	WIDGET_CONTROL,FIELD30,GET_VALUE=var_orbit
	WIDGET_CONTROL,FIELD31,GET_VALUE=var_frame
	WIDGET_CONTROL,FIELD32,GET_VALUE=date
	WIDGET_CONTROL,FIELD33,GET_VALUE=time
	WIDGET_CONTROL,FIELD37,GET_VALUE=ttemp		; vorher Bildtitel auslesen
        if (title ne ttemp(0)) or (ttemp(0) eq 'somewhere') then begin
            region='unknown'
            title=ttemp(0)
        end

	printmenu					; Print-Menue starten
      end

  'BUTTON119': begin
      Print, 'Event for Scan'
      end

  'BUTTON135': begin					; Uebersichtskarte ausgeben
      win_map=1                                         ; var_env setzen
 	WIDGET_CONTROL, /HOURGLASS
	WINDOW,0,XSIZE=400,YSIZE=400,TITLE='Map - '+exabyte
        map_set,var_lat_lon(8),var_lat_lon(9),/gnom, $
          limit=[var_lat_lon(8)   ,var_lat_lon(9)-10, $
                 var_lat_lon(8)+10,var_lat_lon(9)   , $
                 var_lat_lon(8)   ,var_lat_lon(9)+10, $
                 var_lat_lon(8)-10,var_lat_lon(9)  ], $
          /continent, /hires, pos=[0.,0.,1,1], mlinestyle=0, mlinethick=1, $
          lonlab=var_lat_lon(8)-7, latlab=var_lat_lon(9)-7,/grid,/label

	longim=[var_lat_lon(1), var_lat_lon(3), var_lat_lon(7), var_lat_lon(5), var_lat_lon(1)]
	latim =[var_lat_lon(0), var_lat_lon(2), var_lat_lon(6), var_lat_lon(4), var_lat_lon(0)]

	plots, longim, latim, linestyle=0, thick=5
	map_continents, /coast, /hires, /USA, /countries
    end

  'BUTTON126': begin					; reduziertes Bild in Extra-Fenster ausgeben
        win_show=1                                      ; var_env setzen
	WIDGET_CONTROL, /HOURGLASS
	mean=total(im)/n_elements(im)
	imsize=size(im)
	ximsize=imsize(1)
	yimsize=imsize(2)
 	WINDOW, 1, XSIZE=ximsize, YSIZE=yimsize, $
		   TITLE='ERS-'+strmid(string(var_platform), strlen(var_platform)-1, 1)+' SAR PRI ('+exabyte+')'
	minn=0
        if (orient eq 0) then tvscl,im<mean*2>minn
        if (orient eq 1) then tvscl,rotate(im<mean*2>minn,2)

      end

  'BUTTON134': begin
      Print, 'Event for Datenbank'

      end

  'PDMENU5': begin
        Full_Event=PDMENU5_Event(Event)
        Event_Name=strmid(Full_Event,strpos(Full_event,'.',8)+1,strlen(Full_Event))
        region=strmid(Full_Event,7,strlen(Full_Event)-7-strlen(Event_Name)-1)
        WIDGET_CONTROL,FIELD37,SET_VALUE=Event_Name
        title=Event_Name
      end

  'BGROUP59': begin
      case Event.Value OF
          0: orient=0           ; Print,'Button ascending Pressed'
          1: orient=1           ; Print,'Button decending Pressed'
          ELSE: Message,'Unknown button pressed'
      endcase
      end

  'BGROUP43': begin
      	case Event.Value OF
	      0: var_platform=1					; Print,'Button ERS-1 Pressed'
	      1: var_platform=2					; Print,'Button ERS-2 Pressed'
	   ELSE: Message,'Unknown button pressed'
      	endcase
      end

  'BUTTON187': begin
      Print, 'Event for SubImage'
	Widget_Control,BASE185,Sensitive=1			; SubImage Size aktivieren
	modus=2							; Im Loadmenu wird SubImage aktiviert
	print,modus
      end

  'BUTTON92': begin
      Print, 'Event for More Attributes'
      end

  'BUTTON177': begin
      print,'melde CD-ROM ab'
      spawn,'umount /cdrom'          
      Widget_Control,BUTTON177,Sensitive=0              ; Mount
      WIDGET_CONTROL,BUTTON177,SET_VALUE='Mount'
      end

  'BUTTON176': begin
        Print, 'SAR-Tools beendet.'
        if (win_map ne 0) then wdelete,0                ; Windows schliessen
        if (win_show ne 0) then wdelete,1

        WIDGET_CONTROL,BUTTON177,GET_VALUE=mount
        if (mount eq 'Unmount') then begin
            cd,lw
            print,'melde CD-ROM ab'
            spawn,'umount /cdrom'              ; CD-ROM abmelden
            Widget_Control,BUTTON177,Sensitive=0 ; Mount
            WIDGET_CONTROL,BUTTON177,SET_VALUE='Mount'
        end

	WIDGET_CONTROL, event.top, /DESTROY
        exit                                                        ; IDL beenden
      end

  endcase
    
      
end


; -----------------------------------------------------------------------------------------

pro sar,Filenr,GROUP=Group

common main_widgets
common var_env
common var_header
common var_names
common database
common abbruch_ende, abbruchvar
common load_path, lw_path
;common variable
common saving, speiort
common variable, laufvar 
common titel, namevar
common loading, datenort



;lw_path='/pf/u/u241112/Arbeit/'
lw_path='/users/ifmlinux20c/ifmrs/u241028/FEW3O/satellites/ERS-1/ERS1SAR'

var_linelength=''		; Initialisierung der Variablen
var_numberoflines=''		; aus dem Common Block 'var_header'
var_offset=''
var_orbit=''
var_frame=''
var_platform=''
var_procdatestr=''
var_paf=''
var_aqudatestr=''
var_heading=''
var_lat_lon=strarr(10)
var_calibrationconst=''
produkt='PRI'

modus=0				; Initialisierung der Variablen
file=''				; aus dem Common Block 'var_env'
res=8
date='unknown'
time='unknown'
center_lon='unknown'
center_lat='unknown'
noheading=1
orient=0
version='1.32'
win_map=0                       ; MAP-Window geoeffnet?
win_show=0                      ; SHOW-Window geoeffnet?

title='somewhere'               ; Initialisierung der Variablen
exabyte=''     	                ; aus dem Common Block 'var_names'
region='unknown'

if (!version.os_family eq 'unix') then dbpath='~u241112/Arbeit/Sar-Tool/Datenbank' else dbpath='C:\RSI\Datenbank'
;dbpath='/users/ifmlinux20c/ifmrs/u241028/FEW3O/Programme/IDL/Datenbank' else dbpath='C:\RSI\Datenbank'
tape_form=' '			; aus dem Common Block 'database'
PS_CD='7'

;---------------------------------------------------------------------------------

if (N_ELEMENTS(Filenr) ne 0) then fname=STRING(Filenr)
if (N_ELEMENTS(Group) eq 0) then GROUP=0

junk   = { CW_PDMENU_S, flags:0, name:'' }


  MAIN13 = WIDGET_BASE(GROUP_LEADER=Group, $
      COLUMN=1, $
      SPACE=20, $
      MAP=1, $
      TITLE='SAR - Tools v'+version, $
      UVALUE='MAIN13')

  BASE120 = WIDGET_BASE(MAIN13, $
      ROW=1, $
      MAP=1, $
      TITLE='Buttons', $
      UVALUE='BASE120')

  BUTTON137 = WIDGET_BUTTON( BASE120, $
      FRAME=4, $
      UVALUE='BUTTON137', $
      VALUE='Load', $
      XSIZE=50)

  BUTTON117 = WIDGET_BUTTON( BASE120, $
      FRAME=4, $
      UVALUE='BUTTON117', $
      VALUE='Save', $
      XSIZE=50)

  BUTTON118 = WIDGET_BUTTON( BASE120, $
      FRAME=4, $
      UVALUE='BUTTON118', $
      VALUE='Print', $
      XSIZE=50)

  BUTTON119 = WIDGET_BUTTON( BASE120, $
      UVALUE='BUTTON119', $
      VALUE='Scan', $
      XSIZE=50)

  BUTTON135 = WIDGET_BUTTON( BASE120, $
      UVALUE='BUTTON135', $
      VALUE='Map', $
      XSIZE=50)

  BUTTON126 = WIDGET_BUTTON( BASE120, $
      UVALUE='BUTTON126', $
      VALUE='Show', $
      XSIZE=50)

  BUTTON134 = WIDGET_BUTTON( BASE120, $
      UVALUE='BUTTON134', $
      VALUE='DBase', $
      XSIZE=50)


  BASE15 = WIDGET_BASE(MAIN13, $
      ROW=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='Title', $
      UVALUE='BASE15', $
      XSIZE=400)

  FIELD139 = CW_FIELD( BASE15,VALUE=fname, $			; wenn mit Filename gestartet -> uebernehmen
      COLUMN=1, $
      STRING=1, $
      TITLE='Filename', $
      UVALUE='FIELD139', $
      XSIZE=10, $
      YSIZE=1)

  FIELD37 = CW_FIELD( BASE15,VALUE=title, $
      COLUMN=1, $
      STRING=1, $
      TITLE='Title:', $
      UVALUE='FIELD37', $
      XSIZE=25, $
      YSIZE=1)

  MenuDesc291=replicate({CW_PDMENU_S},112)                      ; Aufteilung noetig, da max 90 Eintraege moeglich
  MenuDesc291(0:61) = [ $
      { CW_PDMENU_S,       3, 'Titles' }, $                    ;        0
        { CW_PDMENU_S,       1, 'Baltic Sea' }, $              ;        1
          { CW_PDMENU_S,       0, 'Kattegat' }, $              ;        2
          { CW_PDMENU_S,       0, 'Samso Belt' }, $            ;        3
          { CW_PDMENU_S,       0, 'Store Belt' }, $            ;        4
          { CW_PDMENU_S,       0, 'Smalandfarvandet' }, $      ;        5
          { CW_PDMENU_S,       0, 'Ore Sund' }, $              ;        6
          { CW_PDMENU_S,       0, 'Copenhagen' }, $            ;        7
          { CW_PDMENU_S,       0, 'Bight of Kiel' }, $         ;        8
          { CW_PDMENU_S,       0, 'Bight of Luebeck' }, $      ;        9
          { CW_PDMENU_S,       0, 'Mecklenburg Bay' }, $       ;       10
          { CW_PDMENU_S,       0, 'Neubrandenburg' }, $        ;       78
          { CW_PDMENU_S,       0, 'Schwerin' }, $              ;       11
          { CW_PDMENU_S,       0, 'Arkona Sea' }, $            ;       12
          { CW_PDMENU_S,       0, 'Pomeranian Bight' }, $      ;       13
          { CW_PDMENU_S,       0, 'Bornholm Sea' }, $          ;       14
          { CW_PDMENU_S,       0, 'Bornholm' }, $              ;       15
          { CW_PDMENU_S,       0, 'Bay of Gdansk' }, $         ;       16
          { CW_PDMENU_S,       0, 'SE Gotland Sea' }, $        ;       17
          { CW_PDMENU_S,       0, 'West. Gotland Sea' }, $     ;       18
          { CW_PDMENU_S,       0, 'Gotland' }, $               ;       19
          { CW_PDMENU_S,       0, 'Eastern Gotland Sea' }, $   ;       20
          { CW_PDMENU_S,       0, 'Gulf of Riga' }, $          ;       21
          { CW_PDMENU_S,       0, 'Gulf of Finland' }, $       ;       22
          { CW_PDMENU_S,       0, 'Stockholm' }, $             ;       23
          { CW_PDMENU_S,       0, 'Aland Sea' }, $             ;       24
          { CW_PDMENU_S,       0, 'Gulf of Bothnia' }, $       ;       25
          { CW_PDMENU_S,       0, 'N Gulf of Bothnia' }, $     ;       26
          { CW_PDMENU_S,       2, 'Lake Ladoga' }, $           ;       27
        { CW_PDMENU_S,       1, 'North Sea' }, $               ;       28
          { CW_PDMENU_S,       0, 'North Sea' }, $             ;       83
          { CW_PDMENU_S,       0, 'Skagerrak' }, $             ;       79
          { CW_PDMENU_S,       0, 'Denmark West Coast' }, $    ;       82
          { CW_PDMENU_S,       0, 'German Bight' }, $          ;       29
          { CW_PDMENU_S,       0, 'Rhine Plume' }, $           ;       30
          { CW_PDMENU_S,       0, 'Holland West Coast' }, $    ;       84
          { CW_PDMENU_S,       0, 'Nordfries. Inseln' }, $     ;       31
          { CW_PDMENU_S,       0, 'Westfries. Inseln' }, $     ;       81
          { CW_PDMENU_S,       2, 'Ostfries. Inseln' }, $      ;       32
        { CW_PDMENU_S,       1, 'Strait of Messina' }, $       ;       33
          { CW_PDMENU_S,       0, 'Strait of Messina' }, $     ;       34
          { CW_PDMENU_S,       0, 'Calabria' }, $              ;       35
          { CW_PDMENU_S,       0, 'Salerno' }, $               ;       36
          { CW_PDMENU_S,       0, 'Golfo di Salerno' }, $      ;       37
          { CW_PDMENU_S,       0, 'Sicilia' }, $               ;       38
          { CW_PDMENU_S,       0, 'Isole Lipari' }, $          ;       39
          { CW_PDMENU_S,       0, 'Stromboli' }, $             ;       40
          { CW_PDMENU_S,       0, 'Ionian Sea' }, $            ;       41
          { CW_PDMENU_S,       2, 'Tyrrhenian Sea' }, $        ;       42
        { CW_PDMENU_S,       1, 'Strait of Gibraltar' }, $     ;       43
          { CW_PDMENU_S,       0, 'Strait of Gibraltar' }, $   ;       44
          { CW_PDMENU_S,       0, 'Costa del Sol' }, $         ;       45
          { CW_PDMENU_S,       0, 'Sevilla' }, $               ;      102
          { CW_PDMENU_S,       0, 'Golfo de Cadiz' }, $        ;       46
          { CW_PDMENU_S,       0, 'E of Strait of Gibraltar' }, $ ;    96
          { CW_PDMENU_S,       0, 'Isla de Alboran' }, $       ;       95
          { CW_PDMENU_S,       0, 'Morocco North Coast' }, $   ;       47
          { CW_PDMENU_S,       2, 'Morocco West Coast' }, $    ;       48
        { CW_PDMENU_S,       1, 'NW Mediterranean' }, $        ;       85
          { CW_PDMENU_S,       0, 'Gulf of Lion' }, $          ;       51
          { CW_PDMENU_S,       0, 'S of Gulf of Lion' }, $     ;       87
          { CW_PDMENU_S,       0, 'Costa Dorada' }]            ;       86

  MenuDesc291(62:111) = [ $
          { CW_PDMENU_S,       0, 'S of Costa Dorada' }, $     ;       91
          { CW_PDMENU_S,       0, 'Costa Brava' }, $           ;       88
          { CW_PDMENU_S,       0, 'Mallorca' }, $              ;       89
          { CW_PDMENU_S,       2, 'Menorca' }, $               ;       90
        { CW_PDMENU_S,       1, 'Mediterranean' }, $           ;       49
          { CW_PDMENU_S,       0, 'Cyprus to Port Said' }, $   ;       50
          { CW_PDMENU_S,       0, 'Cyprus' }, $                ;      110
          { CW_PDMENU_S,       0, 'Port Said' }, $             ;      111
          { CW_PDMENU_S,       0, 'East Mediterranean' }, $    ;      107  
          { CW_PDMENU_S,       0, 'Venice' }, $                ;      105
          { CW_PDMENU_S,       0, 'Gulf of Korinth' }, $       ;       75
          { CW_PDMENU_S,       2, 'Danube Delta' }, $          ;       53
        { CW_PDMENU_S,       1, 'North Atlantic Ocean' }, $    ;       54
          { CW_PDMENU_S,       0, 'USA East Coast' }, $        ;       55
          { CW_PDMENU_S,       0, 'Gulf Stream' }, $           ;       92
          { CW_PDMENU_S,       0, 'Newfoundland' }, $          ;       98
          { CW_PDMENU_S,       0, 'Spitsbergen' }, $           ;       97
          { CW_PDMENU_S,       0, 'Jan Mayen' }, $             ;       56
          { CW_PDMENU_S,       0, 'Shetland Islands' }, $      ;      106 
          { CW_PDMENU_S,       0, 'Orkney Islands' }, $        ;      109
          { CW_PDMENU_S,       0, 'Iceland' }, $               ;       57
          { CW_PDMENU_S,       0, 'Iceland - Faroer' }, $      ;      104
          { CW_PDMENU_S,       0, 'Greenland' }, $             ;       58
          { CW_PDMENU_S,       0, 'Greenland Sea' }, $         ;      101  
          { CW_PDMENU_S,       0, 'Norwegian Sea' }, $         ;      100  
          { CW_PDMENU_S,       0, 'Ampere Seamount' }, $       ;      103    
          { CW_PDMENU_S,       0, 'Portugal West Coast' }, $   ;       80
          { CW_PDMENU_S,       0, 'Irish Sea' }, $             ;       94
          { CW_PDMENU_S,       2, 'Bay of Biscay' }, $         ;       59
        { CW_PDMENU_S,       1, 'South Atlantic Ocean' }, $    ;       60
          { CW_PDMENU_S,       0, 'French Guiana' }, $         ;      112  *  
          { CW_PDMENU_S,       2, 'Off Angola' }, $            ;       61
        { CW_PDMENU_S,       1, 'Pacific Ocean' }, $           ;       62
          { CW_PDMENU_S,       0, 'Canada West Coast' }, $     ;       63
          { CW_PDMENU_S,       0, 'USA West Coast' }, $        ;       77
          { CW_PDMENU_S,       0, 'Equatorial Pacific' }, $    ;       99
          { CW_PDMENU_S,       2, 'Galapagos Islands' }, $     ;       64
        { CW_PDMENU_S,       1, 'Asia' }, $                    ;       65
          { CW_PDMENU_S,       0, 'Lake Baikal' }, $           ;       93
          { CW_PDMENU_S,       2, 'Caspian Sea' }, $           ;       66
        { CW_PDMENU_S,       1, 'Southeast Asia' }, $          ;       67
          { CW_PDMENU_S,       0, 'South China Sea' }, $       ;       68
          { CW_PDMENU_S,       0, 'Yellow Sea' }, $            ;      108
          { CW_PDMENU_S,       0, 'Indonesia' }, $             ;       69
          { CW_PDMENU_S,       0, 'Sumatra' }, $               ;       74
          { CW_PDMENU_S,       0, 'Gulf of Thailand' }, $      ;       76
          { CW_PDMENU_S,       2, 'Strait of Malacca' }, $     ;       73
        { CW_PDMENU_S,       3, 'Indian Ocean' }, $            ;       70
          { CW_PDMENU_S,       0, 'Andaman Sea' }, $           ;       71
          { CW_PDMENU_S,       2, 'Arabian Sea' } $            ;       72
  ]

  PDMENU5 = CW_PDMENU( BASE15, MenuDesc291,/RETURN_FULL_NAME, $
      UVALUE='PDMENU5')


  BASE34 = WIDGET_BASE(MAIN13, $
      ROW=1, $
      SPACE=10, $
      MAP=1, $
      TITLE='Orbit', $
      UVALUE='BASE34', $
      XSIZE=400)

  FIELD30 = CW_FIELD( BASE34,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Orbit:', $
      UVALUE='FIELD30', $
      XSIZE=12, $
      YSIZE=1)

  FIELD31 = CW_FIELD( BASE34,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Frame:', $
      UVALUE='FIELD31', $
      XSIZE=12, $
      YSIZE=1)

  FIELD32 = CW_FIELD( BASE34,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Date:', $
      UVALUE='FIELD32', $
      XSIZE=12, $
      YSIZE=1)

  FIELD33 = CW_FIELD( BASE34,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Time:', $
      UVALUE='FIELD33', $
      XSIZE=12, $
      YSIZE=1)

  BASE38 = WIDGET_BASE(MAIN13, $
      ROW=1, $
      SPACE=20, $
      MAP=1, $
      TITLE='Spezial', $
      UVALUE='BASE38', $
      XSIZE=400)

  BASE40 = WIDGET_BASE(BASE38, $
      COLUMN=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='Direction', $
      UVALUE='BASE40')

  FIELD104 = CW_FIELD( BASE40,VALUE='', $
      ROW=1, $
      FLOAT=1, $
      TITLE='Direction:', $
      UVALUE='FIELD104', $
      XSIZE=6, $
      YSIZE=1)

  Btns148 = [ $
    'ascending', $
    'decending' ]
  BGROUP59 = CW_BGROUP( BASE40, Btns148, $
      COLUMN=1, $
      EXCLUSIVE=1, $
      UVALUE='BGROUP59')

  BASE42 = WIDGET_BASE(BASE38, $
      ROW=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='Platform', $
      UVALUE='BASE42')

  Btns151 = [ $
    'ERS-1', $
    'ERS-2' ]
  BGROUP43 = CW_BGROUP( BASE42, Btns151, $
      ROW=2, $
      EXCLUSIVE=1, $
      LABEL_TOP='Platform', $
      UVALUE='BGROUP43')


  BASE71 = WIDGET_BASE(BASE38, $
      COLUMN=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='Center Koordinates', $
      UVALUE='BASE71')

  LABEL81 = WIDGET_LABEL( BASE71, $
      UVALUE='LABEL81', $
      VALUE='Center Coordinates')

  FIELD72 = CW_FIELD( BASE71,VALUE='', $
      ROW=1, $
      STRING=1, $
      TITLE='Longitude:', $
      UVALUE='FIELD72', $
      XSIZE=10, $
      YSIZE=1)

  FIELD73 = CW_FIELD( BASE71,VALUE='', $
      ROW=1, $
      STRING=1, $
      TITLE=' Latitude:', $
      UVALUE='FIELD73', $
      XSIZE=10, $
      YSIZE=1)

  BASE84 = WIDGET_BASE(MAIN13, $
      ROW=1, $
      MAP=1, $
      TITLE='Image Size', $
      UVALUE='BASE84')

  FIELD85 = CW_FIELD( BASE84,VALUE='', $
      ROW=1, $
      INTEGER=1, $
      TITLE='Linelength:', $
      UVALUE='FIELD85', $
      XSIZE=5, $
      YSIZE=1)

  FIELD86 = CW_FIELD( BASE84,VALUE='', $
      ROW=1, $
      INTEGER=1, $
      TITLE='Number of Lines:', $
      UVALUE='FIELD86', $
      XSIZE=5, $
      YSIZE=1)

  FIELD87 = CW_FIELD( BASE84,VALUE='', $
      ROW=1, $
      INTEGER=1, $
      TITLE='Headerlength:', $
      UVALUE='FIELD87', $
      XSIZE=5, $
      YSIZE=1)

goto,xx19

  BASE153 = WIDGET_BASE(MAIN13, $
      COLUMN=1, $
      FRAME=2, $
      MAP=1, $
      TITLE='SubImage', $
      UVALUE='BASE153')

  BASE180 = WIDGET_BASE(BASE153, $
      ROW=1, $
      MAP=1, $
      TITLE='SubImage', $
      UVALUE='BASE180')

  BUTTON187 = WIDGET_BUTTON( BASE180, $
      UVALUE='BUTTON187', $
      VALUE='SubImage', $
      XSIZE=100)


  BASE185 = WIDGET_BASE(BASE153, $
      ROW=1, $
      MAP=1, $
      TITLE='SubImage-Size', $
      UVALUE='BASE185')

  FIELD181 = CW_FIELD( BASE185,VALUE='', $
      ROW=1, $
      INTEGER=1, $
      TITLE='X-Start:', $
      UVALUE='FIELD181', $
      XSIZE=4)

  FIELD182 = CW_FIELD( BASE185,VALUE='', $
      ROW=1, $
      INTEGER=1, $
      TITLE='Y-Start:', $
      UVALUE='FIELD182', $
      XSIZE=4)

  FIELD183=CW_FIELD(BASE185,VALUE='',$
      ROW=1, $
      INTEGER=1, $
      TITLE='X-Size:', $
      UVALUE='FIELD183', $
      XSIZE=4)

  FIELD184 = CW_FIELD ( BASE185,VALUE=' ', $
      ROW=1, $
      INTEGER=1, $
      TITLE='Y-Size:', $
      UVALUE='FIELD184', $
      XSIZE=4)

xx19:

  BASE91 = WIDGET_BASE(MAIN13, $
      ROW=1, $
      SPACE=45, $
      MAP=1, $
      TITLE='More Attributes', $
      UVALUE='BASE91')

goto,xx20
  BUTTON92 = WIDGET_BUTTON( BASE91, $
      UVALUE='BUTTON92', $
      VALUE='More Attributes', $
      XSIZE=110)
xx20:

  BUTTON176 = WIDGET_BUTTON( BASE91, $
      UVALUE='BUTTON176', $
      VALUE='Exit', $
      XSIZE=110)

  BUTTON177 = WIDGET_BUTTON( BASE91, $
      UVALUE='BUTTON177', $
      VALUE='Mount', $
      XSIZE=110)



  Widget_Control,BUTTON117,Sensitive=0		; Save
  Widget_Control,BUTTON118,Sensitive=0		; Print
  Widget_Control,BUTTON119,Sensitive=0		; Scan
  Widget_Control,BUTTON135,Sensitive=0		; Map
  Widget_Control,BUTTON126,Sensitive=0		; Show
;  Widget_Control,BUTTON92,Sensitive=0		; More Attr.

  Widget_Control,BASE34,Sensitive=0		; Orbit/Frame
  Widget_Control,BASE40,Sensitive=0		; Direction
  Widget_Control,BASE38,Sensitive=0		; Platform
  Widget_Control,BASE71,Sensitive=0		; Koordinaten
  Widget_Control,BASE84,Sensitive=0		; Image Size
;  Widget_Control,BASE185,Sensitive=0	; SubImage Size

  Widget_Control,BUTTON177,Sensitive=0		; Mount
     
  WIDGET_CONTROL, MAIN13, /REALIZE
  
 ; 
;-------------------------Ab hier wurde es auf automatic programiert----------------------------------------------------------------
;datenort='/pf/u/u241112/'
datenort='/users/ifmlinux20c/ifmrs/u241028/FEW3O/satellites/ERS-1/ERS1SAR/'
;datenort='/users/ifmlinux20c/ifmrs/u241028/FEW3O/satellites/ERS-2/ERS2SAR'

cd, datenort                 ;Verzeichnis wo die einzulesenden Daten liegen

print, datenort
spawn,'ls *.sar', namevar                                                                         ; speichern aller dateinamen in dei variable namevar
;print, namevar
b=N_ELEMENTS(namevar)                                                                    ;Schleifen ende, ist gleich der anzahl der vorhandenen daten
;b=1        
a=985           ; Schleifen anfang, kann verstellt werden wenn eine datei nicht eingelesen werden kann
print, b
 ;For laufvar = 0,b-1,1 do begin                                                 ; endevar-1 ,1  do begin
   For laufvar = a,b-1,1 do begin                      
    print, 'Erstelle Bild nr. '
    print, laufvar+1
    print, namevar(laufvar)
    speiort='/users/ifmlinux20c/ifmrs/u241028/FEW3O/DeMarine/Vorlaufigeergebnisse'
    ;speiort='/pf/u/u241112/Arbeit/ERS1neu'                                  ; wohin die eingelesenden daten gespeichert werden sollen
    ;speiort='/pf/u/u241112/Arbeit/ERS2'
;----------------------------LOAD--------------------------------------
print,'Lade nun das Bild'    

   loadmenukl        ; Load-menue wird gestartet und das kleine Quicklook erstellt (slider54 auf 20 gestellt)
   
   if (abbruchvar eq 0) then begin     ; wenn die Daten noch nicht eingelesen wurden werden sie eingelesen, ansonsten werden sie uebersprungen

                  printmenukl       ;speichern des kl Quicklooks
   
                  loadmenu          ; Load-Menue starten Laden der datei, das grosse Quicklook wird erstellt (fuer die pdf, slider54 auf 8)

;------------------------------PDF----------------------------------     

                  print,'Es wird ein PDF file erzeugt.'     
 
                  printmenu         ; Print-Menue starten

                  print, 'Die '
                  print, (laufvar+1)
                  print, 'Datei wurde erstellt.'

            endif else begin 
            print, "Daten schon vorhanden"
            endelse
            
endfor

    pstopdf          ;programm zum umwandeln der Postscript-files in PDF-Files, die Postscript-files werden dann geloescht

    dbtxt            ;erstellt eine Textdatei die in die Datenbank eingelesen werden kann

print,'Ende'
   WIDGET_CONTROL,MAIN13, /DESTROY
 ;XMANAGER, 'MAIN13', MAIN13        ;schliesen des SAR-Fenster


end
