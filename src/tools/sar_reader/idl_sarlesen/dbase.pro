;
; Auto Save File For /home/ifmsun12d/fiedler/idl_lib/SAR-Tool/dbase.pro
;
;  Thu Apr 16 13:20:23 MET DST 1998
;
;
; EXTERNE PROZEDUREN:
;       readwid.pro     lesen der einzelnen Datenbankfelder
;       output.pro      Ausgabe des naechsten Datensatzes
;
;
; MODIFICATION HISTORY:
; 	Written by:	Gerald Fiedler
;       16.04.1998:	Initial Release (nur Darstellung der Datens�tze)
;       30.04.1998:     v1.09: �ndern und speichern der Datens�tze implementiert
;       25.05.1998:     v1.10: Auswahl m�glich
;       10.08.1999:     v1.26: Loeschen von Datensaetzen moeglich (Starten mit Keyword DEL=1)
;       03.10.1999:     v1.31: auch Eck-Koordinaten editierbar
;


PRO MAIN13_Event, Event
Common dbase_widgets, DB_FIELD123, DB_FIELD124, DB_FIELD125, DB_FIELD126, DB_FIELD127, DB_FIELD128,$
                      DB_FIELD129, DB_FIELD130, DB_FIELD131, DB_FIELD132, DB_FIELD230, DB_FIELD231,$
                      DB_FIELD232, DB_FIELD234, DB_FIELD235, DB_FIELD69, DB_FIELD43, $
                      DB_FIELD134, DB_FIELD250, DB_FIELD251, DB_FIELD252, DB_FIELD253, DB_FIELD254,$
		      DB_FIELD255, DB_FIELD256, DB_FIELD257, DB_FIELD258, DB_FIELD259, $
                      BASE133, DB_BASE99, DB_BASE41, DB_LABEL46, DB_LABEL48, DB_LABEL145, $
                      DB_BUTTON49, DB_BUTTON152, DB_BUTTON151, DB_BUTTON150, $
                      DB_BUTTON239, DB_BUTTON156, DB_BUTTON257, DB_BUTTON138, DB_SLIDER98

Common database,      datenbank, db_zaehler, db_laenge, db_pointer, db_pointer_old, db_file, db_map

