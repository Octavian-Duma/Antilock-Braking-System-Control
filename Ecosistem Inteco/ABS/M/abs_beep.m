function Crane3D_Beep( action, frequency )

action = upper( action );


if( strcmp( action, '1S' ) == 1 )
  sn = 0.5*sin(frequency*2*pi*(1:8192)/8192);sound(sn,8192);  
  pause( 1 );

else
  disp( [ 'Unrecognized action: ' action ] );
end


