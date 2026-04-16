/////////////////////////////////////////////////////////////////////
//
// RT-DAC4/PCI board - I/O procedures
//
//  Copyright (c) by InTeCo/2K, 2001
//  All Rights Reserved
//
//
//  PCI local bus I/O space (32-bit double words)
//    Board configuration  (0x00-0x07)
#define RTDACPCI_BITSTREAM_VERSION (0x00) /* get XILINX bitstream version    */
#define RTDACPCI_NO_OF_CHANNELS    (0x04) /* get no of available channels    */
                                          /* 2 bytes: no of counters, timers,*/
                                          /* PWM and encoders                */
//    Counters             (0x08-0x0F)
#define RTDACPCI_COUNTER_IDX       (0x20) /* get/set counter index           */
#define RTDACPCI_COUNTER_RESET     (0x24) /* reset counter value             */
#define RTDACPCI_COUNTER           (0x28) /* get/set counter value           */
#define RTDACPCI_TIMER_IDX         (0x2C) /* get/set timer index             */
#define RTDACPCI_TIMER_RESET       (0x30) /* reset timer value               */
#define RTDACPCI_TIMER             (0x34) /* get/set timer value             */

//    Digital IO           (0x10-0x17)
#define RTDACPCI_DIG_IO_DIR_L      (0x40) /* get/set digital IO direction    */
#define RTDACPCI_DIG_IO_DIR_H      (0x44) /* get/set digital IO direction    */
#define RTDACPCI_DIG_IO_VALUE_L    (0x48) /* get/set digital inputs/outputs  */
#define RTDACPCI_DIG_IO_VALUE_H    (0x4C) /* get/set digital inputs/outputs  */

//    PWM                  (0x18-0x1F)
#define RTDACPCI_PWM_IDX           (0x60) /* get/set PWM index               */
#define RTDACPCI_PWM_MODE          (0x64) /* get/set PWM mode                */
#define RTDACPCI_PWM_PRESCALER     (0x68) /* get/set PWM prescalers          */
#define RTDACPCI_PWM_WIDTH         (0x6C) /* get/set PWM channel width       */

//    Encoders             (0x20-0x27)
#define RTDACPCI_ENCODER_IDX       (0x80) /* get/set encoder index           */
#define RTDACPCI_ENCODER_RESET     (0x84) /* reset encoder counter           */
#define RTDACPCI_ENCODER           (0x88) /* get encoder counter             */

//    A/D converter        (0x28-0x2F)
#define RTDACPCI_AD_MUX_GAIN       (0xA0) /* get/set mux channel             */
#define RTDACPCI_AD_CONTROL        (0xA4) /* get/set ADSTART, CS, RD signal  */
#define RTDACPCI_AD_RESULT         (0xA8) /* get AD conversion result        */

//    D/A converter        (0x30-0x37)
#define RTDACPCI_DA_CONTROL        (0xC0) /* get/set LDAC,CS,WR,A1,A0 signals*/
#define RTDACPCI_DA                (0xC4) /* get/set D/A channel             */
//
//
//  Revision: 1.2 - 2001.11.22
//
/////////////////////////////////////////////////////////////////////

//###################################################################
// TO USUNAC
//#define RTDAC_PCI_VERSION_API
//#define RTDAC_PCI_TIMER_API
//#define RTDAC_PCI_COUNTER_API
//#define RTDAC_PCI_DIGITAL_IO_API
//#define RTDAC_PCI_PWM_API
//#define RTDAC_PCI_ENCODER_API
//#define RTDAC_PCI_AD_API
//#define RTDAC_PCI_DA_API

// #define PLX_API



typedef unsigned char      UI8,  *PUI8;
typedef unsigned short int UI16, *PUI16;
typedef unsigned long  int UI32, *PUI32;


#define LITTLE_ENDIAN

#include <stdio.h>
#include <conio.h>

#ifdef PLX_API
  #include "pcitypes.h"
  #include "plxtypes.h"
  #include "plx.h"
  #include "pciapi.h"

