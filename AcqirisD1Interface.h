///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AcqirisD1Interface.h:    Digitizer Driver Function Declarations
//
//----------------------------------------------------------------------------------------
//  Copyright 2001-2005 Acqiris. All rights reserved.
//
//  Purpose:    Declaration of AcqrsD1 device driver API
//
//  $Revision: 103 $                                                $Modtime: 20.12.05 18:23 $
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "vpptype.h"
#include "AcqirisDataTypes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////      
// General comments about the function prototypes:
//
// - All function calls require the argument 'instrumentID' in order to identify
//   the Acqiris Digitizer card to which the call is directed.
//   The only exceptions are the initializer functions 'AcqrsD1_getNbrPhysicalInstruments',
//     'AcqrsD1_setSimulationOptions', 'AcqrsD1_init' and 'AcqrsD1_InitWithOptions'.
//   The last two functions actually return instrument identifiers at initialization time,
//   for subsequent use in the other function calls.
//
// - All function calls return a status value of type 'ViStatus' with information about
//   the success or failure of the call.
//   The Acqiris specific values are defined in the AcqirisErrorCodes.h file.
//   The generic VISA ones are listed in the header file 'vpptype.h'.
//
// - If important parameters supplied by the user (e.g. an invalid instrumentID) are found
//   to be invalid, most functions do not execute and return an error code of the type
//   VI_ERROR_PARAMETERi, where i = 1, 2, ... corresponds to the argument number.
//
// - If the user attempts (with a function AcqrsD1_configXXXX) to set a digitizer
//   parameter to a value which is outside of its acceptable range, the function
//   typically adapts the parameter to the closest available value and returns
//   ACQIRIS_WARN_SETUP_ADAPTED. The digitizer parameters actually set can be retrieved
//   with the 'query' functions AcqrsD1_getXXXX.
//
// - Data are always returned through pointers to user-allocated variables or arrays.
//
///////////////////////////////////////////////////////////////////////////////////////////////////      


///////////////////////////////////////////////////////////////////////////////////////////////////      
//    Function Prototypes
///////////////////////////////////////////////////////////////////////////////////////////////////     
ACQ_DLL ViStatus ACQ_CC AcqrsD1_accumulateData(ViSession instrumentID, ViInt32 channel,
    AqReadParameters* readPar,  void* dataArray, ViInt32 sumArray[], AqDataDescriptor* dataDesc,
    void* segDescArray);
// Returns a waveform segment as an array and accumulates it in a client array.
//
// NOTE: Due to the destination 'sumArray' type, this routine should be called at most
//       16777216 times for 'readPar->dataType = 0' (char) or 
//       65536 times for 'readPar->dataType = 1' (short).
//       Otherwise, an overflow may occur and the summed values will wrap around 0. 
//       Note that the sumArray can be interpreted as an unsigned integer. 
//       Alternatively. negative values have to be increased by 2**32.
//
// The following values must be supplied to the function:
//
// 'channel'      = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' )
// 'readPar'      = pointer to a user-supplied structure that specifies what and how to read,
//                  as needed for the function 'AcqrsD1_readData'.
//                  However, only 'readPar->nbrSegments = 1' and 'readPar->readMode = 0' 
//                  (ReadModeStdW) are supported.
//                  'readPar->dataType = 3' (real) and 'readPar->dataType = 2' (long) are NOT 
//                  supported.
//
// Values returned by the function:
//
// 'dataArray'    = user-allocated data destination array of type defined by 'readPar.dataType'
//                  Its size in dataType units MUST be at least (nbrSamples + 32),
//                  for reasons of data alignment. Please refer to the manual for additional 
//                  details.
// 'sumArray'     = user-allocated waveform accumulation array of type ViInt32 (32 bits).
//                  Its size MUST be at least 'nbrSamples'. Note that the sumArray can be 
//                  interpreted
//                  as an unsigned integer. Alternatively. negative values have to be increased by 
//                  2**32.
// 'dataDesc'     = user-allocated structure for returned data descriptor values
// 'segDescArray' = NULL pointer or pointer to a user allocated structure for returned 
//                  AqSegmentDescriptor
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_DATA_ARRAY                 if 'dataArray' is NULL.
// ACQIRIS_ERROR_NOT_SUPPORTED              if this function is not supported by the instrument.
// ACQIRIS_ERROR_INSTRUMENT_RUNNING         if an acquisition is already running.
// ACQIRIS_ERROR_CANNOT_READ_THIS_CHANNEL   if the requested channel is not available.
// VI_ERROR_PARAMETERx                      if one of the parameters has an invalid value, where 
//                                          x = parameter number.
// VI_SUCCESS                               otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_accumulateWform(ViSession instrumentID, ViInt32 channel,
    ViInt32 segmentNumber,        ViInt32 firstSample,        ViInt32 nbrSamples,
    ViChar waveformArray[],        ViInt32 sumArray[],            ViInt32* returnedSamples, 
    ViReal64* sampTime,            ViReal64* vGain,            ViReal64* vOffset);
// Returns a waveform as a byte array and accumulates it in a client array.
//
// The following values must be supplied to the function:
//
// 'channel'        = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' )
// 'segmentNumber'  = requested segment number, may assume 0 to (the number of segments - 1)
//                    as set with the function 'AcqrsD1_configMemory'
// 'firstSample'    = requested position of first sample to read, typically 0. May assume 0 to
//                    (the number of samples - 1) as set with the function 'AcqrsD1_configMemory'
// 'nbrSamples'     = requested number of samples, may assume 1 to the number of samples as
//                    set with the function 'AcqrsD1_configMemory'
//
// Values returned by the function:
//
// 'waveformArray'   = user-allocated waveform destination array of type char or byte (8 bits).
//                     Its size MUST be at least (nbrSamples + 32), for reasons of data
//                     alignment.
// 'sumArray'        = user-allocated waveform accumulation array of type ViInt32 (32 bits).
//                     Its size MUST be at least 'nbrSamples'.
// 'returnedSamples' = number of data samples actually returned
// 'sampTime'        = sampling interval in seconds
// 'vGain'           = vertical gain in Volts/LSB
// 'vOffset'         = vertical offset in Volts
//                     The value in Volts of a data point 'data' in the returned
//                     'waveformArray[]' array is computed with the formula
//                     V = vGain * data - vOffset
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED              if this function is not supported by the instrument.
// ACQIRIS_ERROR_INSTRUMENT_RUNNING         if an acquisition is already running.
// ACQIRIS_ERROR_CANNOT_READ_THIS_CHANNEL   if the requested channel is not available.
// VI_ERROR_PARAMETERx                      if one of the parameters has an invalid value, where 
//                                          x = parameter number.
// VI_SUCCESS                               otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_acqDone(ViSession instrumentID, ViBoolean* done);
// Checks if the acquisition has terminated.
//
// Returns 'done' = VI_TRUE if the acquisition is terminated
//
// Returns one of the following ViStatus values:
// VI_SUCCESS always.
         
ACQ_DLL ViStatus ACQ_CC AcqrsD1_acquire(ViSession instrumentID);
// Starts an acquisition. This function is equivalent to 'acquireEx' with 'acquireMode = 0, 
// acquireFlags = 0'
//
// Common return values:
// ACQIRIS_ERROR_INSTRUMENT_RUNNING    if the instrument is already running. 
// VI_SUCCESS otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_acquireEx(ViSession instrumentID, ViInt32 acquireMode, 
        ViInt32 acquireFlags, ViInt32 acquireParams, ViInt32 reserved);
// Starts an acquisition.
//
// The following values must be supplied to the function:
//
// 'acquireMode'      = 0     Normal, start an acquisition and return immediatly (equivalent to 
//                            function 'acquire').
//                    = 2     Averagers only! Sets continuous accumulation and starts an 
//                            acquisition.
//            
// 'acquireFlags'     = 0     No flags.
//                    = 4     Reset timestamps (if supported). 
// 'acquireParams'    Unused, must be set to 0.
// 'reserved'         Unused, must be set to 0.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_INSTRUMENT_RUNNING if the instrument is already running. 
// ACQIRIS_ERROR_NOT_SUPPORTED      if the requested mode or flag is not supported by the  
//                                  instrument.
// VI_SUCCESS                       otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_averagedData(ViSession instrumentID, ViInt32 channel,
    AqReadParameters* readPar, ViInt32 nbrAcq, ViBoolean calculateMean, ViReal64 timeout,
    void* dataArray, ViInt32 sumArray[], AqDataDescriptor* dataDesc,
    void* segDescArray);
