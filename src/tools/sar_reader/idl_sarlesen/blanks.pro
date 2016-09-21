function blanks,t
  if n_elements(t) EQ 0 then t = 1
  t = fix(t)
  bstr = ''
  for i = 0,t-1 do bstr = bstr + ' '
  return,bstr
end 