Common match,         db_match, anz_match, $
                      field, fsearch, fsort, fprint, fcontrol, flaenge

  WIDGET_CONTROL,Event.Id,GET_UVALUE=Ev

  CASE Ev OF

  'BUTTON140': BEGIN                              ; neue Suche
          Widget_Control,BASE133,Sensitive=1                          ; Datenbankfelder

      if (fcontrol ne '0') then begin
          Widget_Control,DB_LABEL145,SET_VALUE='  Suchen nach:      ' ; �berschrift anpassen
          Widget_Control,DB_BASE99,Sensitive=0                        ; Scroll-Buttons
          Widget_Control,DB_FIELD69,Sensitive=0                       ; lfd Nr.
          WIDGET_CONTROL,DB_LABEL48,Sensitive=0                       ; Titel Suchergebnis
          Widget_Control,DB_BASE41,Sensitive=0                        ; Suchergebnis
          Widget_Control,DB_BUTTON156,Sensitive=1                     ; Sortierangaben
          Widget_Control,DB_BUTTON257,Sensitive=1                     ; Print-Angaben
          Widget_Control,DB_BUTTON138,Sensitive=1                     ; Suche starten
          Widget_Control,DB_BUTTON239,Sensitive=0                     ; Save Changes
          db_pointer=0

          readwid,0                 ; Maskenfelder auslesen
          case fcontrol of          ; entsprechend ablegen
              '1'  : fsort  =field
              '2'  : fprint =field
              ELSE : print
          endcase

          fcontrol=0            ; Feldkontrolle = suchen

          field=fsearch         ; Maskenfelder mit den Werten
          readwid,1             ; vom letzten Suchen f�llen
      end

      END

  'DB_BUTTON156': BEGIN                ; Sortierangaben

      if (fcontrol ne '1') then begin
          Widget_Control,DB_LABEL145,SET_VALUE='Sortierangaben:     '       ; �berschrift anpassen
          readwid,0                     ; Maskenfelder auslesen
          case fcontrol of              ; entsprechend ablegen
              '0'  : fsearch=field
              '2'  : fprint =field
              ELSE : print
          endcase

          fcontrol=1            ; Feldkontrolle = sortieren

          field=fsort           ; Maskenfelder mit den Werten
          readwid,1             ; vom letzten Sortieren f�llen
      end

      END

  'DB_BUTTON257': BEGIN                ; printangaben

      if (fcontrol ne '2') then begin
          Widget_Control,DB_LABEL145,SET_VALUE=' Print-Angaben:     '       ; �berschrift anpassen
          readwid,0                     ; Maskenfelder auslesen
          case fcontrol of              ; entsprechend ablegen
              '0'  : fsearch=field
              '1'  : fsort  =field
              ELSE : print
          endcase

          fcontrol=2            ; Feldkontrolle = printangaben

          field=fprint          ; Maskenfelder mit den Werten
          readwid,1             ; vom letzten Mal f�llen
      end

      END

  'DB_BUTTON138': BEGIN                        ; Suche starten

      if (fcontrol ne '3') then begin
          Widget_Control,DB_LABEL145,SET_VALUE='  Suchergebnis:     ' ; �berschrift anpassen
          Widget_Control,DB_BUTTON156,Sensitive=0                     ; Sortierangaben
          Widget_Control,DB_BUTTON257,Sensitive=0                     ; Print-Angaben
          Widget_Control,DB_BUTTON138,Sensitive=0                     ; Suche starten
          Widget_Control,BASE133,Sensitive=0                          ; Datenbankfelder

          readwid,0             ; Maskenfelder auslesen
          field=strtrim(field,2)
          case fcontrol of      ; entsprechend ablegen
              '0'  : fsearch=field
              '1'  : fsort  =field
              '2'  : fprint =field
              ELSE : print
          endcase

          fcontrol=3            ; Feldkontrolle = anzeigen

          ; ------------ matching records ermitteln --------------------------
          db_match=datenbank
          ind=0
          for fs=0,flaenge-1 do begin
              if (fsearch(fs) ne '*') then begin       ; wenn in einem Maskenfeld kein '*' enthalten, dann...
                  case fs of
                      0: begin                                                        ; Filename
                           if (strmid(fsearch(fs),3,1) eq 'E') then begin
                               compare=strmid(fsearch(fs),2,strlen(fsearch(fs))-2)
                           endif else begin
                               compare=strmid(fsearch(fs),3,strlen(fsearch(fs))-3)
                           endelse
                         end
                      2:    compare=fix(strmid(fsearch(fs),3,strlen(fsearch(fs))-3))  ; Orbit
                      3:    compare=fix(strmid(fsearch(fs),3,strlen(fsearch(fs))-3))  ; Frame
                      14:    compare=fix(strmid(fsearch(fs),3,strlen(fsearch(fs))-3)) ; PS_CD

                      else: compare=strmid(fsearch(fs),3,strlen(fsearch(fs))-3)
                  endcase
                  print,compare
                  db_fs=[0,1,2,3,4,5,6,7,11,12,14,8,9,23,24,15,16,17,18,19,20,21,22,25,13,10]

                  case strmid(fsearch(fs),0,2) of
                      'eq': ind=where(db_match(db_fs(fs),*) eq compare)

                      else: print,'Syntax Error :'+fsearch(fs)
                  endcase
                  print,'matching '+fsearch(fs)+' :',ind+1
                  if (ind(0) ne -1) then db_match=db_match(*,ind) else print,'No matching Records :'+fsearch(fs)
              endif

          endfor

          ; ----------------- Records sortieren ------------------------------

          fs=sort(fsort)                               ; Sortierreihenfolge festlegen
          t=-1
          repeat t=t+1 until (fsort(fs(t)) ne '*') or (t eq 23)
          if fsort(fs(flaenge-1)) ne '*' then begin           ; prueft, ob ueberhaupt sortiert werden soll
              fs2=lonarr(flaenge-t)
              fs2=fs(t:flaenge-2)
              for s=flaenge-2-t,0,-1 do begin
                  db_match=db_match(*,sort(db_match(fs2(s),*)))
              endfor

          endif

          ; ------------ markierte Record-Felder ausgeben --------------------



          ;-------------------------------------------------------------------
          Widget_Control,DB_BASE41,Sensitive=1 ; Suchergebnis
          WIDGET_CONTROL,DB_LABEL48,Sensitive=1 ; Titel Suchergebnis

          if (ind(0) ne -1) then begin
              Widget_Control,DB_FIELD69,Sensitive=1 ; lfd Nr.
              WIDGET_CONTROL,DB_LABEL46,Sensitive=1
              WIDGET_CONTROL,DB_BUTTON49,Sensitive=1
              WIDGET_CONTROL,DB_BUTTON152,Sensitive=1
              WIDGET_CONTROL,DB_BUTTON151,Sensitive=1

              s=size(db_match)  ; Infos �ber Datenbank einlesen
              if (s(0) eq 2) then anz_match=s(2) else anz_match=1 ; Anzahl Datens�tze

              WIDGET_CONTROL,DB_FIELD43,SET_VALUE=anz_match