// General get/set functions
static U32 RtdacPCI_Write( U32 Address, U32 Value );
static U32 RtdacPCI_Read( U32 Address );
static unsigned short RtdacPCI_BaseAddress( long *BusNumber, long *SlotNumber, long *DeviceId, long *VendorId  );

// Init/terminate functions, create data file
static U32 RtdacPCI_Init( long *BusNumber, long *SlotNumber, long *DeviceId, long *VendorId  );
static U32 RtdacPCI_Terminate( void );
static U32 RtdacPCI_DataFile( char *FName );
#endif /* PLX_API */

// Version management function
static void RtdacPCI_BitstreamVersion( UI16 BaseAddress, char BitstreamVer[] );

// Counter/timer functions
static UI8  RtdacPCI_ReadNoOfCounters( UI16 BaseAddress );
static UI32 RtdacPCI_ReadCounter(      UI16 BaseAddress, UI8 ChanNo );
static UI32 RtdacPCI_ResetCounter(     UI16 BaseAddress, UI8 ChanNo );
static UI8  RtdacPCI_ReadNoOfTimers(   UI16 BaseAddress );
static UI32 RtdacPCI_ReadTimer(        UI16 BaseAddress, UI8 ChanNo );
static UI32 RtdacPCI_ResetTimer(       UI16 BaseAddress, UI8 ChanNo );

// Digital I/O functions
static UI32 RtdacPCI_WriteDigIOConfig( UI16 BaseAddress, UI32 DIOConfValue );
static UI32 RtdacPCI_ReadDigIOConfig(  UI16 BaseAddress );
static UI32 RtdacPCI_WriteDigIO(       UI16 BaseAddress, UI32 DIOValue );
static UI32 RtdacPCI_ReadDigIO(        UI16 BaseAddress );

// PWM functions
static void RtdacPCI_PWMWrite(     UI16 BaseAddress, 
							       UI8 ChanNo, UI8 Mode, UI16 Presc, UI16 Width );

// Encoder functions
static UI32 RtdacPCI_ResetEncoder(     UI16 BaseAddress, UI8  ChanNo, UI8 ResetFlag );

// A/D and D/A functions
static UI32 RtdacPCI_AD( UI16 BaseAddress, UI8 ChanNo, UI8  Gain );
static UI32 RtdacPCI_DA( UI16 BaseAddress, UI8 ChanNo, UI16 DAValue );


////////////////////////////////////////////////////////////////////
//
// Static global varibles
//
#ifdef PLX_API
static HANDLE myPlxDevice;
static DEVICE_LOCATION device;
static unsigned short RtdacPCI_IO_BaseAddress;


static U32 RtdacPCI_Write( U32 Address, U32 Value )
{
   PlxBusIopWrite( 
		myPlxDevice, IopSpace0,
		(U32)4*Address,
		TRUE, (PU32) &Value,
        4*1, BitSize32 );
   return 0;
}


static U32 RtdacPCI_Read( U32 Address )
{
	unsigned int RetVal;
    PlxBusIopRead( 
			myPlxDevice, IopSpace0,
			(U32) 4*Address,
			TRUE, (PU32) &RetVal,
            4*1, BitSize32 );
	return RetVal;
}