// Perform a series of acquisitions and get the resulting averaged waveform for one segment.
//
// NOTE: Because the acquisition control loop is done inside this function, it is
// suitable ONLY for single instrument, single channel, single segment operation.
//
// NOTE2: This function is for DIGITIZERS ONLY !! For getting the average of an averager module 
//        (AP-XXX) you must use 'readData', and have set the instrument through 'configMode' with 
//        'mode = 2'.
//
// The following values must be supplied to the function:
//
// 'channel'        = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' )
// 'readPar'        = pointer to a user-supplied structure that specifies what and how to read,
//                    as needed for the function 'AcqrsD1_readData'.
//                    However, only 'readPar->nbrSegments = 1' and 'readPar->readMode = 0' 
//                    (ReadModeStdW) are supported. 'readPar->dataType = 3' (real) and 
//                    'readPar->dataType = 2' (long) are NOT supported.
// 'nbrAcq'         = number of acquisitions to be performed. This value can be at most 
//                    16777216 for 'readPar->dataType = 0' (char) or
//                    65536 for 'readPar->dataType = 1' (short)
//                    Otherwise, an overflow may occur and the summed values will wrap around 0.
// 'calculateMean'  = if '1', each sample of the 'sumArray' is divided by the 'nbrAcq', in order
//                    to calculate the mean. If '0', the 'sumArray' will contains the
//                    sample-by-sample sums, and must be divided by 'nbrAcq' afterward to get
//                    the averages values
// 'timeout'        = acquisition timeout in seconds. The function will return
//                    ACQIRIS_ERROR_ACQ_TIMEOUT if there is no trigger within the specified 
//                    timeout interval after the start of each acquisition.
//
// Values returned by the function:
//
// 'dataArray'      = user-allocated data destination array of type defined by 'readPar.dataType'
//                    Its size in dataType units MUST be at least (nbrSamples + 32),
//                    for reasons of data alignment. Please refer to the manual for additional 
//                    details.
// 'sumArray'       = user-allocated waveform accumulation array of type ViInt32 (32 bits).
//                    Its size MUST be at least 'nbrSamples'. Note that the sumArray can be 
//                    interpreted as an unsigned integer. Alternatively, negative values have to be 
//                    increased by 2**32.
// 'dataDesc'       = user-allocated structure for returned data descriptor values, from last 
//                    acquisition.
// 'segDescArray'   = NULL pointer or pointer to a user allocated structure for returned 
//                    AqSegmentDescriptor. Returned value in 'segDescArray' is from the last 
//                    acquisition.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_DATA_ARRAY                 if 'dataArray' is NULL.
// ACQIRIS_ERROR_DATATYPE                   if 'readPar->dataType' is invalid.
// ACQIRIS_ERROR_INSTRUMENT_RUNNING         if an acquisition is already running.
// ACQIRIS_ERROR_CANNOT_READ_THIS_CHANNEL   if the requested channel is not available.
// ACQIRIS_ERROR_ACQ_TIMEOUT                if 'timeout' has elapsed without getting valid 
//                                          triggers.
// VI_ERROR_PARAMETERx                      if one of the parameters has an invalid value, where 
//                                          x = parameter number.
// VI_SUCCESS                               otherwise.
// For other errors, please see 'readData'.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_averagedWform(ViSession instrumentID, ViInt32 channel,
     ViInt32 segmentNumber,        ViInt32 firstSample,        ViInt32 nbrSamples,
    ViInt32 nbrAcq,                ViReal64 timeout,            ViChar waveformArray[], 
    ViInt32 sumArray[],            ViInt32* returnedSamples,    
    ViReal64* sampTime,            ViReal64* vGain,            ViReal64* vOffset);
// Perform a series of acquisitions and get the resulting averaged waveform.
//
// NOTE: The 'sumArray' contains the sample-by-sample sums. To get the average values,
// the array elements must be divided by 'nbrAcq'.
//
// NOTE: Because the acquisition control loop is done inside this function, it is
// suitable ONLY for single instrument, single channel operation.
//
// The following values must be supplied to the function:
//
// 'channel'        = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' )
// 'segmentNumber'  = requested segment number, may assume 0 to (the number of segments - 1)
//                    as set with the function 'AcqrsD1_configMemory'
// 'firstSample'    = requested position of first sample to read, typically 0. May assume 0 to
//                    (the number of samples - 1) as set with the function 'AcqrsD1_configMemory'
// 'nbrSamples'     = requested number of samples, may assume 1 to the number of samples
//                    as set with the function 'AcqrsD1_configMemory'
// 'nbrAcq'         = number of acquisitions to be performed
// 'timeout'        = acquisition timeout in seconds. The function will return an error if there
//                    is no trigger within the specified timeout
//
// Values returned by the function:
//
// 'waveformArray'   = user-allocated waveform destination array of type char or byte (8 bits).
//                     Its size MUST be at least (nbrSamples + 32), for reasons of data
//                     alignment.
// 'sumArray'        = user-allocated waveform accumulation array of type ViInt32 (32 bits).
//                     Its size MUST be at least 'nbrSamples'.
// 'returnedSamples' = number of data samples actually returned
// 'sampTime'        = sampling interval in seconds
// 'vGain'           = vertical gain in Volts/LSB
// 'vOffset'         = vertical offset in Volts
//                     The value in Volts of a data point 'data' in the returned
//                     'waveformArray[]' array is computed with the formula
//                     V = vGain * data - vOffset
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_INSTRUMENT_RUNNING         if an acquisition is already running.
// ACQIRIS_ERROR_CANNOT_READ_THIS_CHANNEL   if the requested channel is not available.
// ACQIRIS_ERROR_ACQ_TIMEOUT                if 'timeout' has elapsed without getting valid 
//                                          triggers.
// VI_ERROR_PARAMETERx                      if one of the parameters has an invalid value, where 
//                                          x = parameter number.
// VI_SUCCESS                               otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_bestNominalSamples(ViSession instrumentID, ViInt32* nomSamples);
// Helper function to ease the instrument configuration.
// Returns maximum nominal number of samples which fits into the available memory.
//
// Values returned by the function:
//
// 'nomSamples'        = maximum number of data samples available
// 
// NOTE: When using this method, make sure to use 'AcqrsD1_configHorizontal' and 
//         'AcqrsD1_configMemory' beforehand to set the sampling rate and the number of
//         segments to the desired values ('nbrSamples' in 'AcqrsD1_configMemory' may be 
//         any number!). 'AcqrsD1_bestNominalSamples' depends on these variables.
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                   if one of the parameters has an invalid value, where 
//                                       x = parameter number.
// VI_SUCCESS                            when a good solution has been found.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_bestSampInterval(ViSession instrumentID, ViInt32 maxSamples,
    ViReal64 timeWindow, ViReal64* sampInterval, ViInt32* nomSamples);
// Helper function to ease the instrument configuration.
// Returns the best possible sampling rate for an acquisition which covers the 'timeWindow'
// with no more than 'maxSamples', taking into account the current state of the instrument,
// in particular the requested channel combination and the number of segments.
// In addition, this routine returns the 'real' nominal number of samples which can
// be accommodated (it is computed as timeWindow/sampInterval !).
//
// The following values must be supplied to the function:
//
// 'maxSamples'       = maximum number of samples to be used
// 'timeWindow'       = time window in seconds to be covered
//
// Values returned by the function:
//
// 'sampInterval'     = recommended sampling interval in seconds
// 'nomSamples'       = recommended number of data samples
//
// NOTE: This function DOES NOT modify the state of the digitizer at all. It simply returns
//         a recommendation that the user is free to override.
// NOTE: When using this method, make sure to use 'AcqrsD1_configMemory' beforehand to set 
//         the number of segments to the desired value. ('nbrSamples' may be any 
//         number!). 'AcqrsD1_bestSampInterval' depends on this variable.
// NOTE: The returned 'recommended' values for the 'sampInterval' and the nominal number
//         of samples 'nomSamples' are expected to be used for configuring the instrument
//         with calls to 'AcqrsD1_configMemory' and 'AcqrsD1_configHorizontal'. Make sure
//         to use the same number of segments in this second call to 'AcqrsD1_configMemory'
//         as in the first one.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_SETUP_NOT_AVAILABLE when the available memory is too short, and the longest
//                                   available sampling interval too short. The returned 
//                                   sampling interval is the longest one possible.
// VI_ERROR_PARAMETERx               if one of the parameters has an invalid value, where 
//                                   x = parameter number.
// VI_SUCCESS                        when a good solution has been found.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_calibrate(ViSession instrumentID); 
// Performs an auto-calibration of the instrument
// Equivalent to AcqrsD1_calibrateEx with 'calType' = 0

ACQ_DLL ViStatus ACQ_CC AcqrsD1_calibrateEx(ViSession instrumentID, ViInt32 calType,
    ViInt32 modifier, ViInt32 flags); 
