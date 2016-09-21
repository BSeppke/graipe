
pro readsardim,filename,LINELENGTH=linelength,NUMBEROFLINES=numberoflines,OFFSET=offset,UNIT=funit,NOMESSAGES=nomessages,ERROR=error,PART = part,MINLINEBYTES=minlinebytes

if n_elements(filename) EQ 0 then begin
print
print,'*********************************************************************'
print,'* pro readsardim,filename,LINELENGTH=linelength,                    *'
print,'*  NUMBEROFLINES=numberoflines,OFFSET=offset,UNIT=funit,            *'
print,'*  NOMESSAGES=nomessages,ERROR=error.PART = part,                   *'
print,'*  MINLINEBYTES=minlinebytes                                        *'
print,'*                                                                   *'
print,'* soll die Bilddimensionen eines jeden sar Bildes erkennen          *'
print,'*                                                                   *'
print,'* PARAMETER:                                                        *'
print,'*     filename: datei mit Pfad in der die sar Daten gespeichert sind*'
print,'* KEYWORDS:                                                         *'
print,'*     Linelength, Numberoflines, Offset: named Variables            *'
print,'*           in diesen Variablen werden die Bilddimensionen          *'
print,'*           zurueckgegeben. linelength und offset in byte.          *'
print,'*           offset ist normalerweise gleich linelength, da die erste*'
print,'*           Zeile ein Header ist.                                   *'
print,'*     nomessages: unterdrueckt Bildschirmausgaben                   *'
print,'*     unit: fileunit, falls nicht angegeben ueber get_lun           *'
print,'*     error: Fehler status                                          *'
print,'*     part - Die Linelength wird in jeder Zeile neu ausgelesen      *'
print,'*          (Byte 9-12). falls sie sich aendert,                     *'
print,'*          nachdem schon mehr als 500 Zeilen eingelesen wurden,     *'
print,'*          wird das einlesen normalerweise abgebrochen              *'
print,'*          und nur der erste Teil zurueckgegeben. Ist part = n      *'
print,'*          dann wird der Teil zwischen dem n-1''ten und n''ten Wechsel*'
print,'*          der Linelength eingelesen,wobei Wechsel nicht mitgezaehlt*'
print,'*          werden, zwischen denen weniger als 500 zeilen liegen.    *'
print,'*     minlinebytes: Zeilen am Anfang die kuerzer als minlinebytes   *'
print,'*          sind, werden als zusaetzliche Header angesehen.          *'
print,'*          Default: 2500                                            *'
print,'*                                                                   *'
print,'* BEISPIEL:                                                         *'
print,'*   readsardim,''1099.sar'',linelength=lines,$                        *'
print,'*              numberoflines=numoflines,offset=headerlength         *'
print,'*                                                                   *'
print,'* V1.0 Asmus Michelsen 15.11.1996                                   *'
print,'*********************************************************************'
error = 1
return
end

If n_elements(minlinebytes) EQ 0 Then minlinebytes = 2500
minlinebytes = long(minlinebytes)>0
If n_elements(part) EQ 0 Then part = 0
part = long(part)>0
n_parts = long(part)
If n_elements(nomessages) EQ 0 Then nomessages = 0
nomessages = fix(nomessages)
unit = 0
If n_elements(funit) EQ 0 Then funit = -1
If funit LE 0 Then Get_Lun,unit Else unit = funit
error = 1

filename = strtrim(string(filename),2)
nf = findallfiles(filename)
nfs = size(nf)

goto,weiter                     ; weil folgender Absatz immer mit Fehler abbricht...

if nfs(0) EQ 0 or (nfs(0) EQ 1 and nfs(1) EQ 0) then begin
  if not nomessages then $
    print,'File: ' + filename + ' nicht vorhanden. Abbruch'
  if funit LE 0 then free_lun,unit
  error = 3
  return
end else if nfs(1) GT 1 then filename = nf(0)

weiter:

if not nomessages then print,'Bestimme Bild Dim. von '+filename+'...'
rec_length = long(0)
cur_rec = long(0)
rec_num = long(0)
offset = long(0)
linelength = long(0)
numberoflines = long(0)
ex_err = 0

openr,unit,filename,/swap_if_little_endian

repeat begin
    sreadu,unit,rec_length,position=cur_rec+8,error=ex_err,nomessages=nomessages