static unsigned short RtdacPCI_BaseAddress( long *BusNumber, long *SlotNumber, long *DeviceId, long *VendorId  )
{
    long    BusNo,SlotNo; 
    unsigned int m,n; 
   
    RETURN_CODE rc;
    DEVICE_LOCATION device;
	 HANDLE myPlxDevice;
    U32 plxDeviceCount;
	 U32 i, j;
	 unsigned short IOBaseAddress;
	 int si;

    BusNo  = *BusNumber; 
    SlotNo = *SlotNumber;
        
    device.BusNumber  = BusNo;
    device.SlotNumber = SlotNo;
    device.VendorId   = *VendorId;
    device.DeviceId   = *DeviceId;
    device.SerialNumber[0] = '\0';
    plxDeviceCount = FIND_AMOUNT_MATCHED;
  
    rc = PlxPciDeviceFind(&device, &plxDeviceCount);

	 si = max( 1, plxDeviceCount );
    if ((rc != ApiSuccess) || (plxDeviceCount == 0))
    {
       mexPrintf("\nError in scanning for devices: no supported device found\n");
  	    *BusNumber  = 0;
	    *SlotNumber = 0;
	    *VendorId   = 0;
	    *DeviceId   = 0;
       return( 0 );
    }

    /* Check the number of devices found */
    if (plxDeviceCount > 1)
    {
        /* Display device information for each one */
        mexPrintf("\nMore than one device found!!\n");
        mexPrintf("Available devices:\n\n");
        mexPrintf("#  Bus Slot VendorId DeviceId\n");
        mexPrintf("-----------------------------\n");
        for (i=0; i < plxDeviceCount; i++)
        {
            /* 
               By selecting the search all criteria again and incrementing 
               the search index, all a driver's devices can be located.
             */
            
            device.BusNumber  = BusNo;
            device.SlotNumber = SlotNo;
            device.DeviceId   = *VendorId;
            device.VendorId   = *DeviceId;
            device.SerialNumber[0] = '\0';
            
            j = i;
            rc = PlxPciDeviceFind(&device, &j);
            if (rc != ApiSuccess)
            {
                mexErrMsgTxt("\nSCANNING ERROR\n");
                return(0);
            }

            mexPrintf("%d  %d   0x%02x %04x     %04x\n", 
                                    i, 
                                    device.BusNumber, 
                                    device.SlotNumber, 
                                    device.VendorId, 
                                    device.DeviceId);
	        IOBaseAddress = PlxPciConfigRegisterRead( device.BusNumber,	
		                                      device.SlotNumber, 
		                                      4*6, &rc );
	        IOBaseAddress = IOBaseAddress & 0xFFF0;

        }
  	   *BusNumber  = 0;
	   *SlotNumber = 0;
	   *VendorId   = 0;
	   *DeviceId   = 0;
		return( 0 );
    }

	/* Only one device found */
    /* Open the device */
    rc = PlxPciDeviceOpen (&device, &myPlxDevice);
    if (rc != ApiSuccess)
    {
        mexPrintf("\nErrors in opening device. Returned code is %d\n",rc);
		  mexErrMsgTxt("");
  	     *BusNumber  = 0;
	     *SlotNumber = 0;
	     *VendorId   = 0;
	     *DeviceId   = 0;
        return( 0 );
    }

	 IOBaseAddress = PlxPciConfigRegisterRead( device.BusNumber,	
		                                       device.SlotNumber, 
		                                       4*6, &rc );
	 IOBaseAddress = IOBaseAddress & 0xFFF0;
    *BusNumber  = device.BusNumber;
	 *SlotNumber = device.SlotNumber;
	 *VendorId   = device.VendorId;
	 *DeviceId   = device.DeviceId;

    PlxPciDeviceClose(myPlxDevice);
   
    return( IOBaseAddress );
}
#endif /* PLX_API */


#ifdef PLX_API
static U32 RtdacPCI_Init( long *BusNumber, long *SlotNumber, long *DeviceId, long *VendorId )
{
    U32 plxDeviceCount;
    RETURN_CODE rc;

    device.BusNumber  = *BusNumber;
    device.SlotNumber = *SlotNumber;
    device.DeviceId   = *DeviceId;
    device.VendorId   = *VendorId;
    device.SerialNumber[0] = '\0';

    plxDeviceCount = FIND_AMOUNT_MATCHED;
    
    rc = PlxPciDeviceFind(&device, &plxDeviceCount);
  
    if ((rc != ApiSuccess) || (plxDeviceCount == 0))
        return( 1 );  // Device not found

    /* Check the number of devices found */
    if (plxDeviceCount > 1)
		return( 2 );  // more then one device found

    /* Open the device */
    rc = PlxPciDeviceOpen (&device, &myPlxDevice);
    if (rc != ApiSuccess)
        return( 3 );  // Errors in opening device

    *BusNumber  = device.BusNumber;
	*SlotNumber = device.SlotNumber;
	*VendorId   = device.VendorId;
	*DeviceId   = device.DeviceId;

	return( 0 );
}


