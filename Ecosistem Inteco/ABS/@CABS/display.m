function display( cabs )
% Display properties of the ABS object

%   Copyright (c) 2004 by InTeCo, Inc.  (2K)

cabs.Encoder           = get( cabs, 'Encoder' );
cabs.PWM               = get( cabs, 'PWM' );
cabs.PWMMode           = get( cabs, 'PWMMode' );
cabs.PWMPrescaler      = get( cabs, 'PWMPrescaler' );
cabs.Velocity          = get( cabs, 'Velocity' );
cabs.Resistors         = get( cabs, 'Resistors' );
cabs.ResTresholds      = get( cabs, 'ResTresholds' );
cabs.MaxVelocity       = get( cabs, 'MaxVelocity' );
cabs.MaxVelocityFlag   = get( cabs, 'MaxVelocityFlag' );
cabs.PowerSupply       = get( cabs, 'PowerSupply' );
cabs.MinPowerSupply    = get( cabs, 'MinPowerSupply' );
cabs.PowerSupplyFlag   = get( cabs, 'PowerSupplyFlag' );
cabs.Temperature       = get( cabs, 'Temperature' );
cabs.MaxTemperature    = get( cabs, 'MaxTemperature' );
cabs.TemperatureFlag   = get( cabs, 'TemperatureFlag' );

name = inputname(1);
assignin( 'caller', name, cabs );

disp( [ 'Type:             '   cabs.Type ] )
disp( [ 'BaseAddress:      '   num2str( cabs.BaseAddress ) ' / ' ...
                              dec2hex( cabs.BaseAddress ) 'Hex'] )
disp( [ 'Bitstream ver.:   x'  dec2hex( cabs.BitstreamVersion ) ] )
aux = get( cabs, 'Encoder' );
disp( [ 'Encoder:          [ ' num2str( aux ) ' ][bit]' ] )

disp( [ '                  [ ' num2str( aux(1)*cabs.ScaleCoeff(1) ) '[m] ' ... 
                               num2str( aux(2)*cabs.ScaleCoeff(2) ) '[m] ' ...
                               num2str( aux(3)*cabs.ScaleCoeff(3) ) '[rad] ]' ] ) 
disp( [ 'Reset Encoder:    [ ' num2str( get( cabs, 'ResetEncoder' ) ) ' ]'  ] )
disp( [ 'PWM:              [ ' num2str( cabs.PWM(1) ) '  ' num2str( cabs.PWM(2) ) ' ]' ] )
if cabs.PWMMode(1) == 1, M1='12-bits'; else M1='8-bits'; end
if cabs.PWMMode(2) == 1, M2='12-bits'; else M2='8-bits'; end
disp( [ 'PWM Mode:         [ ' M1 '  ' M2 ' ]' ] )
disp( [ 'PWM Prescaler:    [ ' num2str( cabs.PWMPrescaler(1) ) '  ' num2str( cabs.PWMPrescaler(2) ) ' ]' ] )
if cabs.Resistors(1) == 1, R1='ON'; else R1='OFF'; end
if cabs.Resistors(2) == 1, R2='ON'; else R2='OFF'; end
disp( [ 'Resistors:        [ ' R1 '  ' R2 ' ]' ] )
disp( [ 'Res. Thesholds:   [ ' num2str( cabs.ResTresholds(1) ) '  ' num2str( cabs.ResTresholds(2) ) '  ' ...
                               num2str( cabs.ResTresholds(3) ) '  ' num2str( cabs.ResTresholds(4) ) ' ] [RPM]' ] )
disp( [ 'Velocity:         [ ' num2str( cabs.Velocity(1) ) '  ' num2str( cabs.Velocity(2) ) ' ] [RPM]' ] )
if cabs.MaxVelocityFlag > 0, Msg='CAR VELOCITY TO HIGH'; else Msg=''; end
disp( [ 'Max Velocity:     '   num2str(cabs.MaxVelocity) ' [RPM]   ' Msg] )
disp( [ 'Velocity Period:  [ ' num2str( cabs.VelocityPeriod/40 ) ' ] [us]' ] )
if cabs.PowerSupplyFlag > 0, Msg='POWER SUPPLY TO LOW'; else Msg=''; end
disp( [ 'Power Supply:     '   num2str(cabs.PowerSupply) ' [V]   ' Msg] )
disp( [ 'Min Power Supply: '   num2str(cabs.MinPowerSupply) ' [V]' ] )
if cabs.TemperatureFlag > 0, Msg='TEMPERATURE TO HIGH'; else Msg=''; end
disp( [ 'Temperature:      '   num2str(cabs.Temperature) ' [C]   ' Msg] )
disp( [ 'Max Temperature:  '   num2str(cabs.MaxTemperature) ' [C]' ] )

disp( [ 'Time:             '   num2str( get( cabs, 'Time' )/1000 ) ' [sec]' ] )