// Performs a (partial) auto-calibration of the instrument
//
// 'calType'=  0    calibrate the entire instrument (equivalent to 'AcqrsD1_calibrate')
//             1    calibrate only the current channel configuration, 
//                  as set by 'AcqrsD1_configChannelCombination'
//             2    calibrate external clock timing. Requires operation in External Clock 
//                  (Continuous), i.e. 'clockType' = 1, as set with 'AcqrsD1_configExtClock'
//             3    calibrate only at the current frequency (only instruments with fine resolution 
//                  time bases)
//             4    calibrate fast, only at the current settings (if supported). 
//                  Note: In this mode, any change on 'fullscale', 'bandwidth limit', 'channel 
//                  combine', 'impedance' or 'coupling' will require a new calibration.
//                    
// 'modifier'    
//          ['calType' = 0]    currently unused, set to zero
//          ['calType' = 1]    currently unused, set to zero
//          ['calType' = 2]    currently unused, set to zero
//          ['calType' = 3]  0    = All channels
//                          >0    = Only specified channel (for i.e. 1 = calibrate only channel 1)
//          ['calType' = 4]  0    = All channels
//                          >0    = Only specified channel (for i.e. 1 = calibrate only channel 1)
//
// 'flags'            currently unused, set to zero 
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_INSTRUMENT_RUNNING   if the instrument is currently running.
// ACQIRIS_ERROR_CALIBRATION_FAILED   if the requested calibration failed.
// ACQIRIS_ERROR_NOT_SUPPORTED        if the requested calibration is not supported by the 
//                                    instrument.
// ACQIRIS_ERROR_CANNOT_READ_THIS_CHANNEL if the requested channel is not available.
// ACQIRIS_ERROR_COULD_NOT_CALIBRATE  if the requested frequency is invalid ('calType' = 2 only).
// ACQIRIS_ERROR_ACQ_TIMEOUT          if an acquisition timed out during the calibration 
//                                    (e.g. no clocks provided).
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_close(ViSession instrumentID); 
// Close specified instrument. Once closed, this instrument is not available anymore and
// need to be reenabled using 'InitWithOptions' or 'init'.
// Note: For freeing properly all resources, 'closeAll' must still be called when
// the application close, even if 'close' was called for each instrument.
//
// Returns one of the following ViStatus values:
// VI_SUCCESS always.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_closeAll(void); 
// Closes all instruments and prepares for closing of application
//
// Returns one of the following ViStatus values:
// VI_SUCCESS always.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_configAvgConfig(ViSession instrumentID, 
    ViInt32 channel, ViString parameterString, ViAddr value);
// Configures Averagers and Analyzer (APxxx)!
//
// Configures parameter in the channel dependent averager/analyzer configuration 'channel'
// 'channel'        = 1...Nchan
//                  = 0 (selecting channel 1) is supported for backwards compatibility
//
// 'parameterString'= character string specifying the requested parameter
//                    Please refer to the manual for the accepted parameter strings
// 'value'          = value to set
//                    The type of the value depends on 'parameterString'
//                    Please refer to the manual for the required  data type as a
//                    function of the accepted parameters.
//                    NOTE to C/C++ programmers: 'ViAddr' resolves to 'void*'
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED        if this function is not supported by the instrument.
// ACQIRIS_ERROR_PARAM_STRING_INVALID if 'parameterString' is invalid.
// ACQIRIS_WARN_SETUP_ADAPTED         if 'value' has been adapted.
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_configChannelCombination(ViSession instrumentID, 
    ViInt32 nbrConvertersPerChannel, ViInt32 usedChannels);
// Configures combined operation of multiple channels
// 
// 'nbrConvertersPerChannel'    = 1        all channels use 1 converter each (default)
//                              = 2        half of the channels use 2 converters each
//                              = 4        1/4  of the channels use 4 converters each
//
// 'usedChannels'        bit-field indicating which channels are used (LSbit = channel 1)
// The acceptable values for 'usedChannels' depend on 'nbrConvertersPerChannel' 
// and on the number of available channels in the digitizer:
// A) If 'nbrConvertersPerChannel' = 1, 'usedChannels' must reflect the fact that
// ALL channels are available for use. It accepts a single value for a given digitizer:
// 'usedChannels'        = 0x00000001    if the digitizer has 1 channel
//                       = 0x00000003    if the digitizer has 2 channels
//                       = 0x0000000f    if the digitizer has 4 channels
// B) If 'nbrConvertersPerChannel' = 2, 'usedChannels' must reflect the fact that
// only half of the channels may be used:
// 'usedChannels'        = 0x00000001    use channel 1 on a 2-channel digitizer
//                         0x00000002    use channel 2 on a 2-channel digitizer
//                         0x00000003    use channels 1+2 on a 4-channel digitizer
//                         0x00000005    use channels 1+3 on a 4-channel digitizer
//                         0x00000009    use channels 1+4 on a 4-channel digitizer
//                         0x00000006    use channels 2+3 on a 4-channel digitizer
//                         0x0000000a    use channels 2+4 on a 4-channel digitizer
//                         0x0000000c    use channels 3+4 on a 4-channel digitizer
// C) If 'nbrConvertersPerChannel' = 4, 'usedChannels' must reflect the fact that
// only 1 of the channels may be used:
// 'usedChannels'        = 0x00000001    use channel 1 on a 4-channel digitizer
//                         0x00000002    use channel 2 on a 4-channel digitizer
//                         0x00000004    use channel 3 on a 4-channel digitizer
//                         0x00000008    use channel 4 on a 4-channel digitizer
// NOTE: Digitizers which don't support channel combination, always use the default
//       'nbrConvertersPerChannel' = 1, and the single possible value of 'usedChannels'
// NOTE: If digitizers are combined with ASBus, the channel combination applies equally to
//       all participating digitizers.    
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configControlIO(ViSession instrumentID, ViInt32 connector,
    ViInt32 signal, ViInt32 qualifier1, ViReal64 qualifier2);
// Configures Control-IO connectors
// Typically, only a few (or no) IO connectors are present on a single digitizer
// 
// 'connector'        = 1        Front Panel I/O A (MMCX connector)
//                    = 2        Front Panel I/O B (MMCX connector)
//                    = 9        Front Panel Trigger Out (MMCX connector)
// 'signal'           = value depends on 'connector', refer to manual for definitions.
// 'qualifier1', 'qualifier2'    currently unused (set to zero!)
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configExtClock(ViSession instrumentID, ViInt32 clockType,
    ViReal64 inputThreshold, ViInt32 delayNbrSamples, 
    ViReal64 inputFrequency, ViReal64 sampFrequency);
// Configures the external clock of the digitizer
// 
// 'clockType'        = 0        Internal Clock (default at start-up)
//                    = 1        External Clock (continuous operation)
//                    = 2        External Reference (10 MHz)
//                    = 4        External Clock (start/stop operation)
// 'inputThreshold'   = input threshold for external clock or reference in mV
// 'delayNbrSamples'  = number of samples to acquire after trigger (for 'clockType' = 1 ONLY!)
// 'inputFrequency'   = frequency, in Hz, of applied clock input signal
// 'sampFrequency'    = frequency, in Hz, of requested sampling
//
// NOTE: When 'clockType' is set to 1 or 4, the values 'sampInterval' and 'delayTime' in the 
//       function 'AcqrsD1_configHorizontal' are ignored.
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_configFCounter(ViSession instrumentID, ViInt32 signalChannel, 
         ViInt32 typeMes, ViReal64 targetValue, ViReal64 apertureTime, ViReal64 reserved, 
        ViInt32 flags);
// Configures the frequency counter
//
// 'signalChannel'    = 1...Nchan for signal input channel
// 'typeMes'          = 0        Frequency
//                    = 1        Period
//                    = 2        Totalize by Time, counts input pulses during interval defined by 
//                               'apertureTime'
//                    = 3        Totalize by Gate, counts input pulses during interval defined by 
//                               I/O A or B input
// 'targetValue'      = estimate of expected result (set to 0.0, if no estimate available)
// 'apertureTime'     = minimum measurement time for Frequency and Period modes
//                    = time gate for Totalize by Time mode
// 'reserved', 'flags' currently unused (set to zero!)

//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configHorizontal(ViSession instrumentID, 
    ViReal64 sampInterval, ViReal64 delayTime);
// Configures the horizontal control parameters of the digitizer.
//
// 'sampInterval'    = sampling interval in seconds
// 'delayTime'       = trigger delay time in seconds, with respect to the 
//                     beginning of the record. 
//                     A positive number corresponds to trigger BEFORE the beginning 
//                     of the record (post-trigger recording).
//                     A negative number corresponds to pre-trigger recording. It
//                     cannot be smaller than (- sampInterval * nbrSamples), which
//                     corresponds to 100% pre-trigger.
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_configLogicDevice(ViSession instrumentID, 
    ViChar deviceName[], ViChar filePathName[], ViInt32 flags);
