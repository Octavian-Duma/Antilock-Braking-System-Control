function beep( action, frequency )

action = upper( action );

if( strcmp( action, 'STOP' ) == 1 )
  port = inportb( 97 );        %  97 = 0x61
  port = bitand( port, 253 );  % 253 = 0xFD
  outportb( 97, port );

elseif( strcmp( action, 'START' ) == 1 )
  if( frequency < 20.0 )  frequency = 20.0;  end;
  div = floor( 1000.0 * 1193.0 / frequency );
  lo = rem( div, 256 );   
  hi = ( div - lo ) / 256;
  outportb( 67, 190 );  %  67 = 0x43, mode = 190 = 0xBE
  outportb( 66, lo );   %  66 = 0x42
  outportb( 66, hi );
  port = inportb( 97 ); %  97 = 0x61
  port = bitor( port, 3 );
  outportb( 97, port );

else
  disp( [ 'Unrecognized action: ' action ] );
end