;    byteorder,rec_length,/lswap
    if rec_length LT 1 or rec_length GT 200000 or ex_err NE 0 then begin
        if not nomessages then print,'kann header Zeile nicht lesen. Abbruch'
        error = 4
        if funit LE 0 then free_lun,unit Else close,unit
        return
    endif
    cur_rec = cur_rec+rec_length
    sreadu,unit,position=cur_rec+12,nomessages=nomessages
endrep until rec_length GT minlinebytes or  EOF(unit)

linelength = rec_length
offset = cur_rec
exitflag = 0

while not eof(unit) and not exitflag do begin
  sreadu,unit,rec_length,position=cur_rec+8,nomessages=nomessages
  If rec_length LT 1 or rec_length GT 200000 or rec_length mod 2 EQ 1 then rec_length = linelength
  if rec_length NE linelength then begin
    if rec_num GT 500 then begin
      if n_parts LE 0 then begin
        if not nomessages then $
          print,'change in linelength, get part Nr. ' + str(part)
        exitflag = 1
      end else begin
        if not nomessages then $
          print,'change in linelength, skipping part Nr. '+str(part-n_parts)
        linelength = rec_length
        n_parts = n_parts - 1
      end
    end else begin
      linelength = rec_length
      n_parts = n_parts - 1
    end
  end
  cur_rec = cur_rec+rec_length
  sreadu,unit,position=cur_rec+12,nomessages=nomessages
  rec_num = rec_num + 1
end

numberoflines = rec_num

if rec_num EQ 0 then linelength = 0

if numberoflines NE 0 and linelength NE 0 then begin
  sreadu,unit,position=numberoflines*linelength+offset-2,nomessages=nomessages
  if eof(unit) then numberoflines = numberoflines-1
end

if not nomessages then begin
  print,'Linelength (Byte):',linelength
  print,'Number of Lines:  ',numberoflines
  print,'Offset (Byte):    ',offset
end
error = 0
if funit LE 0 then free_lun,unit Else Close,unit
end




pro tausch,a,b,lat_lon                          ; Austausch zweier Eck-Koordinatenpaare

        t=lat_lon(a)
        lat_lon(a)=lat_lon(b)
        lat_lon(b)=t

END


function read_paf,pafstr
  ;--- scanning for a known paf string
  if n_elements(pafstr) EQ 0 then pafstr = ''
  pafstr = strupcase(string(pafstr))
  if strpos(pafstr,'DPAF') NE -1 Then paf = 'D' ELSE $
  if strpos(pafstr,'D-PAF') NE -1 then paf = 'D' Else $
  if strpos(pafstr,'D PAF') NE -1 then paf = 'D' Else $
  if strpos(pafstr,'UKPAF') NE -1 then paf = 'U' Else $
  if strpos(pafstr,'UK-PAF') NE -1 then paf = 'U' Else $
  if strpos(pafstr,'UK PAF') NE -1 then paf = 'U' Else $
  if strpos(pafstr,'IPAF') NE -1 then paf = 'I' Else $
  if strpos(pafstr,'I-PAF') NE -1 then paf = 'I' Else $
  if strpos(pafstr,'I PAF') NE -1 then paf = 'I' Else $
  if strpos(pafstr,'EPO') NE -1 then paf = 'E' Else $
  if strpos(pafstr,'ESRIN') NE -1 then paf = 'E' Else $
  if strpos(pafstr,'E-PAF') NE -1 then paf = 'E' Else $
  if strpos(pafstr,'E PAF') NE -1 then paf = 'E' Else $
  if strpos(pafstr,'EPAF') NE -1 then paf = 'E' Else begin
    paf = ''
  end
  return,paf
end

function read_platform,platformstr
  ;--- sucht eine 1/2 eine oder 2 stellen hinter 'ERS'.
  if n_elements(platformstr) EQ 0 then platformstr = ''
  platformstr = strupcase(string(platformstr))
  ers = 0
  pos = -2
  repeat begin
    pos = strpos(platformstr,'ERS',pos+2)
    if pos NE -1 then begin
      s1 = strmid(platformstr,pos+3,1)
      s2 = strmid(platformstr,pos+4,1)
      if s1 EQ '1' then ers = 1 else $
      if s1 EQ '2' then ers = 2 else $
      if s2 EQ '1' then ers = 1 else $
      if s2 EQ '2' then ers = 2
      if ers NE 0 then pos = -1
    end
  endrep until pos EQ -1
  return,ers
end

