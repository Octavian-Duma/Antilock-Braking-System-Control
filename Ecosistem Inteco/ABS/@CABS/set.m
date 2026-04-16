function Out = set(cabs,varargin)
% Set properties of the ABS class
%   Writes data from the RT-DAC4/PCI-D / ABS board

%   Copyright (c) 2004 by InTeCo, Inc.  (2K)

ni = nargin;


% Now left with SET(cabs,'Prop1',Value1)
name = inputname(1);
for i=1:2:ni-1,
   Property = lower( varargin{i} );
   if ni > i+1
      Value = varargin{i+1};
   end;

   switch Property
        
      case 'resetencoder'   
         Reset = 0;
         if Value(1) > 0, Reset = Reset + 1; end;
         if Value(2) > 0, Reset = Reset + 2; end;
         if Value(3) > 0, Reset = Reset + 4; end;
         outportd( cabs.BaseAddress+132, Reset );
         Value = inportd( cabs.BaseAddress+132 );
      
     case 'pwm'
        cabs.PWM(1) = SetPWM( cabs, 0, Value(1) );
        cabs.PWM(2) = SetPWM( cabs, 1, Value(2) );
        
     case 'pwmmode'
        Value = max( min( Value, 1 ), 0 );
        outportd( cabs.BaseAddress+96, 0 );
        outportd( cabs.BaseAddress+100, Value(1) );
        Value(1) = inportd( cabs.BaseAddress+100 );
        outportd( cabs.BaseAddress+96, 1 );
        outportd( cabs.BaseAddress+100, Value(2) );
        Value(2) = inportd( cabs.BaseAddress+100 );
        cabs.PWMMode = Value;
        
     case 'pwmprescaler'
        Value = max( min( Value, 63 ), 0 );
        outportd( cabs.BaseAddress+96, 0 );
        outportd( cabs.BaseAddress+104, Value(1) );
        Value(1) = inportd( cabs.BaseAddress+104 );
        outportd( cabs.BaseAddress+96, 1 );
        outportd( cabs.BaseAddress+104, Value(2) );
        Value(2) = inportd( cabs.BaseAddress+104 );
        cabs.PWMPrescaler = Value;
        
     case 'resetencoder'
        val = 0;
        if Value(1) > 0,  val = val +  1; end
        if Value(2) > 0,  val = val +  2; end
        if Value(3) > 0,  val = val +  4; end
 %       outportb( cabs.BaseAddress+0, 5);
        outportb( cabs.BaseAddress+4*5, val);
        outportb( cabs.BaseAddress+4*5, 0);
   
   case 'stop'
      set( cabs, 'PWM', [ 0 0 ] );
      
   otherwise
      % This should not happen
      error('Unexpected property name.')

   end % switch
end % for


% Finally, assign cabs in caller's workspace
assignin( 'caller', name, cabs )


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Set PWM value
%
function ret = SetPWM( cabs, ChanNo, PWM )

PWM = max(0,PWM);
PWM = min(1,PWM);

Mode  = get(cabs,'PWMMode');
if ChanNo == 0
   outportd( cabs.BaseAddress+96, 0 );
   Mode  = Mode(1);
elseif ChanNo == 1
   outportd( cabs.BaseAddress+96, 1 );
   Mode  = Mode(2);
else
   error( 'Uncorrect PWM channel number' )
end

if Mode == 0 % 8-bit PWM mode
    PWM = round(255 * PWM);
else         % 12-bit PWM mode
    PWM = round(4095 * PWM);
end
outportd( cabs.BaseAddress+108, PWM );
ret = inportd( cabs.BaseAddress+108 );
if Mode == 0 % 8-bit PWM mode
    ret = ret / 255;
else         % 12-bit PWM mode
    ret = ret / 4095;
end

