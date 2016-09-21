function str,zahl
z = string(zahl)
while strmid(z,0,1) EQ ' ' and strlen(z) GT 0 Do z = strmid(z,1,strlen(z)-1)


while strmid(z,strlen(z)-1,1) EQ '0' and strlen(z) GT 0 and strpos(z,'.',0) LT strlen(z)-1 and strpos(z,'.',0) GE 0 Do z = strmid(z,0,strlen(z)-1)
return,z 
end