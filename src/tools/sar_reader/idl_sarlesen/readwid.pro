; Readwid.pro wird aufgerufen von dbase.pro,
; um abhaengig vom Parameter 'mode' die Datenbankfelder
; auszulesen, Daten einzutragen oder zu loeschen  
;
; Gerald Fiedler
;


pro readwid,mode

Common match
Common dbase_widgets

case mode of

    0: begin                            ; Felder auslesen
        s1=' '
        WIDGET_CONTROL,DB_FIELD123,GET_VALUE=s1
        field(0)=s1
        WIDGET_CONTROL,DB_FIELD124,GET_VALUE=s1
        field(1)=s1
        WIDGET_CONTROL,DB_FIELD125,GET_VALUE=s1
        field(2)=s1
        WIDGET_CONTROL,DB_FIELD126,GET_VALUE=s1
        field(3)=s1
        WIDGET_CONTROL,DB_FIELD127,GET_VALUE=s1
        field(4)=s1
        WIDGET_CONTROL,DB_FIELD128,GET_VALUE=s1
        field(5)=s1
        WIDGET_CONTROL,DB_FIELD129,GET_VALUE=s1
        field(6)=s1
        WIDGET_CONTROL,DB_FIELD130,GET_VALUE=s1
        field(7)=s1
        WIDGET_CONTROL,DB_FIELD131,GET_VALUE=s1
        field(8)=s1
        WIDGET_CONTROL,DB_FIELD132,GET_VALUE=s1
        field(9)=s1
        WIDGET_CONTROL,DB_FIELD230,GET_VALUE=s1
        field(10)=s1
        WIDGET_CONTROL,DB_FIELD231,GET_VALUE=s1
        field(11)=s1
        WIDGET_CONTROL,DB_FIELD232,GET_VALUE=s1
        field(12)=s1
        WIDGET_CONTROL,DB_FIELD234,GET_VALUE=s1
        field(13)=s1
        WIDGET_CONTROL,DB_FIELD235,GET_VALUE=s1
        field(14)=s1
        WIDGET_CONTROL,DB_FIELD250,GET_VALUE=s1
        field(15)=s1
        WIDGET_CONTROL,DB_FIELD251,GET_VALUE=s1
        field(17)=s1
        WIDGET_CONTROL,DB_FIELD252,GET_VALUE=s1
        field(19)=s1
        WIDGET_CONTROL,DB_FIELD253,GET_VALUE=s1
        field(21)=s1
        WIDGET_CONTROL,DB_FIELD254,GET_VALUE=s1
        field(16)=s1
        WIDGET_CONTROL,DB_FIELD255,GET_VALUE=s1
        field(18)=s1
        WIDGET_CONTROL,DB_FIELD256,GET_VALUE=s1
        field(20)=s1
        WIDGET_CONTROL,DB_FIELD257,GET_VALUE=s1
        field(22)=s1
        WIDGET_CONTROL,DB_FIELD134,GET_VALUE=s1
        field(23)=s1
        WIDGET_CONTROL,DB_FIELD258,GET_VALUE=s1
        field(24)=s1
        WIDGET_CONTROL,DB_FIELD259,GET_VALUE=s1
        field(25)=s1
    end
    1: begin                           ; Felder füllen
        s1=field(0)
        WIDGET_CONTROL,DB_FIELD123,SET_VALUE=s1
        s1=field(1)
        WIDGET_CONTROL,DB_FIELD124,SET_VALUE=s1
        s1=field(2)
        WIDGET_CONTROL,DB_FIELD125,SET_VALUE=s1
        s1=field(3)
        WIDGET_CONTROL,DB_FIELD126,SET_VALUE=s1
        s1=field(4)
        WIDGET_CONTROL,DB_FIELD127,SET_VALUE=s1
        s1=field(5)
        WIDGET_CONTROL,DB_FIELD128,SET_VALUE=s1
        s1=field(6)
        WIDGET_CONTROL,DB_FIELD129,SET_VALUE=s1
        s1=field(7)
        WIDGET_CONTROL,DB_FIELD130,SET_VALUE=s1
        s1=field(8)
        WIDGET_CONTROL,DB_FIELD131,SET_VALUE=s1
        s1=field(9)
        WIDGET_CONTROL,DB_FIELD132,SET_VALUE=s1
        s1=field(10)
        WIDGET_CONTROL,DB_FIELD230,SET_VALUE=s1
        s1=field(11)
        WIDGET_CONTROL,DB_FIELD231,SET_VALUE=s1
        s1=field(12)
        WIDGET_CONTROL,DB_FIELD232,SET_VALUE=s1
        s1=field(13)
        WIDGET_CONTROL,DB_FIELD234,SET_VALUE=s1
        s1=field(14)
        WIDGET_CONTROL,DB_FIELD235,SET_VALUE=s1
        s1=field(15)
        WIDGET_CONTROL,DB_FIELD250,SET_VALUE=s1
        s1=field(17)
        WIDGET_CONTROL,DB_FIELD251,SET_VALUE=s1
        s1=field(19)
        WIDGET_CONTROL,DB_FIELD252,SET_VALUE=s1
        s1=field(21)
        WIDGET_CONTROL,DB_FIELD253,SET_VALUE=s1
        s1=field(16)
        WIDGET_CONTROL,DB_FIELD254,SET_VALUE=s1
        s1=field(18)
        WIDGET_CONTROL,DB_FIELD255,SET_VALUE=s1
        s1=field(20)
        WIDGET_CONTROL,DB_FIELD256,SET_VALUE=s1
        s1=field(22)
        WIDGET_CONTROL,DB_FIELD257,SET_VALUE=s1
        s1=field(23)
        WIDGET_CONTROL,DB_FIELD134,SET_VALUE=s1
        s1=field(23)
        WIDGET_CONTROL,DB_FIELD258,SET_VALUE=s1
        s1=field(24)
        WIDGET_CONTROL,DB_FIELD259,SET_VALUE=s1
    end
    2: begin                           ; Felder löschen
        WIDGET_CONTROL,DB_FIELD123,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD124,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD125,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD126,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD127,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD128,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD129,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD130,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD131,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD132,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD134,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD230,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD231,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD232,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD234,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD235,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD250,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD251,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD252,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD253,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD254,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD255,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD256,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD257,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD258,SET_VALUE='*'
        WIDGET_CONTROL,DB_FIELD259,SET_VALUE='*'
    end
endcase

end

