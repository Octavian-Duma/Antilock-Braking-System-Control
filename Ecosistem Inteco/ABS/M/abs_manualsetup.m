function ABS_ManualSetup

CWD = pwd;
cd( [ matlabroot '\toolbox\ABS\ManualSetup' ] );
!Executor ManualSetup
cd( CWD );