function read_number,s_string1,label1,s_length
  ;--- liest eine Zahl die hoechstens s_length hinterm teilstring label1
  ;--- vom string s_string1 stehen darf
  s_string=strupcase(s_string1)
  label = strupcase(strtrim(label1,2))
  pos = strpos(s_string,label,0)
  if pos NE -1 then begin
    dpos = pos + strlen(label)
    while dpos-(pos+strlen(label)) LT s_length do begin
      if strisdigit(strmid(s_string,dpos,1)) NE 0 then begin
        val_str = strmid(s_string,dpos,strlen(s_string)-dpos)
        return,float(val_str)
      end
      dpos=dpos+1
    end
  end
  return,0
end

function read_date,d_string1
  ;--- wandelt datum der form 10-JAN-1996 in 19960110 um
  ;--- sucht erst Zeichenfolge fuer monat, und dann Zahlen rechts und links
  mon = 0
  d_string = strupcase(strtrim(d_string1,2))
  pos = -1
  if strpos(d_string,'JAN',0) NE -1 then begin
    pos = strpos(d_string,'JAN',0)
    mon = 1
  end else if strpos(d_string,'FEB',0) NE -1 then begin
    pos = strpos(d_string,'FEB',0)
    mon = 2
  end else if strpos(d_string,'MAR',0) NE -1 then begin
    pos = strpos(d_string,'MAR',0)
    mon = 3
  end else if strpos(d_string,'APR',0) NE -1 then begin
    pos = strpos(d_string,'APR',0)
    mon = 4
  end else if strpos(d_string,'MAY',0) NE -1 then begin
    pos = strpos(d_string,'MAY',0)
    mon = 5
  end else if strpos(d_string,'JUN',0) NE -1 then begin
    pos = strpos(d_string,'JUN',0)
    mon = 6
  end else if strpos(d_string,'JUL',0) NE -1 then begin
    pos = strpos(d_string,'JUL',0)
    mon = 7
  end else if strpos(d_string,'AUG',0) NE -1 then begin
    pos = strpos(d_string,'AUG',0)
    mon = 8
  end else if strpos(d_string,'SEP',0) NE -1 then begin
    pos = strpos(d_string,'SEP',0)
    mon = 9
  end else if strpos(d_string,'OCT',0) NE -1 then begin
    pos = strpos(d_string,'OCT',0)
    mon = 10
  end else if strpos(d_string,'NOV',0) NE -1 then begin
    pos = strpos(d_string,'NOV',0)
    mon = 11
  end else if strpos(d_string,'DEC',0) NE -1 then begin
    pos = strpos(d_string,'DEC',0)
    mon = 12
  end
  day = 0
  year = 0
  if pos NE -1 then begin
    left_side = strmid(d_string,pos-3,3)
    if strisdigit(strmid(left_side,0,1)) then day = fix(left_side) else $
    if strisdigit(strmid(left_side,1,1)) then day = fix(strmid(left_side,1,2)) else day = fix(strmid(left_side,2,1))
    right_side = strmid(d_string,pos+3,5)
    rpos = 0
    while strisdigit(strmid(right_side,rpos,1)) EQ 0 and rpos LE 4 do rpos=rpos+1
    year = long(strmid(right_side,rpos,5-rpos))
    if day LE 0 or day GT 31 then begin
      day = 0
      year = 0
      mon = 0
    end
    if year LT 100 then year = year + 1900
    if year LT 1970 or year GT 2100 then begin
      day = 0
      year = 0
      mon = 0
    end
  end
  if day EQ 0 or mon EQ 0 or year EQ 0 then dat = '' else begin
    dat = strtrim(string(year),2)
    mon_str = strtrim(string(mon),2)
    if strlen(mon_str) EQ 1 then mon_str = '0' + mon_str
    dat = dat + mon_str
    day_str =  strtrim(string(day),2)
    if strlen(day_str) EQ 1 then day_str = '0' + day_str
    dat = dat + day_str
    dat = strcompress(dat)
    print, dat
    return,dat
  end
end