// Load/Clear/SetFilePath of the on-board logic devices. ONLY useful for a module 
// with on-board programmable logic devices (APxxx, RCxxx, and 12-bit Digitizers).
//
// 'flags'= operation
//          = 0 program device, using strictly the specified path/file name
//          = 1 clear device
//          = 2 clear and program devices using appropriate files from specified path.
//          = 3 program device, but also look for file path in the 'AqDrv4.ini' file, if the file 
//              was not found. It is sufficient to specify the file name (without explicit path), 
//              if the file is either in the current working directory or in the directory pointed 
//              to by 'FPGAPATH' in 'AqDrv4.ini'.
//
// 'deviceName'   Identifies which device to program. 
//    ['flags' = 0 or 3] = device to program.
//        This string must be "Block1Devx", with 'x' = is the FPGA number to be programmed. 
//        For instance, in the AP200/AP201 it must be "Block1Devx", with 'x' = 1, 2, 3 or 4. 
//    ['flags' = 1] = device to clear, must be "Block1DevAll". 
//    ['flags' = 2] = unused.
//
// 'filePathName'  
//    ['flags' = 0 or 3] = file path and file name. 
//    ['flags' = 1] = unused. 
//    ['flags' = 2] = path (no file name !) where all .bit files can be found.
//
// Note: Most users do not need to call this function. Check the manual for further details.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_PARAM_STRING_INVALID    if 'deviceName' is invalid.
// ACQIRIS_ERROR_FILE_NOT_FOUND          if an FPGA file could not be found (APxxx only).
// ACQIRIS_ERROR_FIRMWARE_NOT_AUTHORIZED if the instrument is not authorized to use the 
//                                       specified 'filePathName' file.
// ACQIRIS_ERROR_EEPROM_DATA_INVALID     if the CPLD 'filePathName' is invalid.
// ACQIRIS_ERROR_FPGA_y_LOAD             if an FPGA could not be loaded (APxxx only), 
//                                       where 'y' = FPGA nbr. 
// ACQIRIS_WARN_SETUP_ADAPTED            if one of the parameters has been adapted.
// VI_SUCCESS                            otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_configMemory(ViSession instrumentID, ViInt32 nbrSamples, 
    ViInt32 nbrSegments);
// Configures the memory control parameters of the digitizer.
//
// 'nbrSamples'        = nominal number of samples to record (per segment!)
// 'nbrSegments'       = number of segments to acquire
//                       1 corresponds to the normal single-trace acquisition mode.
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configMemoryEx(ViSession instrumentID, ViUInt32 nbrSamplesHi, 
    ViUInt32 nbrSamplesLo, ViInt32 nbrSegments, ViInt32 nbrBanks, ViInt32 flags);
// Configures the memory control parameters of the digitizer.
//
// 'nbrSamplesHi'      = reserved for future use, must be set to 0.
// 'nbrSamplesLo'      = nominal number of samples to record (per segment!).
// 'nbrSegments'       = number of segments to acquire per bank
//                       1 corresponds to the normal single-trace acquisition mode.
// 'nbrBanks'          = number of banks in which the memory will be split, 
//                       for buffered reading (DigSSR).
//                       1 corresponds to the normal acquisition mode.
// 'flags'             = 0 no flags. 
//                     = 1 force use of internal memory (for 10-bit digitizers with extended 
//                       memory options only).
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configMode(ViSession instrumentID, ViInt32 mode, 
    ViInt32 modifier, ViInt32 flags);
// Configures the operational mode of the digitizer.
//
// 'mode'            = operational mode
//                   = 0        normal acquisition
//                   = 1        stream data to DPU (only in RCxxx Repeaters or SC2x0/AC2x0 Data 
//                              Streamers)
//                   = 2        averaging mode (only in Real-Time Averagers APx00 and AP240)
//                   = 3        dual-memory mode (only in AP101, AP201)
//                   = 6        frequency counter mode
//                   = 7        SSR mode (for AP235/AP240)
//
// 'modifier' = not used, set to 0
//
// 'flags'    ['mode' = 0]    = 0        normal
//                            = 1        'Start-on-Trigger' mode (if available)
//                            = 2        'Sequence-wrap' mode (use 'nbrSegments' > 1) 
//            ['mode' = 1]    = 0        normal
//                            = 1        autoRestart
//            ['mode' = 3]    = 0        acquire into memory bank 0
//                            = 1        acquire into memory bank 1
//            otherwise  unused, set to 0
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_FILE_NOT_FOUND       if an FPGA file could not be found (APxxx only).
// ACQIRIS_ERROR_FPGA_x_LOAD          if an FPGA could not be loaded (APxxx only), 
//                                    where 'x' = FPGA nbr. 
// ACQIRIS_ERROR_INSTRUMENT_RUNNING   if the instrument is currently running.
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configMultiInput(ViSession instrumentID, ViInt32 channel, 
    ViInt32 input);
// Configures the input multiplexer on a channel
//
//  'input'            = 0        set to input connection A
//                     = 1        set to input connection B
//
// NOTE: This function is only of use for instruments with an input multiplexer (i.e. more
//         than 1 input per channel, e.g. DP211). On the "normal" instruments with a single
//         input per channel, this function may be ignored.
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_configSetupArray(ViSession instrumentID, ViInt32 channel,
     ViInt32 setupType, ViInt32 nbrSetupObj, void* setupData);
// Configures an array of setup data (typically for on-board processing)
//  
// 'setupType'        = indicates the object type (data structure) of which the setup data is 
//                      composed. Some objects might be simple elements, e.g. ViInt32
//
// 'nbrSetupObj'      = number of configuration objects contained in configSetupData
//
// 'setupData'        = pointer to the setup data array
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED        if this function is not supported by the instrument.
// ACQIRIS_ERROR_BUFFER_OVERFLOW      if 'nbrSetupObj' exceeds the maximum allowed value.
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_configTrigClass(ViSession instrumentID, ViInt32 trigClass, 
     ViInt32 sourcePattern, ViInt32 validatePattern, ViInt32 holdType, ViReal64 holdValue1,
     ViReal64 holdValue2);  
// Configures the trigger class control parameters of the digitizer.
//
// 'trigClass'        = 0                edge trigger
// 'sourcePattern'    = 0x00000001    for Channel 1
//                    = 0x00000002    for Channel 2
//                    = 0x00000004    for Channel 3
//                    = 0x00000008    for Channel 4    etc.
//                    = 0x80000000    for External Trigger
// The other parameters are currently ignored, except as noted below
// Modules with the TV-trigger option also accept 'trigClass' = 1 (TV trigger)
// The detailed trigger configuration is set with the function 'AcqrsD1_configTrigTV'

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configTrigSource(ViSession instrumentID, ViInt32 channel, 
     ViInt32 trigCoupling, ViInt32 trigSlope, ViReal64 trigLevel1, ViReal64 trigLevel2);      
// Configures the trigger source control parameters for a specified channel in the digitizer.
//
// 'channel'        = 1... (Number of IntTrigSources) for internal trigger sources
//                  = -1..-(Number of ExtTrigSources) for external trigger sources
// 'trigCoupling'   = 0        DC
//                  = 1        AC
//                  = 2        HFreject (if available)
//                  = 3        DC, 50 Ohms (ext. trigger only, if available)
//                  = 4        AC, 50 Ohms (ext. trigger only, if available)
// 'trigSlope'      = 0        Positive
//                  = 1        Negative
//                  = 2        Window, transition out of window
//                  = 3        Window, transition into window
//                  = 4        HFdivide (by factor 4)
//                  = 5        SpikeStretcher (if available)
// 'trigLevel1'    (internal)    in % of Vertical Full Scale of the channel settings
//                 (external)    in mV
// 'trigLevel2'    (internal)    in % of Vertical Full Scale of the channel settings
//                 (external)    in mV
//                               'trigLevel2' is only used when Window Trigger is selected
// NOTE: Some of the possible states may be unavailable in some digitizers.
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configTrigTV(ViSession instrumentID, ViInt32 channel, 
     ViInt32 standard, ViInt32 field, ViInt32 line);      
// Configures the TV trigger control parameters for a specified channel in the digitizer.
//
// 'channel'        = -1..-(Number of ExtTrigSources) for external trigger sources
//                      NOTE: the TV trigger option is only available on the External Trigger input
// 'standard'       = 0        625 lines per frame / 50 Hz
//                  = 2        525 lines per frame / 60 Hz
// 'field'          = 1        field 1 (odd)
//                  = 2        field 2 (even)
// 'line'           = line number, depends on the 'standard' and 'field' selection:
//                      1 to 263    for 'standard' = 525/60Hz and 'field' = 1
//                      1 to 262    for 'standard' = 525/60Hz and 'field' = 2
//                      1 to 313    for 'standard' = 625/50Hz and 'field' = 1
//                    314 to 625    for 'standard' = 625/50Hz and 'field' = 2
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_configVertical(ViSession instrumentID, ViInt32 channel, 
     ViReal64 fullScale, ViReal64 offset, ViInt32 coupling, ViInt32 bandwidth);
// Configures the vertical control parameters for a specified channel in the digitizer.
//
// 'channel'        = 1...Nchan
//                  = -1  for Ext. Trigger Input of digitizers with programmable Trigger Full Scale
// 'fullScale'      = in Volts
// 'offset'         = in Volts
// 'coupling'       = 0        Ground (Averagers ONLY)
//                  = 1        DC, 1 MOhm
//                  = 2        AC, 1 MOhm
//                  = 3        DC,    50 Ohms
//                  = 4        AC, 50 Ohms
// 'bandwidth'      = 0        no bandwidth limit (default)
//                  = 1        bandwidth limit =  25 MHz
//                  = 2        bandwidth limit = 700 MHz
//                  = 3        bandwidth limit = 200 MHz
//                  = 4        bandwidth limit =  20 MHz
//                  = 5        bandwidth limit =  35 MHz
// NOTE: Not all bandwidth limits are available on a single instrument. In some, there is no
//         bandwidth limiting capability at all. In this case, use 'bandwidth' = 0.
//
// Returns one of the following ViStatus values:
// ACQIRIS_WARN_SETUP_ADAPTED         if one of the parameters has been adapted.
// VI_ERROR_PARAMETER1                if 'instrumentId' is invalid.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_errorMessage(ViSession instrumentID, ViStatus errorCode,
    ViChar errorMessage[]);
