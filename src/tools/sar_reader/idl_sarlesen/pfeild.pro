pro PFEILd,alpd

alp = alpd*!pi/180.

; Diese Routine zeichnet einen Vektor um seinen Mittelpunkt zentriert.
; SCR scaliert die Groesse des Pfeils
; RMN ist die Schwelle fuer Pfeilspitze oder Kaestchen

if (alpd ge 0 and alpd le 45) or (alpd ge 315 and alpd le 360) then begin
	X1 = -tan(alp)
	Y1 = -1.0
	X2 = tan(alp)
	Y2 = 1.0
;	print,'Fall 1 ',x1,y1,x2,y2
endif else if (alpd ge 135 and alpd le 225) then begin 
	X1 = tan(alp)
	Y1 = 1.0
	X2 = -tan(alp)
	Y2 = -1.0
;	print,'Fall 2 ',x1,y1,x2,y2
endif else if (alpd ge 45 and alpd le 135) then begin 
	Y1 = -tan(!pi/2.-alp)
	X1 = -1.0
	Y2 = +tan(!pi/2.-alp)
	X2 = 1.0
;	print,'Fall 3 ',x1,y1,x2,y2
endif else begin
	Y1 = tan(!pi/2.-alp)
	X1 = 1.0
	Y2 = -tan(!pi/2.-alp)
	X2 = -1.0
;	print,'Fall 4 ',x1,y1,x2,y2
endelse

px	=fltarr(5)
py	=fltarr(5)

RV=SQRT((x2-x1)^2+(y2-y1)^2)   ;Laenge des Vektors
ANP=0.31416    ;Halber Oeffnungswinkel der Pfeilspitze
RVP=0.4			;Pfeilspitzenlaenge
  AL=ATAN(Y2-Y1,X2-X1)   	;Bezugswinkel fuer Pfeilspitze

  px(0)=x1			
  py(0)=y1			;Ende des Pfeils
  px(1)=x2	
  py(1)=y2			;Spitze des Pfeils
  px(2)=X2-RVP*COS(AL-ANP)	
  py(2)=Y2-RVP*SIN(AL-ANP)	;Eine Flanke des Pfeils
  pX(3)=X2-RVP*COS(AL+ANP)	
  pY(3)=Y2-RVP*SIN(AL+ANP)	;Andere Flanke des Pfeils
  pX(4)=X2			
  pY(4)=Y2			;Spitze des Pfeils

  oplot,px,py
  polyfill,[px(1),px(2),px(3),px(4)],[py(1),py(2),py(3),py(4)],col = 1.0

end