static U32 RtdacPCI_Terminate( void )
{
    PlxPciDeviceClose(myPlxDevice);
	return 0;
}


static U32 RtdacPCI_DataFile( char *FName )
{
	FILE *fd;
	U8 SdkMajor, SdkMinor, SdkRevision;
	U16 eepromData[ 0x100 ];
	U32 pciRegsBuffer[ 128 ];
	BOOLEAN isPresent;
    PLX_POWER_LEVEL powerLevel;
    VIRTUAL_ADDRESSES virtualAddresses;
    U32 BarRanges[ 6 ];
	U32 buffer[ 0x80 ], value;
    RETURN_CODE rc;
	int i;

	fd = fopen( FName, "w+" );

    fprintf(fd,"PLX SDK version:\n");
    rc = PlxSdkVersion( &SdkMajor, &SdkMinor, &SdkRevision );
    fprintf( fd, "SDK Major: %d, Minor: %d, Revision: %d\n",
		     SdkMajor, SdkMinor, SdkRevision );

   	fprintf(fd,"\nBus = 0x%x, Slot = 0x%x, Vendor ID = 0x%x, Device ID = 0x%x.\n\n", 
				device.BusNumber,
				device.SlotNumber,
				device.VendorId,
				device.DeviceId);

    fprintf(fd, "\nPlxPciConfigRegisterReadAll\n" );
	rc = PlxPciConfigRegisterReadAll( device.BusNumber,	device.SlotNumber, 
		                              pciRegsBuffer );
    for(i=0;i<84;i++)
      fprintf(fd, "  %02X: %08X\n", i, pciRegsBuffer[i] );

    isPresent = PlxSerialEepromPresent( myPlxDevice, &rc );
	if( rc == 0 )
		fprintf( fd, "EEPROM detected\n" );

    fprintf(fd, "\nPlxSerialEepromRead\n" );
    rc = PlxSerialEepromRead( myPlxDevice, Eeprom93CS56,
		                      (U32 *)eepromData, (0x86/2)*sizeof(U16) );
    for(i=0;i<0x86/2;i++)
      fprintf(fd, "  %02X: %04X\n", i, eepromData[i] );

    fprintf(fd, "\nPlxPowerLevelGet\n" );
	powerLevel = PlxPowerLevelGet( myPlxDevice, &rc );
    if( powerLevel == D0 )
      fprintf(fd, "OK: power level received as D0\n" );
	else
      fprintf(fd, "ERROR: power level received as %d\n", powerLevel );

    fprintf(fd, "\nPlxPciBarRangeGet\n" );
	for( i=0; i<6; i++ )
	{
		rc = PlxPciBarRangeGet( myPlxDevice, i, &BarRanges[i] );
		if( rc != ApiSuccess )
			fprintf(fd, "Bar[ %d ]: ERROR\n", i );
		else
			fprintf(fd, "Bar[ %d ]: %08X\n", i, BarRanges[ i ] );
	}

    fprintf(fd, "\nPlxPciBaseAddressesGet\n" );
	rc = PlxPciBaseAddressesGet( myPlxDevice, &virtualAddresses );
	if( rc != ApiSuccess ) 
		fprintf( fd, "----> Function ERROR\n" );
	else
	{
        fprintf(fd, "  Va0:   %08lX\n", virtualAddresses.Va0 );
        fprintf(fd, "  Va1:   %08lX\n", virtualAddresses.Va1 );
        fprintf(fd, "  Va2:   %08lX\n", virtualAddresses.Va2 );
        fprintf(fd, "  Va3:   %08lX\n", virtualAddresses.Va3 );
        fprintf(fd, "  Va4:   %08lX\n", virtualAddresses.Va4 );
        fprintf(fd, "  Va5:   %08lX\n", virtualAddresses.Va5 );
        fprintf(fd, "  VaROM: %08lX\n", virtualAddresses.VaRom );
	}


    fprintf(fd, "\nPlxRegisterRead\n" );
	value = PlxRegisterRead( myPlxDevice, 0x18, &rc );
	if( rc != ApiSuccess ) 
      fprintf( fd, "----> Function ERROR\n" );
	else
      fprintf(fd, "  0x18: %08X\n", value );

    fprintf(fd, "\nPlxRegisterReadAll\n" );
	rc = PlxRegisterReadAll( myPlxDevice, 0x0,
		                              0x74, buffer );
	if( rc != ApiSuccess ) 
		fprintf( fd, "----> Function ERROR\n" );
	else
      for(i=0;i<0x74/4;i++)
        fprintf(fd, "  %02X (%02X): %08X\n", i, 4*i, buffer[i] );


    fclose( fd );
	return( 0 );
}
#endif /* PLX_API */