;              if (anz_match ne db_zaehler) then WIDGET_CONTROL,DB_slider98,Sensitive=0 else 
              WIDGET_CONTROL,DB_slider98,Sensitive=1
              db_pointer=0

          endif else begin
              WIDGET_CONTROL,DB_FIELD43,SET_VALUE=0
              WIDGET_CONTROL,DB_LABEL46,Sensitive=0
              WIDGET_CONTROL,DB_BUTTON49,Sensitive=0
              WIDGET_CONTROL,DB_BUTTON152,Sensitive=0
              WIDGET_CONTROL,DB_BUTTON151,Sensitive=0

          endelse

      end

      END

  'DB_BUTTON49': BEGIN                         ; 'Event for Druckerausgabe'


      END

  'DB_BUTTON152': BEGIN                         ; Ausgabe auf Bildschirm
      print,db_match

      END

  'DB_BUTTON151': BEGIN                         ; Ausgabe ueber die Maske
      Widget_Control,BASE133,Sensitive=1        ; Datenbankfelder
      Widget_Control,DB_BASE99,Sensitive=1
      output
      END

  'DB_BUTTON150': BEGIN                         ; Map an-/ausschalten
      db_map = 1-db_map
      if (db_map eq 1) then begin
          Widget_Control,DB_BUTTON150,SET_VALUE='Map'       ; Button-text anpassen
      endif else begin
          Widget_Control,DB_BUTTON150,SET_VALUE='no Map'    ; Button-text anpassen
      endelse
      END

  'DB_FIELD69': begin
      Widget_Control,DB_FIELD69,GET_VALUE=db_pointer
      if (db_pointer le db_zaehler) then begin
          db_pointer=db_pointer-1
          output
      endif
      end


  'DB_SLIDER98':begin
		Widget_Control,DB_SLIDER98,GET_VALUE=tmp_pointer
                if (tmp_pointer le anz_match-1) then db_pointer=tmp_pointer-1 else db_pointer=anz_match-1 
		output
	end

  'BUTTON100': BEGIN
      if (db_Pointer ne 0) then begin
          db_Pointer=0
          output
      endif
      END


  'BUTTON118': BEGIN
      if (db_Pointer ge 10) then begin
          db_Pointer=db_Pointer-10
          output
      endif else begin
          db_Pointer=0
          output
      endelse
      END

  'BUTTON101': BEGIN
      if (db_Pointer ge 1) then begin
          db_Pointer=db_Pointer-1
          output
      endif
      END

  'BUTTON102': BEGIN
      if (db_Pointer lt anz_match-1) then begin
          db_Pointer=db_Pointer+1
          output
      endif
      END

  'BUTTON120': BEGIN
      if (db_Pointer lt anz_match-10) then begin
          db_Pointer=db_Pointer+10
          output
      endif else begin
          db_Pointer=anz_match-1
          output
      endelse

      END

  'BUTTON103': BEGIN
      if (db_Pointer ne anz_match-1) then begin
          db_Pointer=anz_match-1
          output
      endif
      END

  'DB_BUTTON239' : BEGIN                                       ; Save Changes
      if (db_zaehler eq anz_match) then begin
          datenbank=db_match
          save,datenbank,filename=db_file                      ; neue Datenbank auf Platte sichern
          Widget_Control,DB_BUTTON239,Sensitive=0              ; Save Changes
          print,'�nderungen gespeichert'
      endif
      END

  'BUTTON240' : BEGIN
      WIDGET_CONTROL, event.top, /DESTROY
      exit                                                     ;IDL beenden
      END

  'DB_BUTTON241' : begin

      print,'loesche Datensatz '+strtrim(string(db_pointer+1),2)

      tmparr=strarr(db_laenge,db_zaehler-1)                ; Neues Array hat 1 Datensatz weniger
      if (db_pointer+1 eq 1) then  begin
          tmparr=datenbank(*,1:db_zaehler-1)
      endif
      if (db_pointer+1 eq db_zaehler) then begin
          tmparr=datenbank(*,0:db_zaehler-2)
      endif
      if (db_pointer+1 ne 1) and (db_pointer+1 ne db_zaehler) then begin
          tmparr(*,0:db_pointer-1)=datenbank(*,0:db_pointer-1)
          tmparr(*,db_pointer:db_zaehler-2)=datenbank(*,db_pointer+1:db_zaehler-1)
      endif

      datenbank=tmparr                                     ; Datenbank ist geaendert
      tmparr=0
      db_zaehler=db_zaehler-1


      db_pointer=db_pointer-1                      ; mit neuer Datenbank in Editier-Modus
      db_pointer_old=db_pointer_old-1

      db_match=datenbank
      output
      anz_match=anz_match-1
      WIDGET_CONTROL,DB_FIELD43,SET_VALUE=anz_match

