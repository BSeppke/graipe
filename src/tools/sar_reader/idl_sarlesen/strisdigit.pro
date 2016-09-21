function strisdigit,s
  ; 1 fuer ziffer 0 sonst
  if n_elements(s) EQ 0 then return,0
  s = strmid(string(s),0,1)
  if s EQ '1' or s EQ '2' or s EQ '3' or s EQ '4' or s EQ '5' or s EQ '6' or s EQ '7' or s EQ '8' or s EQ '9' or s EQ '0' then return,1 else return,0
end 
 