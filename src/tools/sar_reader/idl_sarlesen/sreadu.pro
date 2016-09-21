pro sreadu,unit,var1,var2,var3,POSITION=position,NOMESSAGES=nomessages,ERROR = error

;-----------------------------------------------------------
;-  liest aus der Datei mit der fileunit unit unformatiert die
;- Variablen var1,var2,var3 von ab Byte position
;- faengt Fehlermeldungen ab. mit nomessage kann deren Ausgabe
;- unterdrueckt werden. Error ist der fehlerstatus
;-
;- V1.0 Asmus Michelsen 15.11.1996
;-----------------------------------------------------------

if n_elements(nomessages) EQ 0 then nomessages = 0
if n_elements(unit) EQ 0 then unit = 0
if unit LT 0 and n_elements(var1) NE 0 then unit = 0
error = 1
on_ioerror,err
if n_elements(position) NE 0 then begin
  position = position > 0
  point_lun,unit,position
end

if (!version.os_family eq 'Windows') then begin
    if n_elements(var1) NE 0 then begin
    readu,unit,var1

    info1=size(var1)
    info2=size(info1)
    t=info1(info2(2)-2)
    if (t eq 3) then byteorder,var1,/lswap

    if n_elements(var2) NE 0 then begin
            readu,unit,var2

            info1=size(var2)
            info2=size(info1)
            t=info1(info2(2)-2)
            if (t eq 3) then byteorder,var2,/lswap


            if n_elements(var3) NE 0 then begin
                readu,unit,var3

                info1=size(var3)
                info2=size(info1)
                t=info1(info2(2)-2)
                if (t eq 3) then byteorder,var3,/lswap

            end
        end
    end
endif else begin
   if n_elements(var1) NE 0 then begin
    readu,unit,var1
        if n_elements(var2) NE 0 then begin
            readu,unit,var2
            if n_elements(var3) NE 0 then begin
                readu,unit,var3
            end
        end
    end
endelse

error = 0
return

err: error = !error
if not nomessages then print,!err_string
return

end