;      if (db_pointer ne db_zaehler-1) then db_match(*,db_pointer+1)=datenbank(*,db_pointer+1) $
;                                      else db_match(*,db_zaehler-1)=datenbank(*,db_zaehler-1)

      WIDGET_CONTROL,DB_BUTTON239,Sensitive=0

  end

  'DB_BUTTON242' : begin
      print,'fuege leeren Datensatz '+strtrim(string(db_pointer+2),2)+' hinzu'
      tmparr=strarr(db_laenge,db_zaehler+1)
      if (db_pointer eq db_zaehler-1) then  begin
          tmparr(*,0:db_zaehler-1)=datenbank
      endif else begin
          tmparr(*,0:db_pointer)=datenbank(*,0:db_pointer)
          tmparr(*,db_pointer+2:db_zaehler)=datenbank(*,db_pointer+1:db_zaehler-1)
      endelse
      
      datenbank=tmparr              ; Datenbank ist geaendert
      tmparr=0
      db_zaehler=db_zaehler+1

      db_match=datenbank            ; mit neuer Datenbank in Editier-Modus
      db_pointer_old=db_pointer_old+1
      db_pointer=db_pointer+1
      output      
      anz_match=anz_match+1    
      WIDGET_CONTROL,DB_FIELD43,SET_VALUE=anz_match
  end
  ENDCASE
END



pro dbase, GROUP=Group, DEL=del
Common dbase_widgets
Common database
Common match

flaenge=26
field   =strarr(flaenge) & field(*)='*'
fsearch =strarr(flaenge) & fsearch(*)='*'
fsort   =strarr(flaenge) & fsort(*)='*'
fprint  =strarr(flaenge) & fprint(*)='*'
fcontrol='0'




if (!version.os_family eq 'Windows') then begin
;   db_file='~fiedler/sar/Datenbank'
    db_file='c:\RSI\Datenbank'
endif else begin
    db_file='/pf/u/u241112/Arbeit/Sar-Tool/Datenbank'
endelse

