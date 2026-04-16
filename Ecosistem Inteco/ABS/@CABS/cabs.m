function cabs = ABS
% ABS constructor function
%
% Properties of the ABS class:
%   (sg) 'BaseAddress'      - base address of the RTDAC3 board
%   ( g) 'BitstreamVersion' - bitstream version of the XILINX chip logic
%   ( g) 'Encoder'          - read encoders
%   (sg) 'PWM'              - PWM duty cycle
%   (sg) 'PWMPrescaler'     - PWM frequency prescaler
%   (s ) 'ResetEncoder'     - reset encoder(s)
%   ( g) 'ScaleCoeff'       - encoder pulse/physical units coefficients
%   ( g) 'Time'             - time in ms since object was created
%
%
%   ( g) - available for the get function
%   ( s) - available for the set function
%   (sg) - available for the get and set functions

%   Copyright (c) 2004 by InTeCo, Inc.  (2K)



% Wheel perimeters: [ Wheel Car ] [m]
% The line below is used by the ManualSetup application - DO NOT CHANGE the format
Perimeters = [  0.6004 0.619 ];
% P_erimeters = [ 0.628 0.619  ];

cabs.Type = 'ABS Object';

cabs.BaseAddress      = NaN;
cabs.BitstreamVersion = NaN;
cabs.Encoder          = NaN;
cabs.PWM              = NaN;
cabs.PWMMode          = NaN;
cabs.PWMPrescaler     = NaN;
cabs.ScaleCoeff       = NaN;
cabs.VelocityCoeff    = NaN;
cabs.PowerCoeff       = NaN;
cabs.TemperatureCoeff = NaN;
cabs.Velocity         = NaN;
cabs.VelocityPeriod   = NaN;
cabs.Resistors        = NaN;
cabs.ResTresholds     = NaN;
cabs.MaxVelocity      = NaN;
cabs.MaxVelocityFlag  = NaN;
cabs.PowerSupply      = NaN;
cabs.MinPowerSupply   = NaN;
cabs.PowerSupplyFlag  = NaN;
cabs.Temperature      = NaN;
cabs.MaxTemperature   = NaN;
cabs.TemperatureFlag  = NaN;
cabs.Time             = gettime;

cabs=class(cabs,'CABS');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

cabs.BaseAddress      = BaseAddressDetect;
cabs.BitstreamVersion = get( cabs, 'BitstreamVersion' );
cabs.Encoder          = get( cabs, 'Encoder' );
cabs.PWM              = get( cabs, 'PWM' );
cabs.PWMMode          = get( cabs, 'PWMMode' );
cabs.PWMPrescaler     = get( cabs, 'PWMPrescaler' );
cabs.ScaleCoeff       = [ (Perimeters(2))/(4*1024) (Perimeters(1))/(4*1024) (2*pi)/(4*1024) ];  % [ m/imp m/imp rad/imp ]
Div = inportd( cabs.BaseAddress+80 ); Div = 40e6/Div;
cabs.VelocityCoeff    = 60*Div/1024;                                            % [ RPM/imp ]
cabs.PowerCoeff       = 92000;                                                  % [ V^2 ]
cabs.TemperatureCoeff = 1;
cabs.Velocity         = get( cabs, 'Velocity' );
cabs.VelocityPeriod   = get( cabs, 'VelocityPeriod' );
cabs.Resistors        = get( cabs, 'Resistors' );
cabs.ResTresholds     = get( cabs, 'ResTresholds' );
cabs.MaxVelocity      = get( cabs, 'MaxVelocity' );
cabs.MaxVelocityFlag  = get( cabs, 'MaxVelocityFlag' );
cabs.PowerSupply      = get( cabs, 'PowerSupply' );
cabs.MinPowerSupply   = get( cabs, 'MinPowerSupply' );
cabs.PowerSupplyFlag  = get( cabs, 'PowerSupplyFlag' );
cabs.Temperature      = get( cabs, 'Temperature' );
cabs.MaxTemperature   = get( cabs, 'MaxTemperature' );
cabs.TemperatureFlag  = get( cabs, 'TemperatureFlag' );

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Base address autodetection function
%
function BaseAddr = BaseAddressDetect

BaseAddr = RTDAC4PCIBaseAddress(-1,-1);
% The SelectBoard.mat file is used to distinguish RT-DAC/4 boards
% if more then a single board is available in the computer
if exist('SelectBoard.mat') == 2
    load SelectBoard;
    BaseAddr = BaseAddr(SelectBoard);
end