#ifdef RTDAC_PCI_VERSION_API
static void RtdacPCI_BitstreamVersion( UI16 BaseAddress, char BitstreamVer[] )
{
	UI32 Aux;
	Aux = _inpd((unsigned short)(BaseAddress+RTDACPCI_BITSTREAM_VERSION));
	Aux = Aux & 0xFFFF;
	sprintf( BitstreamVer, "%04X", Aux );
}
#endif /* RTDAC_PCI_VERSION_API */

#ifdef RTDAC_PCI_COUNTER_API
static UI8  RtdacPCI_ReadNoOfCounters( UI16 BaseAddress )
{
	return (UI8)((_inpd((unsigned short)(BaseAddress+RTDACPCI_NO_OF_CHANNELS))>>12)&0xF);
}
static UI32 RtdacPCI_ReadCounter( UI16 BaseAddress, UI8 ChanNo )
{
 	 UI32 Aux;
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_COUNTER_IDX), (unsigned long)ChanNo );
    Aux = (UI32) _inpd( (unsigned short)(BaseAddress+RTDACPCI_COUNTER) );
    Aux = Aux & 0xFFFF;
	 return Aux;
}
static UI32 RtdacPCI_ResetCounter( UI16 BaseAddress, UI8 ChanNo )
{
	UI32 Aux;
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_COUNTER_IDX),  (unsigned long)ChanNo );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_COUNTER_RESET), (unsigned long)1 );
    Aux = (UI32) _inpd( (unsigned short)(BaseAddress+RTDACPCI_COUNTER) );
    Aux = Aux & 0xFFFF;
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_COUNTER_RESET), (unsigned long)0 );
	return Aux;
}
#endif /* RTDAC_PCI_COUNTER_API */


#ifdef RTDAC_PCI_TIMER_API
static UI8  RtdacPCI_ReadNoOfTimers( UI16 BaseAddress )
{
	return (UI8)((_inpd((unsigned short)(BaseAddress+RTDACPCI_NO_OF_CHANNELS))>>8)&0xF);
}
static UI32 RtdacPCI_ReadTimer( UI16 BaseAddress, UI8 ChanNo )
{
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_TIMER_IDX), (unsigned long)ChanNo );
    return (UI32) _inpd( (unsigned short)(BaseAddress+RTDACPCI_TIMER) );
}
static UI32 RtdacPCI_ResetTimer( UI16 BaseAddress, UI8 ChanNo)
{
	UI32 Aux;
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_TIMER_IDX),  (unsigned long)ChanNo );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_TIMER_RESET), (unsigned long)1 );
	Aux = (UI32) _inpd( (unsigned short)(BaseAddress+RTDACPCI_TIMER) );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_TIMER_RESET), (unsigned long)0 );
	return Aux;
}
#endif /* RTDAC_PCI_TIMER_API */