function read_date_time,d_string
;wandelt datum dd-mon-yy hh:mm:ss.mmm in yyyymmddhhmmssmm um
d_str = strtrim(strupcase(d_string),2)
y = 0
mon = 0
d = 0
h = 0
min = 0
s = 0
ms = 0
len = strlen(d_str)
ypos = 0
while not strisdigit(strmid(d_str,ypos,1)) and ypos LT len do ypos = ypos + 1
y = fix(strmid(d_str,ypos,4))
if y GT 1970 and y LT 2200 then begin
  ;--- versuche ersten 4 Ziffern jahr, zweiten zwei Ziffern mon u.s.w.
  mpos = ypos+4
  while not strisdigit(strmid(d_str,mpos,1)) and mpos LT len do mpos = mpos + 1
  mon = fix(strmid(d_str,mpos,2))
  dpos = mpos+2
  while not strisdigit(strmid(d_str,dpos,1)) and dpos LT len do dpos = dpos + 1
  d = fix(strmid(d_str,dpos,2))
  hpos = dpos + 2
  while not strisdigit(strmid(d_str,hpos,1)) and hpos LT len do hpos = hpos + 1
  h = fix(strmid(d_str,hpos,2))
  minpos = hpos + 2
  while not strisdigit(strmid(d_str,minpos,1)) and minpos LT len do minpos = minpos + 1
  min = fix(strmid(d_str,minpos,2))
  spos = minpos + 2
  while not strisdigit(strmid(d_str,spos,1)) and spos LT len do spos = spos + 1
  s = fix(strmid(d_str,spos,2))
  mspos = spos + 2
  while not strisdigit(strmid(d_str,mspos,1)) and mspos LT len do mspos = mspos + 1
  ms = fix(strmid(d_str,mspos,len-mspos))
end
if y LE 1970 or y GE 2200 or mon LT 1 or mon GT 12 or d LT 1 or d GT 31 or h LT 0 or h GT 24 or min GT 60 or min LT 0 or s GT 60 or s LT 0 or ms LT 0 then begin
  ;--- versuche 09-Jul-1997 hhmmss.mmm
  dpos = 0
  ;--- suche ende der zweiten Zahl
  while not strisdigit(strmid(d_str,dpos,1)) and dpos LT len do dpos = dpos + 1
  while strisdigit(strmid(d_str,dpos,1)) and dpos LT len do dpos = dpos + 1
  while not strisdigit(strmid(d_str,dpos,1)) and dpos LT len do dpos = dpos + 1
  while strisdigit(strmid(d_str,dpos,1)) and dpos LT len do dpos = dpos + 1
  ;-- gefunden hier trennen
  dat = strmid(d_str,0,dpos)
  time = strmid(d_str,dpos,len-dpos)
  datstr = read_date(dat)
  y = fix(strmid(datstr,0,4))
  mon = fix(strmid(datstr,4,2))
  d = fix(strmid(datstr,6,2))
  hpos = 0
  tlen = strlen(time)
  while not strisdigit(strmid(time,hpos,1)) and hpos LT tlen do hpos = hpos + 1
  h = fix(strmid(time,hpos,2))
  minpos = hpos + 2
  while not strisdigit(strmid(time,minpos,1)) and minpos LT tlen do minpos=minpos+1
  min = fix(strmid(time,minpos,2))
  spos = minpos + 2
  while not strisdigit(strmid(time,spos,1)) and spos LT tlen do spos = spos + 1
  s = fix(strmid(time,spos,2))
  mspos = spos + 2
  while not strisdigit(strmid(time,mspos,1)) and mspos LT tlen do mspos=mspos+1
  ms = fix(strmid(time,mspos,len-mspos))
end
if y LE 1970 or y GE 2200 or mon LT 1 or mon GT 12 or d LT 1 or d GT 31 or h LT 0 or h GT 24 or min GT 60 or min LT 0 or s GT 60 or s LT 0 or ms LT 0 then result = '' else begin
  result = strtrim(string(y),2)
  monstr = strtrim(string(mon),2)
  while strlen(monstr) LT 2 do monstr = '0' + monstr
  dstr = strtrim(string(d),2)
  while strlen(dstr) LT 2 do dstr = '0' + dstr
  hstr = strtrim(string(h),2)
  while strlen(hstr) LT 2 do hstr = '0' + hstr
  minstr = strtrim(string(min),2)
  while strlen(minstr) LT 2 do minstr = '0' + minstr
  sstr = strtrim(string(s),2)
  while strlen(sstr) LT 2 do sstr = '0' + sstr
  msstr = strtrim(string(ms),2)
  while strlen(msstr) LT 3 do msstr = '0' + msstr
  msstr = strmid(msstr,0,3)
  result = result+monstr+dstr+hstr+minstr+sstr+msstr
end
return,result
end