// Translates an error code into a human readable form 
//
// 'errorCode'        = Error code (returned by a function) to be translated
// 'errorMessage'     = Pointer to user-allocated character string (minimum size 256),
//                      into which the error-message text is returned
//
// NOTE: 'instrumentID' can be VI_NULL.
//
// Returns one of the following ViStatus values:
// VI_SUCCESS always.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_errorMessageEx(ViSession instrumentID, ViStatus errorCode, 
    ViChar errorMessage[], ViInt32 errorMessageSize);
// Translates an error code into a human readable form. For file errors, the returned message 
// will also contain the file name and the original 'ansi' error string.
//
// 'errorCode'          = Error code (returned by a function) to be translated
// 'errorMessage'       = Pointer to user-allocated character string (suggested size 512 bytes),
//                        into which the error message text will be copied.
// 'errorMessageSize'   = size of 'errorMessage', in bytes.
//
// NOTE: 'instrumentID' can be VI_NULL.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_BUFFER_OVERFLOW    if 'errorMessageSize' is too small.
// VI_SUCCESS                       otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_forceTrig(ViSession instrumentID);
// Forces a 'manual' trigger. The function returns immediately after initiating
// a trigger. One must therefore wait until this acquisition has terminated
// before reading the data, by checking the status with the 'AcqrsD1_acqDone'
// function. Equivalent to AcqrsD1_forceTrigEx with 'forceTrigType' = 0

ACQ_DLL ViStatus ACQ_CC AcqrsD1_forceTrigEx(ViSession instrumentID, ViInt32 forceTrigType,
    ViInt32 modifier, ViInt32 flags);
// Forces a 'manual' trigger. The function returns immediately after initiating
// a trigger. One must therefore wait until this acquisition has terminated
// before reading the data, by checking the status with the 'AcqrsD1_acqDone'
// or the 'AcqrsD1_waitForEndOfAcquisition' functions.
//
// 'forceTrigType'=       0    Sends a software trigger to end the (entire) acquisition. 
//                          In multisegment mode, the current segment is acquired, the acquisition 
//                          is terminated and the data and timestamps of subsequent segments are 
//                          invalid. The 'trigOut' Control IO will NOT generate a trigger output.
//                          Equivalent to 'AcqrsD1_forceTrig'.
//                        1    Send a software trigger similar to a hardware trigger. 
//                          In multisegment mode, the acquisition advances to the next segment and 
//                          then waits again for a trigger. If no valid triggers are provided to 
//                          the device, the application must call 'AcqrsD1_forceTrigEx' as many 
//                          times as there are segments. In this mode, 'trigOut' Control IO will    
//                          generate a trigger output on each successful call. Every acquired 
//                          segment will be valid. This mode is only supported for single 
//                          (i.e. non-ASBus-connected) instruments.
// 'modifier'               currently unused, must be zero
// 'flags'                  currently unused, must be zero 
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_INSTRUMENT_STOPPED          if the instrument is already stopped. 
// ACQIRIS_ERROR_PRETRIGGER_STILL_RUNNING    if the requested data before trigger is being 
//                                           acquired.
// ACQIRIS_ERROR_NOT_SUPPORTED               if this function is not supported by the current 
//                                           mode (e.g. mode Average on APxxx).
// VI_ERROR_PARAMETERx                       if one of the parameters has an invalid value, where 
//                                           x = parameter number.
// VI_SUCCESS                                otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_getAvgConfig(ViSession instrumentID, 
    ViInt32 channel, ViString parameterString, ViAddr value);
// Returns a parameter from the averager/analyzer configuration
// See remarks under 'AcqrsD1_configAvgConfig'
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_PARAM_STRING_INVALID if 'parameterString' is invalid.
// ACQIRIS_ERROR_NOT_SUPPORTED        if this function is not supported by the  
//                                    instrument.
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_getChannelCombination(ViSession instrumentID, 
    ViInt32* nbrConvertersPerChannel, ViInt32* usedChannels);
// Returns parameters of combined operation of multiple channels
// See remarks under 'AcqrsD1_configChannelCombination'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getControlIO(ViSession instrumentID, ViInt32 connector,
    ViInt32* signal, ViInt32* qualifier1, ViReal64* qualifier2);
// Returns the state of Control-IO connectors
// See remarks under 'AcqrsD1_configControlIO'
//
// SPECIAL CASE: If 'connector' = 0 is specified, the returned value of 'signal'
// is the bit-coded list of the 'connectors' which are available in the digitizer.
// E.g. If the connectors 1 (I/O A) and 9 (TrigOut) are present, the bits 1 and 9 of
// 'signal' are set, where bit 0 is the LSbit and 31 is the MSbit. 
// Thus, 'signal' would be equal to 0x202.
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getExtClock(ViSession instrumentID, ViInt32* clockType,
    ViReal64* inputThreshold, ViInt32* delayNbrSamples,
    ViReal64* inputFrequency, ViReal64* sampFrequency);
// Returns the (external) clock parameters of the digitizer
// See remarks under 'AcqrsD1_configExtClock'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_getFCounter(ViSession instrumentID, ViInt32* signalChannel, 
         ViInt32* typeMes, ViReal64* targetValue, ViReal64* apertureTime, ViReal64* reserved, 
        ViInt32* flags);
// Returns the current settings of the frequency counter
// See remarks under 'AcqrsD1_configFCounter'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_getHorizontal(ViSession instrumentID, 
    ViReal64* sampInterval, ViReal64* delayTime);
// Returns the current horizontal control parameters of the digitizer.
// See remarks under 'AcqrsD1_configHorizontal'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getInstrumentData(ViSession instrumentID,
    ViChar name[], ViInt32* serialNbr, ViInt32* busNbr, ViInt32* slotNbr);
// Returns some basic data about a specified digitizer
//
// Values returned by the function:
//
// 'name'            = pointer to user-allocated string, into which the
//                     model name is returned (length < 32 characters)
// 'serialNbr'       = serial number of the digitizer
// 'busNbr'          = bus number where digitizer is located
// 'slotNbr'         = slot number where digitizer is located
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getInstrumentInfo(ViSession instrumentID, 
    ViString parameterString, ViAddr infoValue);
// Returns general information about a specified digitizer
//
// The following value must be supplied to the function:
//
// 'parameterString'  = character string specifying the requested parameter
//                      Please refer to the manual for the accepted parameter strings
//
// Value returned by the function:
//
// 'infoValue'        = value of requested parameter
//                      The type of the returned value depends on the parameter string
//                      Please refer to the manual for the required  data type as a
//                      function of the accepted parameter strings
//                      NOTE to C/C++ programmers: 'ViAddr' resolves to 'void*'
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_PARAM_STRING_INVALID if 'parameterString' is invalid.
// ACQIRIS_ERROR_NOT_SUPPORTED        if the requested value is not supported by the 
//                                    instrument.
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getMemory(ViSession instrumentID, 
    ViInt32* nbrSamples, ViInt32* nbrSegments);
// Returns the current memory control parameters of the digitizer.
// See remarks under 'AcqrsD1_configMemory'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.
                                      
ACQ_DLL ViStatus ACQ_CC AcqrsD1_getMemoryEx(ViSession instrumentID, 
    ViUInt32* nbrSamplesHi, ViUInt32* nbrSamplesLo, ViInt32* nbrSegments, ViInt32* nbrBanks, 
    ViInt32* flags);
// Returns the current memory control parameters of the digitizer.
// See remarks under 'AcqrsD1_configMemoryEx'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getMode(ViSession instrumentID, ViInt32* mode,
    ViInt32* modifier, ViInt32* flags);
// Returns the current operational mode of the digitizer.
// See remarks under 'AcqrsD1_configMode'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_getMultiInput(ViSession instrumentID, ViInt32 channel, 
    ViInt32* input);
// Returns the multiple input configuration on a channel
// See remarks under 'AcqrsD1_configMultiInput'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getNbrChannels(ViSession instrumentID, ViInt32* nbrChannels);
// Returns the number of channels on the specified digitizer.
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getNbrPhysicalInstruments(ViInt32* nbrInstruments);
// Returns the number of supported physical Acqiris digitizers found on the computer.
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETER1                if 'nbrInstruments' is NULL.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_getSetupArray(ViSession instrumentID, ViInt32 channel,
     ViInt32 setupType, ViInt32 nbrSetupObj, void* setupData, ViInt32* nbrSetupObjReturned);
// Returns setup data array (typically used for on-board processing).
// See remarks under 'AcqrsD1_configSetupArray'

//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED        if this function is not supported by the instrument.
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getTrigClass(ViSession instrumentID, ViInt32* trigClass, 
     ViInt32* sourcePattern, ViInt32* validatePattern, ViInt32* holdType, ViReal64* holdValue1,
     ViReal64* holdValue2);
