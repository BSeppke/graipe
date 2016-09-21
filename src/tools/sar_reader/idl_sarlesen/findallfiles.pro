function findallfiles,spez,COUNT = count
;  da findfile bei ~ im path nicht funktioniert
;  erst in verzeichniss wechseln, dann findfile.
;  findet nur dateien keine directorys

lsresult=''

if (!version.os_family eq 'Windows') then begin
	spawn,'dir '+spez,lsresult
endif else begin
	spawn,'ls '+spez,lsresult
endelse

print,lsresult
return,''

if n_elements(spez) EQ 0 then spez = ''
;pos = 0
;opos = -1
;while pos NE -1 do begin
;  pos = strpos(spez,'/',opos+1)
;  if pos NE -1 then opos = pos
;end
path=''
name=''
ext=''
split_filename,spez,path=path,name=name,extension=ext
ename=name+ext

if strlen(path) GT 0 then begin
  pushd,path
  result = findfile(name,count=count)
  popd
end else result = findfile(name,count=count)
return,result
end