function check_lat_lon,lat_lon
;--- prueft ob lat/lon gut sind
for i=0,6,2 do if lat_lon(i) GT 90.01 or lat_lon(i) LT -90.01 then return,2
for i=1,7,2 do if lat_lon(i) GT 180.01 or lat_lon(i) LT -180.01 then return,3
for i=0,6,2 do begin
  for ii = i+2,6,2 do begin
    if abs(lat_lon(i)-lat_lon(ii)) GT 3. then return,4
  end
end
for i=1,7,2 do begin
  for ii = i+2,7,2 do begin
    if abs(lat_lon(i)-lat_lon(ii)) GT 8. then return,5
  end
end
if ((lat_lon(0)-lat_lon(6)) * (lat_lon(2)-lat_lon(4))) LE 0 then return,6
if ((lat_lon(1)-lat_lon(3)) * (lat_lon(7)-lat_lon(5))) LE 0 then return,7
return,1
end



pro sarhead, data_name, vdf_name, lf_name, $
    UNIT1=funit1, UNIT2=funit2, ERROR=error, INPATH=inpath, NOMESSAGES=nomessages,$
    linelength=linelength, numberoflines=numberoflines, offset=offset, $
    orbit=orbit, frame=frame, platform=platform, procdate=procdatestr, $
    aquisition_date=aqu_dat, aquisition_time=aqu_time, paf=paf, $
    heading=heading, corner_coordinates=lat_lon, calibration_constant=calibrationconst

;--- data_name - datafilename,z.b. '256.sar'
;--- vdf_name  - volume directory file. z.b. '256.h1'
;--- lf_name   - leader file, z.b. '256.h2'
;--- linelength - linelength des sarfiles in int
;--- numberoflines
;--- offset    - headerlength des sarfiles in byte
;--- platform  - 1 oder 2 (ERS-1/2)
;--- procdate  - processingdate im format yyyymmdd
;--- aquisition_date - data aquisition date im format yyyymmdd
;--- aquisition_time - data aquisition time im format hhmmsszzz
;--- paf       - product processing facility (E, I, D, U)
;--- heading   - satellite heading degree
;--- corner_coordinates - corner coordinates als array(8) ,+North/ -South
;--- corner_coordinates(0) = 'lat first line first pixel'
;--- corner_coordinates(1) = 'lon first line first pixel'
;--- corner_coordinates(2) = 'lat first line last pixel'
;--- corner_coordinates(3) = 'lon first line last pixel'
;--- corner_coordinates(4) = 'lat last line last pixel'
;--- corner_coordinates(5) = 'lon last line last pixel'
;--- corner_coordinates(6) = 'lat last line first pixel'
;--- corner_coordinates(7) = 'lon last line first pixel'
;--- calibrationconst - calibrierungskonstante (linear)
;---
;--- rueckgabewerte sind vom typ string
;--- meistens 'unknown' falls unbekannt.


Common var_header,var_linelength, var_numberoflines, var_offset, var_orbit, $
          var_frame, var_platform, var_procdatestr, var_paf, $
          var_aqu_dat, var_aqu_time, var_heading, var_lat_lon, var_calibrationconst


;--- einlesen aller headerdaten

;--- check keywords
if n_elements(inpath) EQ 0 then inpath = ''
unknown = 'unknown'

if n_elements(nomessages) EQ 0 then nomessages = 0
error = 1
ex_error = 0
unit1 = 0

If n_elements(funit1) EQ 0 Then funit1 = -1

If funit1 LE 0 Then Get_Lun,unit1 Else unit1 = funit1
unit = 0

If n_elements(funit2) EQ 0 Then funit2 = -1

If funit2 LE 0 Then Get_Lun,unit2 Else unit2 = funit2

if not nomessages then print,'reading header: '+inpath+vdf_name+', '+inpath+lf_name
; --- weitermachen auch bei Fehler

ON_IOERROR,openerr1
; --- header oeffnen
openr,unit1,inpath+vdf_name,/swap_if_little_endian
openerr1: ON_IOERROR,openerr2
openr,unit2,inpath+lf_name,/swap_if_little_endian
openerr2: ON_IOERROR,NULL

; --- Recordsizes und positions bestimmen
vdr_size = 360
lfp_size = 360
dfp_size = 360
vdr = 0
lfp = vdr + vdr_size
dfp = lfp + lfp_size
text = dfp + dfp_size
fdr_size=long(0)
sreadu,unit2,fdr_size,pos=8,nomessages=nomessages
fdr = 0
dsr_size=bytarr(6)
sreadu,unit2,dsr_size,pos=fdr+186,nomessages=nomessages
dsr_size=long(string(dsr_size))
mpr_size=bytarr(6)
sreadu,unit2,mpr_size,pos=fdr+198,nomessages=nomessages
mpr_size=long(string(mpr_size))
ppr_size=bytarr(6)
sreadu,unit2,ppr_size,pos=fdr+210,nomessages=nomessages
ppr_size=long(string(ppr_size))