// Returns the current trigger class control parameters of the digitizer.
// See remarks under 'AcqrsD1_configTrigClass'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getTrigSource(ViSession instrumentID, ViInt32 channel, 
     ViInt32* trigCoupling, ViInt32* trigSlope, ViReal64* trigLevel1, ViReal64* trigLevel2);
// Returns the current trigger source control parameters for a specified channel.
// See remarks under 'AcqrsD1_configTrigSource'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getTrigTV(ViSession instrumentID, ViInt32 channel, 
     ViInt32* standard, ViInt32* field, ViInt32* line);      
// Returns the current TV trigger control parameters of the digitizer.
// See remarks under 'AcqrsD1_configTrigTV'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getVersion(ViSession instrumentID,
    ViInt32 versionItem, ViInt32* version);
// Returns version numbers associated with a specified digitizer / current device driver
//
// The following value must be supplied to the function:
//
// 'versionItem'    =   1        for version of Kernel-Mode Driver 
//                      2        for version of EEPROM Common Section 
//                      3        for version of EEPROM Digitizer Section
//                      4        for version of CPLD firmware
//
// Value returned by the function:
//
// 'version'        = version number.
//
// For drivers, the version number is composed of 2 parts. The upper 2 bytes represent
// the major version number, and the lower 2 bytes represent the minor version number. 
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_getVertical(ViSession instrumentID, ViInt32 channel, 
     ViReal64* fullScale, ViReal64* offset, ViInt32* coupling, ViInt32* bandwidth);
// Returns the current vertical control parameters for a specified channel.
// See remarks under 'AcqrsD1_configVertical'
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    x = parameter number.
// VI_SUCCESS                         otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_init(ViRsrc resourceName, ViBoolean IDQuery, 
    ViBoolean resetDevice, ViSession* instrumentID);
// Initializes an instrument
// See remarks under 'AcqrsD1_InitWithOptions'
 
ACQ_DLL ViStatus ACQ_CC AcqrsD1_InitWithOptions(ViRsrc resourceName, ViBoolean IDQuery, 
    ViBoolean resetDevice, ViString optionsString, ViSession* instrumentID);
// Initializes an instrument with options
//
// The following values must be supplied to the function:
//
// 'resourceName'   = an ASCII string which identifies the digitizer to be initialized
//                    See manual for a detailed description of this string.
// 'IDQuery'        = currently ignored
// 'resetDevice'    = if set to 'TRUE', resets the digitizer after initialization
// 'optionsString'  = an ASCII string which specifies options. Currently, we support
//                    "CAL=False" to suppress calibration during the initialization
//                    "DMA=False" to inhibit (for diagnostics) the use of scatter-gather DMA for 
//                    data transfers
//                    "Simulate=True" for the use of simulated digitizers during application 
//                    development. 
// Values returned by the function:
//
// 'instrumentID'   = identifier for subsequent use in the other function calls.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_INIT_STRING_INVALID        if the 'resourceName' is invalid.
// ACQIRIS_ERROR_INTERNAL_DEVICENO_INVALID  if the 'resourceName' contains values that are not 
//                                          within the expected ranges (e.g. wrong serial number).
// ACQIRIS_ERROR_TOO_MANY_DEVICES           if there are too many devices installed.
// ACQIRIS_ERROR_KERNEL_VERSION             if the instrument require a newer kernel driver.
// ACQIRIS_ERROR_HW_FAILURE                 if the instrument doesn't answer properly to 
//                                          defined requests.
// ACQIRIS_ERROR_HW_FAILURE_CHx             if one of the channels doesn't answer properly to 
//                                          defined requests, where 'x' = channel number.
// VI_ERROR_PARAMETERx                      if one of the parameters has an invalid value, where 
//                                          'x' = parameter number.
// VI_SUCCESS                               otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_logicDeviceIO(ViSession instrumentID,
    ViChar deviceName[],        ViInt32 registerID,            ViInt32 nbrValues,
    ViInt32 dataArray[],        ViInt32 readWrite,            ViInt32 flags);
// Currently ONLY for Averagers (APxxx) and Repeaters (RCxxx)!
//
// Reads/writes a number of 32-bit data values from/to a user-defined register in the
// logic device identified by 'deviceName[]'. 
// ONLY useful for a digitizer with on-board programmable logic devices.
//
// The following values must be supplied to the function:
//
// 'deviceName'       = an ASCII string which identifies the device
//                      Must be of the form "BlockMDevN", where M = 1..4 and N = 1..number
//                      of logical devices in the device block M.
//                      In the RC200, this string must be "Block1Dev1"
//                      See manual for a detailed description of this string.
// 'registerID'       = 0..to (nbrRegisters-1)
// 'nbrValues'        = number of 32-bit values to be transferred
// 'dataArray[]'      = user-supplied buffer for data values
// 'readWrite'        = 0 for reading (from logic device to application)
//                      1 for writing (from application to logic device)
// 'flags'              currently ignored, use '0'
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_PARAM_STRING_INVALID   if the 'deviceName' is not valid.
// ACQIRIS_ERROR_NO_ACCESS              if the operation is not authorized.
// ACQIRIS_ERROR_NOT_SUPPORTED          if the operation is not supported by the instrument,  
//                                          or if 'registerID' is outside the expected values.
// ACQIRIS_ERROR_INSTRUMENT_RUNNING     if the instrument was not stopped beforehand.
// ACQIRIS_ERROR_DATA_ARRAY             if 'dataArray' is NULL.
// ACQIRIS_ERROR_IO_WRITE               if a 'write' verify failed.
// VI_ERROR_PARAMETERx                  if one of the parameters has an invalid value, where 
//                                      'x' = parameter number.
// VI_SUCCESS                           otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_multiInstrAutoDefine(ViString optionsString, 
    ViInt32* nbrInstruments);
// Automatically combines as many digitizers as possible to "MultiInstrument"s.
// Digitizers are only combined if they are physically connected via ASBus.
// This call must be followed by 'nbrInstruments' calls to 'AcqrsD1_init' or 
// 'AcqrsD1_InitWithOptions' to retrieve the 'instrumentID's of the (multi)digitizers.
//
// The following value must be supplied to the function:
//
// 'optionsString'    = an ASCII string which specifies options. 
//                      Currently, no options are supported
//
// Values returned by the function:
//
// 'nbrInstruments'   = number of user-accessible instruments. This number includes 
//                      also single instruments that don't participate on the ASBus.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_EEPROM_DATA_INVALID  if one of the instruments being initialized has invalid 
//                                    data in EEPROM.
// ACQIRIS_ERROR_UNSUPPORTED_DEVICE   if one of the instruments being initialized is not 
//                                    supported by this driver version.
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    'x' = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_multiInstrDefine(ViSession instrumentList[], 
    ViInt32 nbrInstruments, ViSession masterID, ViSession* instrumentID);
// 'Manually' combines a number of digitizers into a single "MultiInstrument"
//
// The following values must be supplied to the function:
//
// 'instrumentList'    = array of 'instrumentID' of already initialized single digitizers
// 'nbrInstruments'    = number of digitizers in the 'instrumentList'
// 'masterID'          = 'instrumentID' of master digitizer
//
// Values returned by the function:
//
// 'instrumentID'    = identifier of the new "MultiInstrument", for subsequent use 
//                     in the other function calls.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED        if this function is not supported by the instrument(s).
// ACQIRIS_ERROR_NOT_ENOUGH_DEVICES   if 'nbrInstruments' is < 1.
// ACQIRIS_ERROR_TOO_MANY_DEVICES     if 'nbrInstruments' exceeds the maximum number 
//                                    of ASBUS instruments.
// ACQIRIS_ERROR_INSTRUMENT_NOT_FOUND if one of the 'instrumentList[]' entries is invalid.
// ACQIRIS_ERROR_NO_MASTER_DEVICE     if 'masterID' is invalid.
// ACQIRIS_ERROR_SETUP_NOT_AVAILABLE  if one of the 'instrumentList[]' entries is not ASBUS 
//                                    compatible.
// ACQIRIS_ERROR_UNSUPPORTED_DEVICE   if one of the 'instrumentList[]' entries is not supported 
//                                    by this driver version.
// VI_ERROR_PARAMETERx                if one of the parameters has an invalid value, where 
//                                    'x' = parameter number.
// VI_SUCCESS                         otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_multiInstrUndefineAll(ViString optionsString);
// Undefines all "MultiInstruments".
//
// The following value must be supplied to the function:
//
// 'optionsString'    = an ASCII string which specifies options. 
//                      Currently, no options are supported
//
// Please refer to the User's manual for a detailed description of the steps required
// to reestablish the identifiers of the existing individual digitizers, in order
// to continue using them.
//
// Returns one of the following ViStatus values:
// VI_SUCCESS always.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_processData(ViSession instrumentID, ViInt32 processType, 
            ViInt32 flags);
