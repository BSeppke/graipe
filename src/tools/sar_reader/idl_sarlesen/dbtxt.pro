pro dbtxt


restore,'~/Arbeit/Sar-Tool/Datenbank'
s=size(datenbank)               ; Infos ueber Datenbank einlesen 
db_laenge=s(1)                   ; Laenge eines Datensatzes 
db_zaehler=s(2)                 ; Anzahl Datensaetze 
db_zaehler=fix(db_zaehler)      ; Long -> Int 
print,'Datenbank eingelesen ('+str(db_zaehler)+' Saetze).' 
print,db_zaehler
db=datenbank

rfile='/pf/u/u241112/Arbeit/Sar-Tool/db.txt'
print, db(0,1300)
openw,10,rfile
;printf,10,format='(a23,a21,a11,a12,a7,a8,a8,a9)','Region','Location','Ex/CD-Nr','Date','Time','Orbit','Frame','PS-CD'
;printf,10,format='(a23,a21,a11,a12,a7,a8,a8,a9)','----------------------','--------------------','----------','-----------','------','-------','-------','--------'
;for i=1118,db_zaehler-1 do begin
for i=1251,db_zaehler-1 do begin
;for i=1104,1215 do begin

  dbnamepfad=strmid(datenbank(0,i),6,11)      ; um zu ueberpruefen welcher pfad in der Datenbank gespeichert ist
                   if (dbnamepfad eq 'ifmlinux20c') then begin
                    title=strmid(datenbank(0,i),68,17) 
                    sarsat=strmid(datenbank(0,i),63,4) 
                   endif 
                   if (dbnamepfad eq 'ifmsun27b/i') then begin 
                    title=strmid(datenbank(0,i),57,17) 
                     sarsat=strmid(datenbank(0,i),52,4)
                   endif ;else begin 
                   ;title=datenbank(0,i)
                   ;sarsat=strmid(title,52,20)
                   ;endelse
;pfad wird vom namen getrennt und orbit und frame ergaenzt
;title=strmid(db(0,i),57,17)
;sarsat=strmid(db(0,i),52,4)
print, sarsat
print, title

orbittitle=strmid(title,5,6)
frametitle=strmid(title,12,5)

;Das jahr wird ins richtige Format gebracht
DD=strmid(db(4,i),0,2)
MM=strmid(db(4,i),3,2)
YYYY=strmid(db(4,i),6,4)
date=YYYY+'-'+MM+'-'+DD

;Die Zentrumskoordinaten werden mit minus versehen
cenlon=strmid(db(11,i),6,8)
vorlon=strmid(db(11,i),14,1)
;print, cenlon
print, vorlon
if (vorlon EQ 'W') then begin 
            centerlon='-'+cenlon
endif else begin
            centerlon=cenlon
            endelse 
print, centerlon            

cenlat=strmid(db(12,i),6,7)
vorlat=strmid(db(12,i),14,1)
;print, cenlat
print, vorlat
if (vorlat EQ 'S') then begin 
            centerlat='-'+cenlat
endif else begin
            centerlat=cenlat
            endelse 
print,centerlat

     ;printf,10,format='(a30,a1,a4,a1,a17,a1)',db(0,i),';',sarsat,';',title,';' ;a10,a1,a6,a7,a1,a6,a1,F6.2,a1,F6.2,a1,F6.2,a1,F6.2,a1,F6.2,a1,F6.2,a1,F6.2,a1,F6.2,a1,F6.2,a1,F6.2,a1)',db(1,i),';',date,';',db(5,i)+';',orbittitle,';', $
                                               ;  frametitle,';',db(11,i),';',db(12,i),';',db(15,i),';',db(16,i),';',db(17,i),';',db(18,i),';',db(19,i),';',db(20,i),';',db(21,i),';',db(22,i),';' 
    printf,10,format='(a4,a1,a17,a1,a10,a1,a6,a7,a1,a6,a1,a9,a1,a7,a1,F6.2,a1,F7.2,a1,F6.2,a1,F7.2,a1,F6.2,a1,F7.2,a1,F6.2,a1,F7.2,a1)',sarsat,';',title,';',date,';',db(5,i)+';',orbittitle,';', $
                                                 frametitle,';',centerlon,';',centerlat,';',db(15,i),';',db(16,i),';',db(17,i),';',db(18,i),';',db(19,i),';',db(20,i),';',db(21,i),';',db(22,i),';' 
                                   ;Name;Datum;Zeit;Orbit;Frame;Centercorlon;centercorlat;next cornercordinat
                                    ; a23,a25,        gehoert zum format macht die region und ort
                                    ; db(14,i)+';',db(1,i)=';',    
endfor


close,10
print, 'Fertig'
end