;----------
rcr_number = bytarr(6)
sreadu,unit2,rcr_number,pos=fdr+240,nomessages=nomessages
rcr_number = long(string(rcr_number))
if rcr_number NE 0 then begin
  rcr_size = bytarr(6)
  sreadu,unit2,rcr_size,pos=fdr+246,nomessages=nomessages
  rcr_size = long(string(rcr_size))
end ELSE rcr_size = 0

rpa_number = bytarr(6)
sreadu,unit2,rpa_number,pos=fdr+300,nomessages=nomessages
rpa_number = long(string(rpa_number))
if rpa_number NE 0 then begin
  rpa_size = bytarr(6)
  sreadu,unit2,rpa_size,pos=fdr+306,nomessages=nomessages
  rpa_size = long(string(rpa_size))
end ELSE rpa_size = 0
dsr = fdr + fdr_size
mpr = dsr + dsr_size
ppr = mpr + mpr_size
frd_start = ppr + ppr_size + rcr_size + rpa_size

frd_number = bytarr(6)
sreadu,unit2,frd_number,pos=fdr+420,nomessages=nomessages
frd_number = long(string(frd_number))
frd = intarr(2)
frd(0) = frd_start
frd_size_tmp = long(0)
sreadu,unit2,frd_size_tmp,pos=frd(0)+8,nomessages=nomessages
frd(1) = frd_start+frd_size_tmp
;--------------
sreadu,unit2,frd_size_tmp,pos=frd(1)+8,nomessages=nomessages


;--- Bild Dimensionen bestimmen
linelength = 0
numberoflines = 0
offset = 0
readsardim,inpath+data_name,linelength=linelength,numberoflines=numberoflines,offset=offset,nomessages=nomessages


;--- Orbit und Frame
stri = ''
orbit = long(0)
frame = long(0)
sreadu,unit1,pos=text+12,nomessages=nomessages
while not eof(unit1) do begin
  ;--- scanne text record
  readf,unit1,stri
  if orbit EQ 0 then begin
    orbit_tmp = read_number(stri,'ORBIT',4)
    if long(orbit_tmp) GT 0 and long(orbit_tmp) LT 900000 then orbit=long(orbit_tmp)
  end
  if frame EQ 0 then begin
    frame_tmp = read_number(stri,'FRAME',4)
    if long(frame_tmp) GT 0 and long(frame_tmp) LT 900000 then frame=long(frame_tmp)
  end
end
if frame EQ 0 or orbit EQ 0 then begin
  ;--- wenn nicht gut schau im dsr (scene identifier)
  orbitstr = blanks(32)
  sreadu,unit2,orbitstr,pos=dsr+36,nomessages=nomessages
  if orbit EQ 0 then begin
    orbit_tmp = read_number(orbitstr,'ORBIT',4)
    if long(orbit_tmp) GT 0 and long(orbit_tmp) LT 900000 then orbit=long(orbit_tmp)
  end
  if frame EQ 0 then begin
    frame_tmp = read_number(orbitstr,'FRAME',4)
    if long(frame_tmp) GT 0 and long(frame_tmp) LT 900000 then frame=long(frame_tmp)
  end
  if frame EQ 0 then begin
    if long(orbitstr) GT 0 and long(orbitstr) LT 900000 then frame=long(orbitstr)
  end
  if orbit EQ 0 then begin
    orbitstr = blanks(8)
    sreadu,unit2,orbitstr,pos=dsr+444,nomessages=nomessages
    if long(orbitstr) GT 0 and long(orbitstr) LT 900000 then orbit=long(orbitstr)
  end
  if orbit EQ 0 then orbit = unknown
  if frame EQ 0 then frame = unknown
end


;--- platform is 1 or 2 for ers-1/ers-2
platformstr = blanks(16)
default = unknown
sreadu,unit2,platformstr,pos=dsr + 396,nomessages=nomessages
platform = read_platform(platformstr)
if platform EQ 0 then begin
  platformstr = '1234567890123456789012345678901234567890'
  sreadu,unit1,platformstr,pos=text + 16,nomessages=nomessages
  platform = read_platform(platformstr)
  if platform EQ 0 then begin
    if nomessages EQ 0 then print,'Achtung! kann Platform nicht identifizieren'
    error = 1
    platform = default
  end
