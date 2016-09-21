; Output.pro wird aufgerufen von dbase.pro,
; um den aktuellen Datensatz über die Maske auszugeben
; und zu prüfen, ob der vorherige Datensatz verändert wurde (=> 'Save changes' aktiv)
;
; Gerald Fiedler

pro output

Common database
Common dbase_Widgets
Common match

flaenge=26
if (db_zaehler eq anz_match) then begin 
    if (db_pointer ne 0) then begin                     ; vorherigen Datensatz einlesen
        readwid,0
        check=strarr(flaenge)
        check(0:5)=db_match(0:5,db_pointer_old)
        check(6)='ERS-'+strtrim(db_match(6,db_pointer_old),2)
        check(7)=db_match(7,db_pointer_old)
        check(8)=db_match(11,db_pointer_old)
        check(9)=db_match(12,db_pointer_old)
        check(10)=db_match(14,db_pointer_old)
        check(11)=db_match(8,db_pointer_old)/2
        check(12)=db_match(9,db_pointer_old)
        check(13)=db_match(23,db_pointer_old)
        check(14)=db_match(24,db_pointer_old)
        check(15:22)=db_match(15:22,db_pointer_old)
        check(23)=db_match(25,db_pointer_old)
        check(24)=db_match(13,db_pointer_old)
        check(25)=db_match(10,db_pointer_old)


        check=strtrim(check,1)

        
        for j=0,flaenge-1 do begin                                ; Felder durchlaufen,

            if (field(j) ne check(j)) then begin           ; ob eins geändert wurde

                Widget_Control,DB_BUTTON239,Sensitive=1    ; Save Changes

                case j of       ; kontrolliertes zurückschreiben in die Datenbank
		    0: begin
		       if (strmid(field(j),0,2) eq 'CD') then begin
			   db_match(0,db_pointer_old)=field(j)	
		       endif else begin
			   db_match(0,db_pointer_old)=' '+field(j)	
		       endelse
		       end	
                    6: db_match(6,db_pointer_old)=strmid(field(j),4,1)
                    8: db_match(11,db_pointer_old)=field(j)
                    9: db_match(12,db_pointer_old)=field(j)
                    10: db_match(14,db_pointer_old)=field(j)
                    11: db_match(8,db_pointer_old)=field(j)*2
                    12: db_match(9,db_pointer_old)=field(j)
                    13: db_match(23,db_pointer_old)=field(j)
                    14: db_match(24,db_pointer_old)=field(j)
                    23: db_match(25,db_pointer_old)=field(j)
                    24: db_match(13,db_pointer_old)=field(j)
                    25: db_match(10,db_pointer_old)=field(j)
                    else:  db_match(j,db_pointer_old)=field(j)
                endcase
            endif
        endfor
    endif
endif

i=db_pointer                                                   ; nächsten Datensatz ausgeben
	WIDGET_CONTROL,DB_FIELD123,SET_VALUE=db_match(0,i)
	WIDGET_CONTROL,DB_FIELD124,SET_VALUE=db_match(1,i)
 	WIDGET_CONTROL,DB_FIELD125,SET_VALUE=db_match(2,i)
	WIDGET_CONTROL,DB_FIELD126,SET_VALUE=db_match(3,i)
 	WIDGET_CONTROL,DB_FIELD127,SET_VALUE=db_match(4,i)
	WIDGET_CONTROL,DB_FIELD128,SET_VALUE=db_match(5,i)
 	WIDGET_CONTROL,DB_FIELD129,SET_VALUE='ERS-'+strtrim(db_match(6,i),2)
	WIDGET_CONTROL,DB_FIELD130,SET_VALUE=db_match(7,i)
	WIDGET_CONTROL,DB_FIELD131,SET_VALUE=db_match(11,i)
	WIDGET_CONTROL,DB_FIELD132,SET_VALUE=db_match(12,i)
        WIDGET_CONTROL,DB_FIELD134,SET_VALUE=db_match(25,i)
	WIDGET_CONTROL,DB_FIELD230,SET_VALUE=db_match(14,i)
	WIDGET_CONTROL,DB_FIELD231,SET_VALUE=db_match(8,i)/2
 	WIDGET_CONTROL,DB_FIELD232,SET_VALUE=db_match(9,i)
	WIDGET_CONTROL,DB_FIELD234,SET_VALUE=db_match(23,i)
	WIDGET_CONTROL,DB_FIELD235,SET_VALUE=db_match(24,i)
        WIDGET_CONTROL,DB_FIELD250,SET_VALUE=db_match(15,i)
	WIDGET_CONTROL,DB_FIELD254,SET_VALUE=db_match(16,i)
 	WIDGET_CONTROL,DB_FIELD251,SET_VALUE=db_match(17,i)
	WIDGET_CONTROL,DB_FIELD255,SET_VALUE=db_match(18,i)
	WIDGET_CONTROL,DB_FIELD252,SET_VALUE=db_match(19,i)
        WIDGET_CONTROL,DB_FIELD256,SET_VALUE=db_match(20,i)
	WIDGET_CONTROL,DB_FIELD253,SET_VALUE=db_match(21,i)
 	WIDGET_CONTROL,DB_FIELD257,SET_VALUE=db_match(22,i)
 	WIDGET_CONTROL,DB_FIELD258,SET_VALUE=db_match(13,i)
 	WIDGET_CONTROL,DB_FIELD259,SET_VALUE=db_match(10,i)
	WIDGET_CONTROL,DB_FIELD69,SET_VALUE=db_pointer+1
	WIDGET_CONTROL,DB_SLIDER98,SET_VALUE=db_pointer+1


db_pointer_old=db_pointer                                      ; Zeiger_alt


if (db_map eq 1) then begin
    WINDOW,0,XSIZE=400,YSIZE=400,TITLE='Map - '+db_match(0,i)
    var_lat_lon=strarr(10)
    var_lat_lon(0)=db_match(15,i)
    var_lat_lon(1)=db_match(16,i)
    var_lat_lon(2)=db_match(17,i)
    var_lat_lon(3)=db_match(18,i)
    var_lat_lon(4)=db_match(19,i)
    var_lat_lon(5)=db_match(20,i)
    var_lat_lon(6)=db_match(21,i)
    var_lat_lon(7)=db_match(22,i)
    center_lat=db_match(12,i)
    center_lon=db_match(11,i)
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

    map_set,var_lat_lon(8),var_lat_lon(9),/gnom, $
      limit=[var_lat_lon(8)-8,var_lat_lon(9)-8,var_lat_lon(8)+8,var_lat_lon(9)+8], $
      /continent, /hires, pos=[0.,0.,1,1], mlinestyle=0, mlinethick=1, $
      lonlab=var_lat_lon(8)-7, latlab=var_lat_lon(9)-7,/grid,/label

    longim=[var_lat_lon(1), var_lat_lon(3), var_lat_lon(7), var_lat_lon(5), var_lat_lon(1)]
    latim =[var_lat_lon(0), var_lat_lon(2), var_lat_lon(6), var_lat_lon(4), var_lat_lon(0)]

    plots, longim, latim, linestyle=0, thick=5
    map_continents, /coast, /hires, /USA, /countries

endif

end
