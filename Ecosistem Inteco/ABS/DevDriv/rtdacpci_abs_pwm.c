 /*
 * RTDACPCI_ABS_PWM.c
 *
 *   S-Function output device driver for the RT-DAC PCI board.
 *   PWM outputs
 *
 *  Copyright (c) 2003 by INTECO/2K
 *  All Rights Reserved
 */

 
#define S_FUNCTION_NAME rtdacpci_abs_pwm
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <conio.h>
#include <math.h>
#ifdef MATLAB_MEX_FILE
#include <mex.h>
#endif

#define RTDAC_PCI_PWM_API
#include "abs_rtdacpci.c"


/* Input Arguments */
#define BASE_ADDRESS_ARG     (ssGetSFcnParam(S,0))
#define CHANNEL_IDX_ARG      (ssGetSFcnParam(S,1))
#define MODE_ARG             (ssGetSFcnParam(S,2))
#define PRESCALER_ARG        (ssGetSFcnParam(S,3))
#define SET_OUTPUT_FLAG_ARG  (ssGetSFcnParam(S,4))
#define SET_OUTPUT_VALUE_ARG (ssGetSFcnParam(S,5))
#define SAMPLE_TIME_ARG      (ssGetSFcnParam(S,6))
#define NUMBER_OF_ARGS   (7)
#define NSAMPLE_TIMES    (1)

#define BASE_ADDRESS     ((real_T) mxGetPr(BASE_ADDRESS_ARG)[0])
#define CHANNEL_IDX      ((real_T) mxGetPr(CHANNEL_IDX_ARG)[0])
#define MODE_VAL         ((real_T) mxGetPr(MODE_ARG)[0])
#define PRESCALER        ((real_T) mxGetPr(PRESCALER_ARG)[0])
#define SET_OUTPUT_FLAG  ((real_T) mxGetPr(SET_OUTPUT_FLAG_ARG)[0])
#define SET_OUTPUT_VALUE ((real_T) mxGetPr(SET_OUTPUT_VALUE_ARG)[0])
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

    if (!ssSetNumOutputPorts(S, 0)) return;

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);

    ssSetNumSampleTimes(S, NSAMPLE_TIMES );

    /* Take care when specifying exception free code */
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);

#ifdef MATLAB_MEX_FILE
	if( (int)PRESCALER < 0 )
      mexErrMsgTxt("Prescaler must be greater or equal to 0");
	if( (int)PRESCALER > 65536 )
      mexErrMsgTxt("Prescaler must be less then 65536");
#endif
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
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    int    BaseAddress      = (int)BASE_ADDRESS;
    int    ChannelIdx       = (int)CHANNEL_IDX - 1;
    int    Mode             = (int)MODE_VAL;
    int    Prescaler        = (int)PRESCALER;
    double U                = *uPtrs[0];

    if( BaseAddress == 0 ) return;

	 if( U < 0.0 ) U = 0.0;
         if( ChannelIdx == 1 )
	   if( U > 0.7 ) U = 0.7;  // Limit due to the max. brake power supply current
         else
	   if( U > 1.0 ) U = 1.0;  // Full acceleration power
    if( Mode == 1 )  // 8-bit PWM
      U = 255 * U;   // 255-1
    if( Mode == 2 )  // 12-bit PWM
      U = 4095 * U;  // 4095-1

    RtdacPCI_PWMWrite( (UI16)BaseAddress, (UI8)ChannelIdx, (UI8)(Mode-1), 
                       (UI16)Prescaler, (UI16)U ); 
}


/* Function to perform cleanup at execution termination */
static void mdlTerminate(SimStruct *S)
{
    int    BaseAddress      = (int)BASE_ADDRESS;
    int    ChannelIdx       = (int)CHANNEL_IDX - 1;
    int    Mode             = (int)MODE_VAL;
    int    Prescaler        = (int)PRESCALER;
    double U                = SET_OUTPUT_VALUE;

    if( BaseAddress == 0 ) return;

    if( SET_OUTPUT_FLAG > 0.0 ) {
       if( U < 0.0 ) U = 0.0;
       if( U > 1.0 ) U = 1.0;
       if( Mode == 1 )  // 8-bit PWM
          U = 255 * U;   // 255-1
       if( Mode == 2 )  // 12-bit PWM
          U = 4095 * U;  // 4095-1
       RtdacPCI_PWMWrite( (UI16)BaseAddress, (UI8)ChannelIdx, (UI8)(Mode-1), 
                          (UI16)Prescaler, (UI16)U );
    } 
}

#ifdef MATLAB_MEX_FILE  /* Is this file being compiled as a MEX-file? */
#include "simulink.c"        /* MEX-File interface mechanism */
#else
#include "cg_sfun.h"    /* Code generation registration function */
#endif
