 /*
 * RTDACPCI_ABS_ENCODER.c
 *
 *   S-Function output device driver for the RT-DAC PCI board.
 *   Encoders
 *
 *  Copyright (c) 2003 by INTECO/2K
 *  All Rights Reserved
 */

 
#define S_FUNCTION_NAME rtdacpci_abs_encoder
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <conio.h>
#include <math.h>
#ifdef MATLAB_MEX_FILE
#include <mex.h>
#endif

#define RTDAC_PCI_ENCODER_API
#include "abs_rtdacpci.c"

/* Input Arguments */
#define BASE_ADDRESS_ARG (ssGetSFcnParam(S,0))
#define CHANNEL_IDX_ARG  (ssGetSFcnParam(S,1))
#define SAMPLE_TIME_ARG  (ssGetSFcnParam(S,2))
#define NUMBER_OF_ARGS   (3)
#define NSAMPLE_TIMES    (1)

#define BASE_ADDRESS     ((real_T) mxGetPr(BASE_ADDRESS_ARG)[0])
#define CHANNEL_IDX      ((real_T) mxGetPr(CHANNEL_IDX_ARG)[0])
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
    ssSetOutputPortWidth(S, 0, 1);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

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
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    int  BaseAddress        = (int)BASE_ADDRESS;
    int  ChannelIdx         = (int)CHANNEL_IDX - 1;
    real_T *y = ssGetOutputPortRealSignal(S,0);
    double U                = *uPtrs[0];

    if( BaseAddress == 0 ) { *y=0.0; return; }

    *y = RtdacPCI_ResetEncoder( (UI16)BaseAddress, (UI8)ChannelIdx, (UI8)U );
//    *y = RtdacPCI_ReadEncoder( BaseAddress, ChannelIdx );
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