#ifdef RTDACPCI_DIGITALDIRECTIONS_API
static UI32 RtdacPCI_WriteDigIOConfig( UI16 BaseAddress, UI32 DIOConfValue )
{
   UI32 Aux;
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_DIR_L), 
           (unsigned long)(DIOConfValue & 0xFFFF) );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_DIR_H), 
           (unsigned long)((DIOConfValue>>16) & 0xFFFF) );

   Aux = _inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_DIR_L) ) & 0xFFFF; 
   Aux = Aux + ((_inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_DIR_H) ) & 0xFFFF)<<16); 
	return Aux;
}

static UI32 RtdacPCI_ReadDigIOConfig( UI16 BaseAddress )
{
   UI32 Aux;
   Aux = _inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_DIR_L) ) & 0xFFFF; 
   Aux = Aux + ((_inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_DIR_H) ) & 0xFFFF)<<16); 
	return Aux;
}
#endif /* RTDACPCI_DIGITALDIRECTIONS_API */


#ifdef RTDAC_PCI_DIGITAL_IO_API
static UI32 RtdacPCI_WriteDigIO( UI16 BaseAddress, UI32 DIOValue )
{
   UI32 Aux;
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_VALUE_L), 
           (unsigned long)(DIOValue & 0xFFFF) );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_VALUE_H), 
           (unsigned long)((DIOValue>>16) & 0xFFFF) );

   Aux = _inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_VALUE_L) ) & 0xFFFF; 
   Aux = Aux + ((_inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_VALUE_H) ) & 0xFFFF)<<16); 
	return Aux;
}

static UI32 RtdacPCI_ReadDigIO( UI16 BaseAddress )
{
   UI32 Aux;
   Aux = _inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_VALUE_L) ) & 0xFFFF; 
   Aux = Aux + ((_inpd( (unsigned short)(BaseAddress+RTDACPCI_DIG_IO_VALUE_H) ) & 0xFFFF)<<16); 
	return Aux;
}
#endif /* RTDAC_PCI_DIGITAL_IO_API */


#ifdef RTDAC_PCI_PWM_API
static void RtdacPCI_PWMWrite( UI16 BaseAddress, 
							   UI8 ChanNo, UI8 Mode, UI16 Presc, UI16 Width )
{
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_PWM_IDX), (unsigned long)ChanNo );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_PWM_MODE), (unsigned long)Mode );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_PWM_PRESCALER), (unsigned long)Presc );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_PWM_WIDTH), (unsigned long)Width );
}
#endif /* RTDAC_PCI_PWM_API */


#ifdef RTDAC_PCI_ENCODER_API
static UI32 RtdacPCI_ResetEncoder( UI16 BaseAddress, UI8 ChanNo, UI8 ResetFlag )
{
	UI32 ResFlag = 0, RetVal = 0;
	ResFlag = _inpd( (unsigned short)(BaseAddress+RTDACPCI_ENCODER_RESET) );
   ResFlag = ResFlag & 0xF;
	if( ResetFlag > 0 )
	  switch( ChanNo ) 
	  {
	    case 0: ResFlag |= 1; break;
	    case 1: ResFlag |= 2; break;
	    case 2: ResFlag |= 4; break;
	    case 3: ResFlag |= 8; break;
	    default: break;
	  }
	else
	  switch( ChanNo ) 
	  {
	    case 0: ResFlag &= 0xFFFE; break;
	    case 1: ResFlag &= 0xFFFD; break;
	    case 2: ResFlag &= 0xFFFB; break;
	    case 3: ResFlag &= 0xFFF7; break;
	    default: break;
	  }
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_ENCODER_RESET),(unsigned long)ResFlag );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_ENCODER_IDX), (unsigned long)ChanNo);
	RetVal = (UI32) _inpd( (unsigned short)(BaseAddress+RTDACPCI_ENCODER) );
	return RetVal & 0xFFFFFFFF;
}

#endif /* RTDAC_PCI_ENCODER_API */