restore,db_file                  ; Datenbank einlesen
s=size(datenbank)                ; Infos �ber Datenbank einlesen
db_laenge=s(1)                   ; L�nge eines Datensatzes
db_zaehler=s(2)                  ; Anzahl Datens�tze
db_zaehler=fix(db_zaehler)       ; Long -> Int
print,'Datenbank eingelesen ('+str(db_zaehler)+' S�tze).'
db_pointer=0
db_pointer_old=0
db_map=0

  IF N_ELEMENTS(Group) EQ 0 THEN GROUP=0
  IF N_ELEMENTS(del) EQ 0 THEN GROUP=0
  junk   = { CW_PDMENU_S, flags:0, name:'' }


  MAIN13 = WIDGET_BASE(GROUP_LEADER=Group, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Datenbank', $
      UVALUE='MAIN13')

  BASE2 = WIDGET_BASE(MAIN13, $
      COLUMN=1, $
      SPACE=20, $
      MAP=1, $
      TITLE='Suchergebnis', $
      UVALUE='BASE2')

  DB_LABEL145 = WIDGET_LABEL( BASE2, $
      FONT='7x13bold', $
      UVALUE='LABEL145', $
      VALUE='  Suchen nach:      ')

  BASE122 = WIDGET_BASE(BASE2, $
      ROW=1, $
      MAP=1, $
      TITLE='Suchen nach + Buttons', $
      UVALUE='BASE122')

  BASE133 = WIDGET_BASE(BASE122, $
      COLUMN=1, $
      FRAME=1, $
      MAP=1, $
      TITLE='suchen nach', $
      UVALUE='BASE133')

  BASE161 = WIDGET_BASE(BASE133, $
      ROW=1, $
      MAP=1, $
      TITLE='file, orbit, frame', $
      UVALUE='BASE161')

  BASE162 = WIDGET_BASE(BASE133, $
      ROW=1, $
      MAP=1, $
      TITLE='date,time, center-koord.', $
      UVALUE='BASE162')

  BASE163 = WIDGET_BASE(BASE133, $
      ROW=1, $
      MAP=1, $
      TITLE='region, nn, nl, version', $
      UVALUE='BASE163')
 
  BASE164 = WIDGET_BASE(BASE133, $
      ROW=2, $
      MAP=1, $
      TITLE='Corner Coordinates', $
      UVALUE='BASE164')



  DB_FIELD123 = CW_FIELD( BASE161,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Filename', $
      UVALUE='FIELD123', $
      XSIZE=9)

  DB_FIELD124 = CW_FIELD( BASE161,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Title', $
      UVALUE='FIELD124', $
      XSIZE=25)


  DB_FIELD230 = CW_FIELD( BASE161,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Region', $
      UVALUE='FIELD230', $
      XSIZE=25)

  DB_FIELD125 = CW_FIELD( BASE162,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Orbit', $
      UVALUE='FIELD125', $
      XSIZE=6)

  DB_FIELD126 = CW_FIELD( BASE162,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Frame', $
      UVALUE='FIELD126', $
      XSIZE=6)

  DB_FIELD127 = CW_FIELD( BASE162,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Date', $
      UVALUE='FIELD127', $
      XSIZE=10)

  DB_FIELD128 = CW_FIELD( BASE162,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Time', $
      UVALUE='FIELD128', $
      XSIZE=6)

  DB_FIELD129 = CW_FIELD( BASE162,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Platform', $
      UVALUE='FIELD129', $
      XSIZE=6)

  DB_FIELD130 = CW_FIELD( BASE162,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Heading', $
      UVALUE='FIELD130', $
      XSIZE=6)

  DB_FIELD258 = CW_FIELD( BASE162,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Orientation', $
      UVALUE='FIELD258', $
      XSIZE=9)

  DB_FIELD134 = CW_FIELD( BASE163,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Produkt', $
      UVALUE='FIELD134', $
      XSIZE=6)


  DB_FIELD231 = CW_FIELD( BASE163,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Linelength', $
      UVALUE='FIELD231', $
      XSIZE=10)

  DB_FIELD232 = CW_FIELD( BASE163,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='No. of lines', $
      UVALUE='FIELD232', $
      XSIZE=10)

  DB_FIELD259 = CW_FIELD( BASE163,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Headerlength', $
      UVALUE='FIELD259', $
      XSIZE=10)


  DB_FIELD234 = CW_FIELD( BASE163,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Version', $
      UVALUE='FIELD234', $
      XSIZE=5)

  DB_FIELD235 = CW_FIELD( BASE163,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='PS_CD', $
      UVALUE='FIELD235', $
      XSIZE=5)

  DB_LABEL248 = WIDGET_LABEL( BASE164, $
      UVALUE='DB_LABEL248', $
      VALUE='Latitude :', $
      XSIZE=80)

  DB_FIELD250 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='NW', $
      UVALUE='FIELD250', $
      XSIZE=8)

  DB_FIELD251 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='NE', $
      UVALUE='FIELD251', $
      XSIZE=8)

  DB_FIELD252 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='SW', $
      UVALUE='FIELD252', $
      XSIZE=8)

  DB_FIELD253 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='SE', $
      UVALUE='FIELD253', $
      XSIZE=8)

  DB_FIELD132 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Center', $
      UVALUE='FIELD132', $
      XSIZE=10)

  DB_LABEL249 = WIDGET_LABEL( BASE164, $
      UVALUE='DB_LABEL249', $
      VALUE='Longitude:', $
      XSIZE=80)

  DB_FIELD254 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='NW', $
      UVALUE='FIELD254', $
      XSIZE=8)

  DB_FIELD255 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='NE', $
      UVALUE='FIELD255', $
      XSIZE=8)

  DB_FIELD256 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='SW', $
      UVALUE='FIELD256', $
      XSIZE=8)

  DB_FIELD257 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='SE', $
      UVALUE='FIELD257', $
      XSIZE=8)



 DB_FIELD131 = CW_FIELD( BASE164,VALUE='', $
      COLUMN=1, $
      STRING=1, $
      TITLE='Center', $
      UVALUE='FIELD131', $
      XSIZE=10)




  BASE136 = WIDGET_BASE(BASE122, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Buttons', $
      UVALUE='BASE136')

  BUTTON140 = WIDGET_BUTTON( BASE136, $
      FONT='7x13bold', $
      UVALUE='BUTTON140', $
      VALUE='Neue Suche')

  DB_BUTTON156 = WIDGET_BUTTON( BASE136, $
      UVALUE='DB_BUTTON156', $
      VALUE='Sortierangaben')

  DB_BUTTON257 = WIDGET_BUTTON( BASE136, $
      UVALUE='DB_BUTTON257', $
      VALUE='Printangaben')

  DB_BUTTON138 = WIDGET_BUTTON( BASE136, $
      FONT='9x15bold', $
      UVALUE='DB_BUTTON138', $
      VALUE='Suchen !', $
      YSIZE=40)


  DB_LABEL48 = WIDGET_LABEL( BASE2, $
      FONT='7x13bold', $
      UVALUE='LABEL48', $
      VALUE='Suchergebnis:')

  DB_BASE41 = WIDGET_BASE(BASE2, $
      ROW=1, $
      SPACE=10, $
      FRAME=1, $
      MAP=1, $
      TITLE='Ergebnisausgabe', $
      UVALUE='BASE41')

  DB_FIELD43 = CW_FIELD( DB_BASE41,VALUE='', $
      ROW=1, $
      INTEGER=1, $
      TITLE='Matching Files', $
      UVALUE='DB_FIELD43', $
      XSIZE=6)

  DB_LABEL46 = WIDGET_LABEL( DB_BASE41, $
      UVALUE='DB_LABEL46', $
      VALUE='Ausgabe auf:', $
      XSIZE=150)

  DB_BUTTON49 = WIDGET_BUTTON( DB_BASE41, $
      UVALUE='DB_BUTTON49', $
      VALUE='Printer', $
      XSIZE=60, $
      YSIZE=1)

  DB_BUTTON152 = WIDGET_BUTTON( DB_BASE41, $
      UVALUE='DB_BUTTON152', $
      VALUE='Screen', $
      XSIZE=60, $
      YSIZE=1)

  DB_BUTTON151 = WIDGET_BUTTON( DB_BASE41, $
      UVALUE='DB_BUTTON151', $
      VALUE='Maske', $
      XSIZE=60, $
      YSIZE=1)


  DB_BUTTON150 = WIDGET_BUTTON( DB_BASE41, $
      UVALUE='DB_BUTTON150', $
      VALUE='no Map', $
      YSIZE=1)


  BASE68 = WIDGET_BASE(BASE2, $
      ROW=1, $
      SPACE=10, $
      MAP=1, $
      TITLE='Edit Suchergebnis', $
      UVALUE='BASE68')

  DB_FIELD69 = CW_FIELD( BASE68,VALUE='', $
      COLUMN=1, $
      INTEGER=1, $
      RETURN_EVENTS=1, $
      TITLE='lfd. Nr.:', $
      UVALUE='DB_FIELD69', $
      XSIZE=6)

  DB_BASE99 = WIDGET_BASE(BASE68, $
      COLUMN=1, $
      FRAME=1, $
      MAP=1, $
      TITLE='Titel + Schalter', $
      UVALUE='BASE99')

  LABEL111 = WIDGET_LABEL( DB_BASE99, $
      UVALUE='LABEL111', $
      VALUE='Scroll Buttons')

  BASE104 = WIDGET_BASE(DB_BASE99, $
      ROW=1, $
      MAP=1, $
      TITLE='Schalter zum scrollen', $
      UVALUE='BASE104')

  DB_SLIDER98 = WIDGET_SLIDER( DB_BASE99, $
      MAXIMUM=db_zaehler, $
      MINIMUM=1, $
      UVALUE='DB_SLIDER98', $
      VALUE=1)


  BUTTON100 = WIDGET_BUTTON( BASE104, $
      UVALUE='BUTTON100', $
      VALUE='BEGIN', $
      XSIZE=50)

  BUTTON118 = WIDGET_BUTTON( BASE104, $
      UVALUE='BUTTON118', $
      VALUE='- 10', $
      XSIZE=50)

  BUTTON101 = WIDGET_BUTTON( BASE104, $
      UVALUE='BUTTON101', $
      VALUE='- 1', $
      XSIZE=50)

  BUTTON102 = WIDGET_BUTTON( BASE104, $
      UVALUE='BUTTON102', $
      VALUE='+ 1', $
      XSIZE=50)

  BUTTON120 = WIDGET_BUTTON( BASE104, $
      UVALUE='BUTTON120', $
      VALUE='+ 10', $
      XSIZE=50)

  BUTTON103 = WIDGET_BUTTON( BASE104, $
      UVALUE='BUTTON103', $
      VALUE='END', $
      XSIZE=50)


  BASE238 = WIDGET_BASE(BASE68, $
      COLUMN=1, $
      MAP=1, $
      TITLE='Schalter save/exit', $
      UVALUE='BASE238')

  DB_BUTTON242 = WIDGET_BUTTON( BASE238, $
      FONT='7x13bold', $
      UVALUE='DB_BUTTON242', $
      VALUE='Add')

  DB_BUTTON241 = WIDGET_BUTTON( BASE238, $
      FONT='7x13bold', $
      UVALUE='DB_BUTTON241', $
      VALUE='Delete')

  DB_BUTTON239 = WIDGET_BUTTON( BASE238, $
      FONT='7x13bold', $
      UVALUE='DB_BUTTON239', $
      VALUE='Save changes', $
      XSIZE=100)

  BUTTON240 = WIDGET_BUTTON( BASE238, $
      FONT='7x13bold', $
      UVALUE='BUTTON240', $
      VALUE='EXIT', $
      XSIZE=100)



  readwid,2                                     ; Maske mit '*' f�llen

  Widget_Control,DB_BASE99,Sensitive=0          ; Scroll-Buttons
  Widget_Control,DB_FIELD69,Sensitive=0         ; lfd Nr.
  WIDGET_CONTROL,DB_LABEL48,Sensitive=0         ; Titel Suchergebnis
  Widget_Control,DB_BASE41,Sensitive=0          ; Suchergebnis
  Widget_Control,DB_BUTTON239,Sensitive=0       ; Save Changes
  Widget_Control,BASE133,Sensitive=0            ; Datenbankfelder



  if (N_ELEMENTS(del) EQ 0) then begin 
      Widget_Control,DB_BUTTON241,Sensitive=0 ; Delete
      Widget_Control,DB_BUTTON242,Sensitive=0 ; Add
  endif

  WIDGET_CONTROL, MAIN13, /REALIZE

  XMANAGER, 'MAIN13', MAIN13
END
