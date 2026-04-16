function Value = get( cabs, Property )
% Get properties of the ABS class
%   Reads data from the RTDAC4/PCI-D board

%   Copyright (c) 2004 by InTeCo, Inc.  (2K)

Property = lower( Property );

switch Property
   case 'baseaddress'
      Value = cabs.BaseAddress;
   
   case 'bitstreamversion'
      Value = inport( cabs.BaseAddress+0 );
   
   case 'encoder'   
      outportd( cabs.BaseAddress+128,0);
      Value(1) = inportd( cabs.BaseAddress+136 );
      if Value(1) >= 2^31, Value(1) = Value(1)-2^32; end;
      outportd( cabs.BaseAddress+128,1);
      Value(2) = inportd( cabs.BaseAddress+136 );
      if Value(2) >= 2^31, Value(2) = Value(2)-2^32; end;
      outportd( cabs.BaseAddress+128,2);
      Value(3) = inportd( cabs.BaseAddress+136 );
      if Value(3) >= 2^31, Value(3) = Value(3)-2^32; end;

   case 'resetencoder'   
      Reset = inportd( cabs.BaseAddress+132 );
      Value(1) = (bitand( Reset, 1 ) > 0);
      Value(2) = (bitand( Reset, 2 ) > 0);
      Value(3) = (bitand( Reset, 4 ) > 0);
      
   case 'position'      
      Value = get( cabs, 'encoder' );       
      for i=1:5
        if bitand(Value(i),hex2dec('80000')) ~= 0
            Value(i) = -hex2dec('80000') + bitand(Value(i),hex2dec('7FFFF'));
        end
      end
      Value = Value.*cabs.ScaleCoeff;

   case 'pwm'
      Mode = get(cabs,'PWMMode');
      outportd( cabs.BaseAddress+96,0);
      Value(1) = inportd( cabs.BaseAddress+108 );
      if Mode(1) == 0, Value(1)=Value(1)/255; else Value(1)=Value(1)/4095; end;
      outportd( cabs.BaseAddress+96,1);
      Value(2) = inportd( cabs.BaseAddress+108 );
      if Mode(2) == 0, Value(2)=Value(2)/255; else Value(2)=Value(2)/4095; end;
      
   case 'pwmmode'
      outportd( cabs.BaseAddress+96,0);
      Value(1) = inportd( cabs.BaseAddress+100 );
      outportd( cabs.BaseAddress+96,1);
      Value(2) = inportd( cabs.BaseAddress+100 );
      
   case 'pwmprescaler'
      outportd( cabs.BaseAddress+96,0);
      Value(1) = inportd( cabs.BaseAddress+104 );
      outportd( cabs.BaseAddress+96,1);
      Value(2) = inportd( cabs.BaseAddress+104 );
      
   case 'scalecoeff'
      Value = cabs.ScaleCoeff;
      
   case 'velocitycoeff'
      Div = inportd( cabs.BaseAddress+80 ); Div = 40e6/Div;
      Value = 60*Div/1024;
      
   case 'powercoeff'
      Value = cabs.PowerCoeff;

   case 'temperaturecoeff'
      Value = cabs.TemperatureCoeff;
      
   case 'velocity'
      Value(1) = inportd( cabs.BaseAddress+84 );
      Value(2) = inportd( cabs.BaseAddress+88 );
      Value = Value*get(cabs,'velocitycoeff');
      
   case 'velocityperiod'
      Value(1) = inportd( cabs.BaseAddress+80 );
      
   case 'resistors'
      aux = inportd( cabs.BaseAddress+144 );
      Value(1) = bitand(aux,1);
      Value(2) = (aux > 1);
      
   case 'restresholds'
      Value(1) = inportd( cabs.BaseAddress+148 );
      Value(2) = inportd( cabs.BaseAddress+152 );
      Value(3) = inportd( cabs.BaseAddress+156 );
      Value(4) = inportd( cabs.BaseAddress+160 );
      Value = Value*get(cabs,'velocitycoeff');
      
   case 'maxvelocity'
      Value(1) = inportd( cabs.BaseAddress+92 );
      Value = Value*get(cabs,'velocitycoeff');

   case 'maxvelocityflag'
      Value = inportd( cabs.BaseAddress+172 );
      Value = ( bitand( Value, 4 ) > 0 );
      
   case 'powersupply'
      Value = inportd( cabs.BaseAddress+56 );
      if Value ~= 0
        Value = cabs.PowerCoeff/Value;   % Voltage scaling
      end

   case 'minpowersupply'
      Value = inportd( cabs.BaseAddress+164 );
      Value = cabs.PowerCoeff/Value;   % Voltage scaling

   case 'powersupplyflag'
      Value = inportd( cabs.BaseAddress+172 );
      Value = ( bitand( Value, 2 ) > 0 );

   case 'temperature'
      HLen = inportd( cabs.BaseAddress+180 );
      LLen = inportd( cabs.BaseAddress+184 );
      if (LLen == 0) | (HLen == 0)
        Value = 0;
      else
        Value = 235-(400*HLen/LLen);
      end

   case 'maxtemperature'
      Value(1) = inportd( cabs.BaseAddress+168 );

   case 'temperatureflag'
      Value = inportd( cabs.BaseAddress+172 );
      Value = ( bitand( Value, 1 ) > 0 );

   case 'time'
      Value = gettime - cabs.Time;
   
   

   otherwise
      % This should not happen
      error('Unexpected property name.')
      
   end % switch
   
% Finally, assign cabs in caller's workspace
name = inputname(1);
assignin( 'caller', name, cabs )
   
return;

