pro loadtape,tape

dbpath='~uhso/idl_lib/Sar-Tool/Datenbank'

exabyte='E'+strtrim(string(tape),2))


;-------------------- Datenbank einlesen ---------------------------

restore,dbpath                 

s=size(datenbank)               ; Infos �ber Datenbank einlesen
db_laenge=s(1)                   ; L�nge eines Datensatzes
db_zaehler=s(2)                 ; Anzahl Datens�tze
db_zaehler=fix(db_zaehler)      ; Long -> Int
print,'Datenbank eingelesen ('+str(db_zaehler)+' S�tze).'


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
index=where(datenbank(0,*) eq tape_form)
dat=strarr(db_laenge)
dat(*)  = datenbank(*,index)

endfor

spawn,'dd of='+strtrim(string(file),2)+'.h1 if=/dev/nrst4 bs=32k'
spawn,'dd of='+strtrim(string(file),2)+'.h2 if=/dev/nrst4 bs=32k
spawn,'dd of='+strtrim(string(file),2)+'.sar if=/dev/nrst4 bs=32k
spawn,'dd of='+strtrim(string(file),2)+'.tlr if=/dev/nrst4 bs=32k

spawn,'mt -f /dev/nrst4 status'
spawn,'mt -f /dev/nrst4 offline'





print,'fertig.'
stop
end