// Starts data processing on acquired data (only in instruments with on-board data processing)
//
// The following values must be supplied to the function:
//
// 'processType'    =    0        for no processing
//                       1        for extrema mode 
//                       2        for hysteresis mode 
//                       3        for interpolated extrema mode
//                       4        for interpolated hysteresis mode
//
//    defines how switching for the dual-bank memory is done 
//
// 'flags'          =    0        switching is done by software
//                       1        switching is automatic, auto switch turned on
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED      if this function is not supported by the instrument.
// VI_SUCCESS                       otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_procDone(ViSession instrumentID, ViBoolean* done);
// Checks if the on-board processing has terminated.
//
// Returns 'done' = VI_TRUE if the processing is terminated
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED      if this function is not supported by the instrument.
// VI_ERROR_PARAMETERx              if one of the parameters has an invalid value, where 
//                                  'x' = parameter number.
// VI_SUCCESS                       otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_readCharSequence(ViSession instrumentID, 
    ViInt32 channel,             ViInt32 firstSegment,        ViInt32 nbrSegments,        
    ViInt32 firstSampleInSeg,    ViInt32 nbrSamplesInSeg, 
    ViInt32 segmentOffset,        ViInt32 arraySize,            ViChar waveformArray[],         
    ViReal64 horPos[],            ViReal64* sampTime,            ViReal64* vGain, 
    ViReal64* vOffset,            ViInt32 timeStampLo[],        ViInt32 timeStampHi[]);
// Returns a sequence waveform as an array of byte arrays.
// This function is faster than a loop over 'AcqrsD1_readCharWform()' if many
// short segments (< 10'000 points/segment) are read. See Manual for details.
//
// The following values must be supplied to the function:
//
// 'channel'          = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' )
// 'firstSegment'     = requested first segment number, may assume 0 to (nbrSegments - 1),  
//                      as set with the function 'AcqrsD1_configMemory'
// 'nbrSegments'      = requested number of segments, may assume 1 to the number of 
//                      segments set with the function 'AcqrsD1_configMemory'
// 'firstSampleInSeg' = requested position of first sample in segment to read, 
//                      typically 0. May assume 0 to (nbrSamples - 1) as set 
//                      with the function 'AcqrsD1_configMemory'
// 'nbrSamplesInSeg'  = requested number of samples per segment, may assume 1 to the 
//                      number of samples as set with the function 'AcqrsD1_configMemory'
// 'segmentOffset'    = requested offset (in samples!) between adjacent segments in the
//                      destination buffer 'waveformArray'. Must be >= 'nbrSamplesInSeg'.
// 'arraySize'        = number of data elements in the user-allocated 'waveformArray'
//                      Refer to function description in the manual for the required
//                      array size!
//
// Values returned by the function:
//
// 'waveformArray'  = user-allocated waveform destination array of type char or byte. 
//                    Refer to function description in the manual for the required
//                    array size!
// 'horPos'         = user-allocated array of horizontal positions, one per segment, 
//                    of first (nominal) data point with respect to the origin of the 'nominal
//                    trigger delay' in seconds. Since the first data point is BEFORE the 
//                    origin, these numbers will be in the range [-sampTime, 0].
//                    The number of elements in the 'horPos' array MUST be >= the number 
//                    of segments set with the function 'AcqrsD1_configMemory'.
// 'sampTime'       = sampling interval in seconds
// 'vGain'          = vertical gain in Volts/LSB
// 'vOffset'        = vertical offset in Volts
//                    The value in Volts of a data point 'data' in the returned
//                    'waveformArray[]' array is computed with the formula
//                    V = vGain * data - vOffset
// 'timeStampLo' + 
// 'timeStampHi'    = user-allocated arrays of low and high part of the 64-bit trigger time stamps,
//                    one per segment, in units of picoseconds. These values are the trigger 
//                    times with respect to an arbitrary time origin (typically the start-time
//                    of the acquisition).
//                    The number of elements in the 'timeStampXx' arrays MUST be >= the  
//                    number of segments set with the function 'AcqrsD1_configMemory'.
//                    Intended for the computation of time differences between two waveform
//                    acquisitions. See manual for detailed explanations.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED          if this function is not supported by the instrument.
// ACQIRIS_ERROR_INSTRUMENT_RUNNING     if the instrument is running.
// ACQIRIS_ERROR_NO_DATA                if nothing was acquired beforehand.
// VI_ERROR_PARAMETERx                  if one of the parameters has an invalid value, where 
//                                      'x' = parameter number.
// VI_SUCCESS                           otherwise.
// For other errors, please see 'readData'.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_readCharWform(ViSession instrumentID, ViInt32 channel,
     ViInt32 segmentNumber,        ViInt32 firstSample,        ViInt32 nbrSamples, 
    ViChar waveformArray[],        ViInt32* returnedSamples,    ViInt32* addrFirstPoint, 
    ViReal64* horPos,            ViReal64* sampTime,            ViReal64* vGain, 
    ViReal64* vOffset,            ViInt32* timeStampLo,        ViInt32* timeStampHi);
// Returns a waveform as a byte array.
//
// The following values must be supplied to the function:
//
// 'channel'        = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' )
// 'segmentNumber'  = requested segment number, may assume 0 to (nbrSegments - 1)
//                    as set with the function 'AcqrsD1_configMemory'
// 'firstSample'    = requested position of first sample to read, typically 0
// 'nbrSamples'     = requested number of samples, may assume 1 to the number of samples
//                    set with the function 'AcqrsD1_configMemory'
//
// Values returned by the function:
//
// 'waveformArray'  = user-allocated waveform destination array of type char or byte. 
//                    Its size MUST be at least (nbrSamples + 32), for reasons of data
//                    alignment.
// 'returnedSamples'= number of data samples actually returned
// 'addrFirstPoint' = the offset of the first valid data point in the destination array.
//                    It should always be in the range [0...31].
// 'horPos'         = horizontal position of first (nominal) data point with respect 
//                    to the origin of the 'nominal trigger delay' in seconds. Since 
//                    the first data point is BEFORE the origin, this number will be 
//                    in the range [-sampTime, 0].
// 'sampTime'       = sampling interval in seconds
// 'vGain'          = vertical gain in Volts/LSB
// 'vOffset'        = vertical offset in Volts
//                    The value in Volts of a data point 'data' in the returned
//                    'waveformArray[]' array is computed with the formula
//                    V = vGain * data - vOffset
// 'timeStampLo' + 
// 'timeStampHi'    = low and high part of the 64-bit trigger time stamp, in units of 
//                    picoseconds. This value is the trigger time with respect to an
//                    arbitrary time origin (typically the start-time of the acquisition).
//                    Intended for the computation of time differences between two waveform
//                    acquisitions. See manual for detailed explanations.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED          if this function is not supported by the instrument.
// ACQIRIS_ERROR_INSTRUMENT_RUNNING     if the instrument is running.
// ACQIRIS_ERROR_NO_DATA                if nothing was acquired beforehand.
// VI_ERROR_PARAMETERx                  if one of the parameters has an invalid value, where 
//                                      'x' = parameter number.
// VI_SUCCESS                           otherwise.
// For other errors, please see 'readData'.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_readData(ViSession instrumentID, ViInt32 channel,
    AqReadParameters* readPar, void* dataArray, AqDataDescriptor* dataDesc,
    void* segDescArray);