end

;--- processing date im format yyyymmdd
procdatestr = blanks(8)
default = unknown
sreadu,unit1,procdatestr,pos=vdr+112,nomessages=nomessages
y = float(strmid(procdatestr,0,4))
m = float(strmid(procdatestr,4,2))
d = float(strmid(procdatestr,6,2))
if y LT 1970 or y GT 2100 or m LT 1 or m GT 12 or d LT 1 or d GT 31 then begin
  if nomessages EQ 0 then print,'Achtung! kann Product Processing date nicht lesen'
  error = 1
  procdatestr = default
end

; aquisition date im format yyyymmdd oder 10-JAN-1995
aqudatestr = blanks(32)
default = unknown
sreadu,unit2,aqudatestr,pos=dsr+68,nomessages=nomessages
adatstr = read_date_time(aqudatestr)


if strlen(adatstr) LT 2 then begin
  aqudatestr = blanks(24)
  sreadu,unit2,aqudatestr,pos=dsr+1838,nomessages=nomessages
  adatstr = read_date_time(aqudatestr)
  if strlen(adatstr) LT 2 then begin
    aqudatestr = blanks(32)
    sreadu,unit2,aqudatestr,pos=dsr+998,nomessages=nomessages
    adatstr = read_date_time(aqudatestr)
    if strlen(adatstr) LT 2 then begin
      aqu_dat = default
    end else begin
      aqu_dat = strmid(adatstr,0,8)
    end
    aqu_time = default
  end else begin
    aqu_dat = strmid(adatstr,0,8)
    aqu_time = strmid(adatstr,8,9)
  end
end else begin
  aqu_dat = strmid(adatstr,0,8)
  aqu_time = strmid(adatstr,8,9)
end



;print,'date/time:'
;print,aqu_dat
;print,aqu_time


;--- paf could be U,D,I,E
pafstr = blanks(12)
default = unknown
sreadu,unit1,pafstr,pos=vdr + 148,nomessages=nomessages
paf = read_paf(pafstr)
if strlen(paf) EQ 0 then begin
  pafstr = blanks(200)
  sreadu,unit1,pafstr,pos=text + 16,nomessages=nomessages
  paf = read_paf(pafstr)
  if strlen(paf) EQ 0 then begin
    pafstr = '123456789012'
    sreadu,unit2,pafstr,pos=dsr + 1046,nomessages=nomessages
    paf = read_paf(pafstr)
    if strlen(paf) EQ 0 then begin
      if nomessages EQ 0 then print,'Achtung! kann Paf nicht identifizieren'
      error = 1
      paf = default
    end
  end
end

;--- corner coordinates in lat/lon
default = unknown
reml = strarr(8)
reml(0) = '81.076 [deg], +North/ -South'
reml(1) = '20.769 [deg], +East / -West'
reml(2) = '81.690 [deg], +North/ -South'
reml(3) = '16.413 [deg], +East / -West'
reml(4) = '80.988 [deg], +North/ -South'
reml(5) = '12.592 [deg], +East / -West'
reml(6) = '80.417 [deg], +North/ -South'
reml(7) = '16.863 [deg], +East / -West'
labl = strarr(8)
labl(0) = 'lat first line first pixel'
labl(1) = 'lon first line first pixel'
labl(2) = 'lat first line last pixel'
labl(3) = 'lon first line last pixel'
labl(4) = 'lat last line last pixel'
labl(5) = 'lon last line last pixel'
labl(6) = 'lat last line first pixel'
labl(7) = 'lon last line first pixel'
lat_lon_str = strarr(8)
lat_lon = fltarr(10)
for i = 0,7 do lat_lon_str(i) = '1234567891234567'
sreadu,unit2,lat_lon_str,pos=mpr+1072,nomessages=nomessages
lat_lon(0:7) = float(lat_lon_str)
for i = 0,6,2 do if lat_lon(i) GT 90 then lat_lon(i) = lat_lon(i) - 180.
for i = 1,7,2 do if lat_lon(i) GT 180 then lat_lon(i) = lat_lon(i) - 360.

;-------- Center Coordinates berechnen --------------------

