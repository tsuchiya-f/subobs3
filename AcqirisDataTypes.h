//////////////////////////////////////////////////////////////////////////////////////////
//
//  AcqirisDataTypes.h:    Acqiris Readout Data Types
//
//----------------------------------------------------------------------------------------
//  Copyright 2001-2005 Acqiris. All rights reserved.
//
//  $Revision: 44 $                                                $Modtime: 23.08.05 15:51 $
//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef _ACQIRISDATATYPES_H
#define _ACQIRISDATATYPES_H

#include "vpptype.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Data Structures for '...readData' function

typedef struct 
{
    ViInt32 dataType;               //!< 0 = 8-bit (char)  1 = 16-bit (short) 2 = 32-bit (integer)  3 = 64-bit (real)
                                    //!< or use "enum ReadType" defined below
    ViInt32 readMode;               //!< use "enum AqReadDataMode" defined below
    ViInt32 firstSegment;
    ViInt32 nbrSegments;
    ViInt32 firstSampleInSeg;
    ViInt32 nbrSamplesInSeg;
    ViInt32 segmentOffset;
    ViInt32 dataArraySize;          //!< in bytes            
    ViInt32 segDescArraySize;       //!< in bytes            
    ViInt32 flags;                  //!< use "enum AqReadDataFlags" defined below
    ViInt32 reserved;
    ViReal64 reserved2;
    ViReal64 reserved3;
} AqReadParameters;

typedef struct
{
    ViReal64 horPos;
    ViUInt32 timeStampLo;           //!< Timestamp 
    ViUInt32 timeStampHi;
} AqSegmentDescriptor;

typedef struct
{
    ViReal64 horPos;
    ViUInt32 timeStampLo;           //!< Timestamp 
    ViUInt32 timeStampHi;
    ViUInt32 actualTriggersInSeg;   //!< Number of actual triggers acquired in this segment                
    ViInt32  avgOvfl;               //!< Acquisition overflow (avg)
    ViInt32  avgStatus;             //!< Average status (avg)
    ViInt32  avgMax;                //!< Max value in the sequence (avg)    
    ViReal64 reserved1;
} AqSegmentDescriptorAvg;

//! For backward compatibility.
#define addrFirstPoint indexFirstPoint

typedef struct
{
    ViInt32     returnedSamplesPerSeg;
    ViInt32     indexFirstPoint;    //!< "data[desc.indexFirstPoint]" is the first valid point. 
    ViReal64    sampTime;
    ViReal64    vGain;
    ViReal64    vOffset;
    ViInt32     returnedSegments;   //!< When reading multiple segments in one waveform
    ViInt32     nbrAvgWforms;        
    ViUInt32    actualTriggersInAcqLo;
    ViUInt32    actualTriggersInAcqHi;
    ViUInt32    actualDataSize;
    ViInt32     reserved2;    
    ViReal64    reserved3;
} AqDataDescriptor;


typedef struct     
{
    ViInt32 GatePos;
    ViInt32 GateLength;
} AqGateParameters;


enum AqReadType { ReadInt8 = 0, ReadInt16, ReadInt32, ReadReal64, ReadRawData };    

enum AqReadDataMode 
{
    ReadModeStdW = 0,   //!< standard waveform
    ReadModeSeqW,       //!< sequenced waveform
    ReadModeAvgW,       //!< averaged waveform
    ReadModeGateW,      //!< gated waveform
    ReadModePeak,       //!< peaks of a waveform 
    ReadModeShAvgW,     //!< standard short averaged
    ReadModeSShAvgW,    //!< short shifted averaged waveform 
    ReadModeSSRW,       //!< sustained sequential recording
    ReadModeZsW,        //!< zero suppressed waveform
    ReadModeHistogram,  //!< histogram
    ReadModePeakPic,    //!< peak picture
    nbrDataCodings 
};

enum AqReadDataSetupTypes 
{    
    AvgGate = 0,        //!< gate parameters
    SSR_Default, 
    CodingTable,        //!< coding table e.g. for huffman coding
    FilterCoefficient,  //!< Filter coefficients
    AlgoCoefficient ,   //!< peak detection
    nbrSetupTypes 
};

enum AqReadDataFlags 
{
    AqIgnoreTDC          = 0x0001, //!< If set, the TDC value (if any) will be ignored.
    AqIgnoreLookUp       = 0x0002, //!< If set, the lookup table will not be applied on data.
    AqSkipClearHistogram = 0x0004, //!< If set, the histogram will be not be zero-ed during read
    AqSkipCircular2Linear= 0x0008, //!< If set, the memory image will be transferred in ReadModeSeqW
};

//!BeginAcqiris!
//////////////////////////////////////////////////////////////////////////////////////////
//  AcqrsT3Interface structure definitions

typedef struct
{
    ViAddr dataArray;           //!< pointer to user allocated memory
    ViUInt32 dataSizeInBytes;   //!< size of user allocated memory in bytes
    ViInt32 nbrSamples;         //!< number of samples requested
    ViInt32 dataType;           //!< use "enum AqReadType" defined above
    ViInt32 readMode;           //!< use "enum AqT3ReadModes" defined below
    ViInt32 nbrChannelDesc;     //!< Number of ChannelDescriptor allocated in channelDesc
    ViInt32 reserved2;          //!< reserved, must be 0
    ViInt32 reserved1;          //!< reserved, must be 0
} AqT3ReadParameters;


typedef struct
{
    ViAddr dataPtr;             //!< pointer to returned data (same as user buffer if aligned)
    ViInt32 nbrSamples;         //!< number of samples returned
    ViInt32 sampleSize;         //!< size in bytes of one data sample
    ViInt32 sampleType;         //!< type of the returned samples, see AqT3SampleType below
    ViInt32 reserved4;          //!< reserved, will be 0
    ViInt32 reserved3;          //!< reserved, will be 0
    ViInt32 reserved2;          //!< reserved, will be 0
    ViInt32 reserved1;          //!< reserved, will be 0
} AqT3DataDescriptor;


typedef struct
{
    ViAddr dataPtr;             //!< pointer to returned data for this channel
    ViInt32 nbrSamples;         //!< number of samples returned
    ViInt32 reserved2;          //!< reserved, must be 0
    ViInt32 reserved1;          //!< reserved, must be 0
} AqT3ChannelDescriptor;


enum AqT3ReadModes
{
    AqT3ReadStandard,           //!< Standard read mode
    AqT3ReadContinuous,         //!< Continuous read mode
};


enum AqT3SampleType
{
    AqT3SecondReal64,           //!< double value in seconds
    AqT3Count50psInt32,         //!< count of 50 ps
    AqT3Count5psInt32,          //!< count of 5 ps
    AqT3Struct50ps6ch,          //!< struct on 32 bits with (LSB to MSB)
                                //!<  27 bits count of 50 ps, 3 bits channel number, 1 bit overflow
};

//!EndAcqiris!


#endif // _ACQIRISDATATYPES_H

// End of file ///////////////////////////////////////////////////////////////////////////
