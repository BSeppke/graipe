pro checkdb

restore,'~uhso/idl_lib/Sar-Tool/Datenbank'
s=size(datenbank)                ; Infos über Datenbank einlesen
db_laenge=s(1)                   ; Länge eines Datensatzes
db_zaehler=s(2)                  ; Anzahl Datensätze
db_zaehler=fix(db_zaehler)       ; Long -> Int
print,'Datenbank eingelesen ('+str(db_zaehler)+' Sätze).'

goto,koord

print,'Pruefe Orientation...'

ch=1
for i=0,db_zaehler-1 do begin
    if (datenbank(7,i) gt 90) and (datenbank(7,i) lt 270) then begin
        if (datenbank(13,i) ne 1) then begin
            print,datenbank(0,i)
            ch=0
        endif
    endif else begin
        if (datenbank(13,i) ne 0) then begin
            print,datenbank(0,i) 
            ch=0
        endif
    endelse

endfor

if (ch eq 1) then print,'alles OK.'

koord:

print,'Pruefe Koordinaten'

ch=1
for i=0,db_zaehler-1 do begin

    center8=(0.1+datenbank(15,i)+datenbank(17,i)+datenbank(19,i)+datenbank(21,i)-0.1)/4
    center9=(0.1+datenbank(16,i)+datenbank(18,i)+datenbank(20,i)+datenbank(22,i)-0.1)/4
    
    if (strpos(datenbank(12,i),'N') eq -1) then begin
        dbcenter8='-'+strtrim(strmid(datenbank(12,i),0,strlen(datenbank(12,i))-2),2)
    endif else begin
        dbcenter8=strmid(datenbank(12,i),0,strlen(datenbank(12,i))-2)
    endelse
    if (strpos(datenbank(11,i),'E') eq -1) then begin
        dbcenter9='-'+strtrim(strmid(datenbank(11,i),0,strlen(datenbank(11,i))-2),2)
    endif else begin
        dbcenter9=strmid(datenbank(11,i),0,strlen(datenbank(11,i))-2)
    endelse
    
    if (center8-dbcenter8) gt .05 then begin
        print,datenbank(0,i),center8,' ',dbcenter8
        ch=0
    endif

    if (center9-dbcenter9) gt .05 then begin
        print,datenbank(0,i),center9,' ',dbcenter9
        ch=0
    endif

endfor

if (ch eq 1) then print,'alles OK.'

end

