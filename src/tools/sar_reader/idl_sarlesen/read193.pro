pro read193

openr,1,'PRI_16598_0405'             ; Datenfile oeffnen

hl=4180
hl=long(hl)

hlarr=intarr(hl)
readu,1,hlarr          ; entsprechend vorfahren


print,'reduziere Bild:'
res=16
ll=long(8002) & nl=long(8238)
im=intarr((ll-(ll mod res))/res, (nl-(nl mod res))/res)
line=intarr(ll,res)

file='193'
prozent=0
if (!version.arch eq 'x86') or (file eq '5') $ ; bei diesen Bildern ist Byteswap notwendig (+ Intel-Rechner)
  or (file eq '6') $
  or (file eq '193') then begin 
    print,'(-> Byteswapping)'
    for i=0,nl/res-1 do begin
        readu,1,line
        byteorder,line,/lswap
        im(*,i)=rebin(line(0:(ll-(ll mod res))-1,*), (ll-(ll mod res))/res, 1)
        if (i mod ((nl/res-1)/100) eq 0) then begin
            print,strtrim(string(prozent),2)+'% fertig'+string(13b),format='(A,$)'
            if (prozent lt 100) then prozent=prozent+1        
        endif
    endfor
endif else begin
    for i=0,nl/res-1 do begin
        readu,1,line
        im(*,i)=rebin(line(0:(ll-(ll mod res))-1,*), (ll-(ll mod res))/res, 1)
        if (i mod ((nl/res-1)/10) eq 0) and (prozent lt 100) then begin
            print,strtrim(string(prozent),2)+'% fertig'
            prozent=prozent+10
        endif
    endfor
endelse
close,1

xs=strtrim(string((ll-(ll mod res))/res),2)
ys=strtrim(string((nl-(nl mod res))/res),2)

print,'Bild ist reduziert auf '+xs+' mal '+ys+'.'

window,0,xsize=fix(xs),ysize=fix(ys)
tvscl,im

stop
end