#ifdef RTDAC_PCI_AD_API
static UI32 RtdacPCI_AD( UI16 BaseAddress, UI8 ChanNo, UI8 Gain )
{
   int AuxCounter;
   UI32 ADResult;
	ChanNo &= 0xF;
   switch( Gain )
	{
      case 1:   Gain = 0x0;   break;
      case 2:   Gain = 0x1;   break;
      case 4:   Gain = 0x2;   break;
      case 8:   Gain = 0x3;   break;
      case 16:  Gain = 0x4;   break;
      default:  Gain = 0x0;   break;     // Gain = 1
	}

	_outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_MUX_GAIN), (Gain<<4)+ChanNo);
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_MUX_GAIN), (Gain<<4)+ChanNo);
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_MUX_GAIN), (Gain<<4)+ChanNo);

	_outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL), 0x6 ); // CONVST->0
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL), 0x7 ); // CONVST->1

   _outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL), 0x5 ); // CS->0
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL), 0x1 ); // CS->0, RD->0
   for(AuxCounter=0;AuxCounter<10000;AuxCounter++)
   {
      // Wait for EOC
      if( (_inpd((unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL)) & 1 ) == 0 )
      {
        _inpd((unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL));
        ADResult = _inpd((unsigned short)(BaseAddress+RTDACPCI_AD_RESULT));
        ADResult = ADResult & 0xFFF;
	     _outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL), 0x7 ); // CONVST->1, RD->1, CS->1
        return( ADResult );
      }
   }
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_AD_CONTROL), 0x7 ); // CONVST->1, RD->1, CS->1
   return( 9999 );

   
}
#endif /* RTDAC_PCI_AD_API */


#ifdef RTDAC_PCI_DA_API
static UI32 RtdacPCI_DA( UI16 BaseAddress, UI8 ChanNo, UI16 DAValue )
{
   UI32 Aux = 0;
   // Set DA data
   // PCI LCLK = 10MHz
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA), (UI32)DAValue );

	// Write to D/A buffer
   Aux = Aux | (ChanNo<<3);  // Set channel no
   Aux = Aux | 0x04;         // LDAC->1
   Aux = Aux & 0xFC;         // R/W->0, CS->0
   //A0 A1 LDAC R/W CS = xx100
   // Wait 3 PCI bus cyclec
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux );
	_outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux );

   // LDAC->1, R/W->0, CS->1
   Aux = 0;
   Aux = Aux | (ChanNo<<3);  // Set channel no
   Aux = Aux | 0x05;         // LDAC->1, R/W->0, CS->1
   //A0 A1 LDAC R/W CS = xx101
   // Set LDAC to 0 and wait 3 PCI bus cyclec
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux  ); 
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux  ); 
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux  ); 

   // Update D/A buffer: LDAC-> 1-0-1  min. 180ns
   Aux = 0;
   Aux = Aux | (ChanNo<<3);  // Set channel no
   Aux = Aux | 0x01;         // LDAC->0, R/W->0, CS->1
   //A0 A1 LDAC R/W CS = xx001
   // Set LDAC to 0 and wait 3 PCI bus cyclec
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux  ); 
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux  ); 
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux  ); 
   Aux = Aux | 0x05;
   //A0 A1 LDAC R/W CS = xx101
   _outpd( (unsigned short)(BaseAddress+RTDACPCI_DA_CONTROL), Aux ); 
   return( (UI32)DAValue );
}
#endif /* RTDAC_PCI_DA_API */



#ifdef PLX_API_MAIN
void main( void )
{
	long BusNumber  = MINUS_ONE_LONG;
	long SlotNumber = MINUS_ONE_LONG;
	long DeviceId   = MINUS_ONE_LONG;
	long VendorId   = MINUS_ONE_LONG;

    RtdacPCI_Init( &BusNumber, &SlotNumber, &DeviceId, &VendorId );
    RtdacPCI_DataFile( "C:\\RTDAC-PCI.dat" );




	RtdacPCI_Terminate( );
}
#endif /* PLX_API */