lat_lon(8)=(lat_lon(0)+lat_lon(2)+lat_lon(4)+lat_lon(6))/4
lat_lon(9)=(lat_lon(1)+lat_lon(3)+lat_lon(5)+lat_lon(7))/4

if check_lat_lon(lat_lon) NE 1 then begin
  lat_lon = string(lat_lon)
  for i = 0,9 do lat_lon(i) = unknown
end


;--- satellite heading degree
default = unknown
heading_str = blanks(8)
heading = float(0)
sreadu,unit2,heading_str,pos=dsr+468,nomessages=nomessages
print,heading_str
first_h = strmid(strtrim(heading_str,2),0,1)
if strisdigit(first_h) EQ 0 and first_h NE '-' then heading=-500 else $
  heading= float(heading_str)
if heading LT 0 then heading = 360 + heading
if ((360-heading) GT 50 or heading GT 360) and abs(heading-180) GT 50 and (heading GT 50 or heading LT 0) then begin
  heading_str = blanks(16)
  sreadu,unit2,heading_str,pos=mpr+124,nomessages=nomessages
  print,heading_str
  first_h = strmid(strtrim(heading_str,2),0,1)
  if strisdigit(first_h) EQ 0 and first_h NE '-' then heading=-500 else $
    heading= float(heading_str)
  if heading LT 0 then heading = 360 + heading
  if ((360-heading) GT 50 or heading GT 360) and abs(heading-180) GT 50 and (heading GT 50 or heading LT 0) then begin
    heading_str = blanks(16)
    sreadu,unit2,heading_str,pos=mpr+220,nomessages=nomessages
    print,heading_str
    first_h = strmid(strtrim(heading_str,2),0,1)
    if strisdigit(first_h) EQ 0 and first_h NE '-' then heading=-500 else $
      heading= float(heading_str)
    if heading LT 0 then heading = 360 + heading
    if ((360-heading) GT 50 or heading GT 360) and abs(heading-180) GT 50 and (heading GT 50 or heading LT 0) then begin
      heading = default
    end
  end
end

if string(lat_lon(0)) NE string(unknown) then begin
  mhead = abs(float(heading) mod 360.)
  if float(lat_lon(0)) LE float(lat_lon(6)) then begin
    if mhead GT 90. And mhead LT 270. then heading = unknown
  end else begin
    if mhead LT 90. And mhead GT 270. then heading = unknown
  end
end
if string(heading) NE string(unknown) then heading = double(heading)



calibrationconststr = '1234567891234567'
calibrationconst = long(0)
sreadu,unit2,calibrationconststr,pos=frd(0)+662,nomessages=nomessages
calibrationconst = long(calibrationconststr)
while (calibrationconst LT 10000 And calibrationconst GT 0) DO calibrationconst=calibrationconst * 10.
while calibrationconst GE 100000000 DO calibrationconst=calibrationconst / 10.
if calibrationconst LE 0 then calibrationconst = unknown


error = 0

if funit1 LE 0 then free_lun,unit1 Else Close,unit1
if funit2 LE 0 then free_lun,unit2 Else Close,unit2


if lat_lon(0) le lat_lon(4) then begin
    tausch,0,4, lat_lon             ; Koordinatenpaare solange
    tausch,1,5, lat_lon
;   print,0,4
endif
if lat_lon(2) le lat_lon(6) then begin          ; tauschen, bis die Ecken
    tausch,2,6, lat_lon
    tausch,3,7, lat_lon
;   print,2,6
endif
if lat_lon(0) le lat_lon(6) then begin          ; richtig zugeordnet sind
    tausch,0,6, lat_lon
    tausch,1,7, lat_lon
;   print,0,6
endif
if lat_lon(2) le lat_lon(4) then begin
    tausch,2,4, lat_lon
    tausch,3,5, lat_lon
;   print,2,4
endif
if lat_lon(3) le lat_lon(1) then begin
    tausch,2,0, lat_lon
    tausch,3,1, lat_lon
;   print,3,1
endif
if lat_lon(7) le lat_lon(5) then begin
    tausch,6,4, lat_lon
    tausch,7,5, lat_lon
;   print,7,5
endif


var_linelength=linelength
var_numberoflines=numberoflines
var_offset=offset
var_orbit=orbit
var_frame=frame
var_platform=platform
var_procdatestr=procdatestr
var_paf=paf
var_aqu_dat=aqu_dat
var_aqu_time=aqu_time
var_heading=heading
var_lat_lon=lat_lon
var_calibrationconst=calibrationconst


end