// Returns a waveform and associated descriptors, in one of several possible formats
// This function is the preferred way of reading waveforms, since it includes the capabilities
// of all the other read functions, and more!
//
// The following values must be supplied to the function:
//
// 'channel'        = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' )
// 'readPar'        = pointer to a user-supplied structure that specifies what and how to read
//
// Values returned by the function:
//
// 'dataArray'      = user-allocated data destination array of type defined by 'readPar.dataType'
//                    When reading a single segment of raw data, its size MUST be at least
//                    (nbrSamples + 32), for reasons of data alignment. Please refer to the manual
//                    for additional details.
// 'dataDesc'       = user-allocated structure for returned data descriptor values
// 'segDescArray'   = user allocated array of structures for returned segment descriptor values
//                    This array must contain at least 'readPar.nbrSegments' elements of the
//                    appropriate type (typically 'AqSegmentDescriptor' or 'AqSegmentDescriptorAvg')
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_INSTRUMENT_RUNNING       if the instrument is running.
// ACQIRIS_ERROR_READMODE                 if 'readPar->readMode' is not valid.
// ACQIRIS_ERROR_NOT_SUPPORTED            if 'readPar->readMode' is not supported by the instrument.
// ACQIRIS_ERROR_DATATYPE                 if 'readPar->dataType' is not valid for the 
//                                        choosen 'readPar->readMode' or for this instrument.
// ACQIRIS_ERROR_FIRST_SEG                if 'readPar->firstSegment' is invalid.
// ACQIRIS_ERROR_NBR_SEG                  if 'readPar->nbrSegments' is invalid.
// ACQIRIS_ERROR_DATA_ARRAY or
// ACQIRIS_ERROR_NBR_SAMPLE               if 'readPar->dataArraySize' is invalid.
// ACQIRIS_ERROR_SEG_DESC_ARRAY           if 'readPar->segDescArraySize' is invalid.
// ACQIRIS_ERROR_SEG_OFF                  if 'readPar->segmentOffset' is invalid.
// ACQIRIS_ERROR_NBR_SEG                  if 'readPar->nbrSegments' is invalid.
// ACQIRIS_ERROR_BUFFER_OVERFLOW          if 'readPar->dataArraySize' is too small.
// ACQIRIS_ERROR_NO_DATA                  if nothing was acquired beforehand.
// ACQIRIS_ERROR_CANNOT_READ_THIS_CHANNEL if the requested channel is not available.
// ACQIRIS_ERROR_READ_TIMEOUT             if the reading encoutered a problem.
// ACQIRIS_WARN_READPARA_NBRSEG_ADAPTED   if 'readPar->nbrSegments' has been adapted.
// ACQIRIS_WARN_ACTUAL_DATASIZE_ADAPTED   if 'readPar->dataArraySize' has been adapted.
// ACQIRIS_WARN_READPARA_FLAGS_ADAPTED    if 'readPar->flags' has been adapted.
// VI_ERROR_PARAMETERx                    if one of the parameters has an invalid value, where 
//                                        'x' = parameter number.
// VI_SUCCESS                             otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_readFCounter(ViSession instrumentID, ViReal64* result); 
//    Reads the frequency counter
// 'result'       = result of measurement, whose units depend on the measurement 'type':
//                  Hz     for typeMes = 0 (Frequency)
//                  sec    for typeMes = 1 (Period)
//                  counts for typeMes = 2 (Totalize)
//
// Common return values:
// ACQIRIS_ERROR_NOT_SUPPORTED if this function is not supported by the instrument.
// ACQIRIS_ERROR_NO_DATA       if the instrument has not first acquired data in the 'frequency 
//                             counter' mode (mode = 6).
// VI_ERROR_PARAMETERx         if one of the parameters has an invalid value, where 
//                             'x' = parameter number.
// VI_SUCCESS                  otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_readRealSequence(ViSession instrumentID, 
    ViInt32 channel,             ViInt32 firstSegment,        ViInt32 nbrSegments,
    ViInt32 firstSampleInSeg,     ViInt32 nbrSamplesInSeg, 
    ViInt32 segmentOffset,        ViInt32 arraySize,            ViReal64 waveformArray[], 
    ViReal64 horPos[],            ViReal64* sampTime,            
    ViInt32 timeStampLo[],        ViInt32 timeStampHi[]);
// Returns a sequence waveform as an array of floating point (double) arrays, with the
// measured data values in Volts.
// This function is faster than a loop over 'AcqrsD1_readRealWform()' if many
// short segments (< 10'000 points/segment) are read.
//
// See remarks under 'AcqrsD1_readCharSequence'

ACQ_DLL ViStatus ACQ_CC AcqrsD1_readRealWform(ViSession instrumentID, ViInt32 channel,
     ViInt32 segmentNumber,        ViInt32 firstSample,        ViInt32 nbrSamples, 
    ViReal64 waveformArray[],    ViInt32* returnedSamples,    ViReal64* horPos, 
    ViReal64* sampTime,            ViInt32* timeStampLo,        ViInt32* timeStampHi);
// Returns a waveform as a floating point (double) array, with the measured data values in Volts.
//
// See remarks under 'AcqrsD1_readCharWform'

ACQ_DLL ViStatus ACQ_CC AcqrsD1_reportNbrAcquiredSegments(ViSession instrumentID, 
    ViInt32* nbrSegments); 
// Returns the number of segments already acquired.
// Can be called during the acquisition period, in order to follow the progress of a
// Sequence acquisition. 
// Can be called after an acquisition, in order to obtain the number of segments actually
// acquired (until 'AcqrsD1_stopAcquisition' was called).
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                  if one of the parameters has an invalid value, where 
//                                      'x' = parameter number.
// VI_SUCCESS                           otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_reset(ViSession instrumentID);
// Resets an instrument
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETER1                     if 'instrumentID' is invalid.
// VI_SUCCESS                              otherwise.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_resetDigitizerMemory(ViSession instrumentID);
// Resets the digitizer memory to a known default state, ONLY useful
// for a digitizer with the battery back-up option. 
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_INSTRUMENT_RUNNING    if the instrument is already running. 
// VI_ERROR_PARAMETER1                 if 'instrumentID' is invalid.
// VI_SUCCESS                          otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_restoreInternalRegisters(ViSession instrumentID,
    ViReal64 delayOffset, ViReal64 delayScale);
// Restores some internal registers of an instrument, needed ONLY after power-up
// of a digitizer with the battery back-up option.
// Please refer to the manual for a detailed description of the steps required
// to read battery backed-up waveforms.
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                  if one of the parameters has an invalid value, where 
//                                      'x' = parameter number.
// VI_SUCCESS                           otherwise.



ACQ_DLL ViStatus ACQ_CC AcqrsD1_setAttributeString(ViSession instrumentID, ViInt32 channel, 
    ViConstString name, ViConstString value); 
// Set through 'value' the value of the attribute named 'name'.
//
// 'channel'            = 1...Nchan (as returned by 'AcqrsD1_getNbrChannels' ) or
//                        0 if it is an attribute related to the instrument 
//                        itself (i.e. not to a channel).
// 'name'               = specify the name of the attribute to change.
//                        Please refer to the manual for the accepted names.
// 'value'              = specify the value in which the attribute will be set.
//                        Please refer to the manual for the accepted values.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_ATTR_NOT_FOUND         if not found or if a wrong 'channel' is specified. 
// ACQIRIS_ERROR_ATTR_WRONG_TYPE        if found but not of the expected type.
// ACQIRIS_ERROR_ATTR_INVALID_VALUE     if 'value' is not valid.
// ACQIRIS_ERROR_ATTR_IS_READ_ONLY      if found but not writable.
// VI_SUCCESS                           otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_setLEDColor(ViSession instrumentID, ViInt32 color);
// Sets the front-panel LED to the desired color
// 'color' = 0        OFF (returns to normal 'acquisition status' indicator)
//           1        Green
//           2        Red
//           3        Yellow
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETERx                  if one of the parameters has an invalid value, where 
//                                      'x' = parameter number.
// VI_SUCCESS                           otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_setSimulationOptions(ViString simOptionString);
// Sets one or several options which will be used by the function 'AcqrsD1_InitWithOptions',
// provided that the 'optionString' supplied to 'AcqrsD1_InitWithOptions' contains the
// string 'simulate=TRUE' (or similar).
// Refer to the manual for the accepted form of 'simOptionString'.
// The simulation options are reset to none by setting 'simOptionString' to an empty string "".
//
// Returns one of the following ViStatus values:
// VI_SUCCESS always.

ACQ_DLL ViStatus ACQ_CC AcqrsD1_stopAcquisition(ViSession instrumentID);
// Stops the acquisition immediately
//
// Returns one of the following ViStatus values:
// VI_ERROR_PARAMETER1                  if 'instrumentID' is invalid.
// VI_SUCCESS                           otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_stopProcessing(ViSession instrumentID);
// Stops the on-board processing immediately.(only in instruments with on-board data processing)
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_NOT_SUPPORTED      if this function is not supported by the instrument.
// VI_ERROR_PARAMETERx              if one of the parameters has an invalid value, where 
//                                  'x' = parameter number.
// VI_SUCCESS                       otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_waitForEndOfAcquisition(ViSession instrumentID, ViInt32 timeout);
// Returns after acquisition has terminated or after timeout, whichever is shorter.
// 'timeout' is in milliseconds. For protection, 'timeout' is internally clipped to a
// range of [0, 10000] milliseconds.
//
// This function puts the calling thread into 'idle' until it returns, permitting optimal 
// use of the CPU by other threads.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_OVERLOAD           if a channel/trigger overload was detected.
// ACQIRIS_ERROR_ACQ_TIMEOUT        if the acquisition timed out (and there was no overload). 
//                                  In this case, you should use either 
//                                  'AcqrsD1_stopAcquisition()' or 'AcqrsD1_forceTrig()' to 
//                                  stop the acquisition.
// ACQIRIS_ERROR_INSTRUMENT_STOPPED if the acquisition was not started beforehand
// VI_ERROR_PARAMETERx              if one of the parameters has an invalid value, where 
//                                  'x' = parameter number.
// VI_SUCCESS                       otherwise.


ACQ_DLL ViStatus ACQ_CC AcqrsD1_waitForEndOfProcessing(ViSession instrumentID, ViInt32 timeout);
// Returns after on-board processing has terminated or after timeout, whichever is shorter.
// 'timeout' is in milliseconds. For protection, 'timeout' is internally clipped to a
// range of [0, 10000] milliseconds. (only in instruments with on-board data processing)
//
// This function puts the calling thread into 'idle' until it returns, permitting optimal 
// use of the CPU by other threads.
//
// Returns one of the following ViStatus values:
// ACQIRIS_ERROR_PROC_TIMEOUT        if the processing timed out. In this case, you should use 
//                                   'AcqrsD1_stopProcessing()' to stop the processing.
// VI_ERROR_PARAMETERx               if one of the parameters has an invalid value, where 
//                                   'x' = parameter number.
// VI_SUCCESS                        otherwise.



// End of file ////////////////////////////////////////////////////////////////////////////////////
