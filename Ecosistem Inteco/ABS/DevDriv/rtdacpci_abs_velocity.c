 /*
 * RTDACPCI_ABS_VELOCITY.c
 *
 *   S-Function output device driver for the RT-DAC PCI board.
 *   Velocity measurements
 *
 *  Copyright (c) 2003 by INTECO/2K
 *  All Rights Reserved
 */

 
#define S_FUNCTION_NAME rtdacpci_abs_velocity
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <conio.h>
#include <math.h>
#ifdef MATLAB_MEX_FILE
#include <mex.h>
#endif

#include "abs_rtdacpci.c"


/* Input Arguments */
#define BASE_ADDRESS_ARG     (ssGetSFcnParam(S,0))
#define SAMPLE_TIME_ARG      (ssGetSFcnParam(S,1))
#define NUMBER_OF_ARGS   (2)
#define NSAMPLE_TIMES    (1)

#define BASE_ADDRESS     ((real_T) mxGetPr(BASE_ADDRESS_ARG)[0])
#define SAMPLE_TIME      ((real_T) mxGetPr(SAMPLE_TIME_ARG)[0])


static void mdlInitializeSizes(SimStruct *S)
{

    ssSetNumSFcnParams(S, NUMBER_OF_ARGS);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch will be reported by Simulink */
    }
#ifdef MATLAB_MEX_FILE
	if( (int)BASE_ADDRESS == 0 )
      mexPrintf("%s: base address set to 0: entering test mode !!!\n", mexFunctionName() );
   else if( (int)BASE_ADDRESS < 1024 )
      mexErrMsgTxt("Base address must be greated then 1023");
#endif

    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 2);

    if (!ssSetNumInputPorts(S, 0)) return;

    ssSetNumSampleTimes(S, NSAMPLE_TIMES );

    /* Take care when specifying exception free code */
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
    
}
 
 
/* Function to initialize sample times */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0);
}
 

/* Function to compute outputs */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    int    BaseAddress      = (int)BASE_ADDRESS;
    real_T *y = ssGetOutputPortRealSignal(S,0);

    if( BaseAddress == 0 ) return;
 
    *(y+0) = (double)(_inpd( (unsigned short)(BaseAddress + 0x54) ) & 0xFFFF);
    *(y+1) = (double)(_inpd( (unsigned short)(BaseAddress + 0x58) ) & 0xFFFF);
}


/* Function to perform cleanup at execution termination */
static void mdlTerminate(SimStruct *S)
{
}

#ifdef MATLAB_MEX_FILE  /* Is this file being compiled as a MEX-file? */
#include "simulink.c"        /* MEX-File interface mechanism */
#else
#include "cg_sfun.h"    /* Code generation registration function */
#endif
