%function ABSDispHistory

% plot slip
time = ABSHistory.time(1:2000);
ctrl = ABSHistory.signals(6).values(1:2000);
fnd = find(ctrl>0.0);
fctrl = fnd(1);
for i=2:20
  ctrl(:,i) = ABSHistory.signals(6).values(2000*(i-1)+1:2000*(i-1)+1+1999);
  fnd = find(ctrl(:,i)>0.1);
  fctrl(i) =fnd(1);
end

for Idx=1:6
  aux_dta = ABSHistory.signals(Idx).values(1:2000);
  for i=2:20
    aux_dta(:,i) = ABSHistory.signals(Idx).values(2000*(i-1)+1:2000*(i-1)+1+1999);
  end
  dta = aux_dta(fctrl(1)-50:fctrl(1)+199,1);
  for i=2:20
    dta(:,i) = aux_dta(fctrl(i)-50:fctrl(i)+199,i);
  end
  plot(0:0.01:2.49,dta);grid; 
  switch(Idx) 
      case 1, title('Car position')
      case 2, title('Wheel position')
      case 3, title('Car velocity')
      case 4,title('Wheel velocity')
      case 5, title('Slip')
      case 6, title('Control')
  end
  pause
end