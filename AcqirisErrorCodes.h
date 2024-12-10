//////////////////////////////////////////////////////////////////////////////////////////
//
//  AcqirisErrorCodes.h:    Acqiris Error Codes
//
//----------------------------------------------------------------------------------------
//  Copyright 2001-2005 Acqiris. All rights reserved.
//
//  $Revision: 44 $                                                $Modtime: 2.11.05 10:06 $
//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef _ACQIRIS_ERRORCODES_H
#define _ACQIRIS_ERRORCODES_H


//////////////////////////////////////////////////////////////////////////////////////////      
//    Define Instrument Specific Error Codes
//////////////////////////////////////////////////////////////////////////////////////////
#define ACQIRIS_ERROR        0xBFFA4000L      
#define ACQIRIS_WARNING      0x3FFA4000L

#define ACQIRIS_ERROR_FILE_NOT_FOUND                ACQIRIS_ERROR + 0x800
#define ACQIRIS_ERROR_PATH_NOT_FOUND                ACQIRIS_ERROR + 0x801
#define ACQIRIS_ERROR_INVALID_CHARS_IN_PATH         ACQIRIS_ERROR + 0x802
#define ACQIRIS_ERROR_INVALID_HANDLE                ACQIRIS_ERROR + 0x803
#define ACQIRIS_ERROR_NOT_SUPPORTED                 ACQIRIS_ERROR + 0x805
#define ACQIRIS_ERROR_INVALID_WINDOWS_PARAM         ACQIRIS_ERROR + 0x806 
#define ACQIRIS_ERROR_NO_DATA                       ACQIRIS_ERROR + 0x807
#define ACQIRIS_ERROR_NO_ACCESS                     ACQIRIS_ERROR + 0x808
#define ACQIRIS_ERROR_BUFFER_OVERFLOW               ACQIRIS_ERROR + 0x809
#define ACQIRIS_ERROR_BUFFER_NOT_64BITS_ALIGNED     ACQIRIS_ERROR + 0x80a

#define ACQIRIS_ERROR_ALREADY_OPEN                  ACQIRIS_ERROR + 0x840
#define ACQIRIS_ERROR_SETUP_NOT_AVAILABLE           ACQIRIS_ERROR + 0x880

#define ACQIRIS_ERROR_IO_WRITE                      ACQIRIS_ERROR + 0x8a0
#define ACQIRIS_ERROR_IO_READ                       ACQIRIS_ERROR + 0x8a1

#define ACQIRIS_ERROR_INTERNAL_DEVICENO_INVALID     ACQIRIS_ERROR + 0x8c0
#define ACQIRIS_ERROR_TOO_MANY_DEVICES              ACQIRIS_ERROR + 0x8c1
#define ACQIRIS_ERROR_EEPROM_DATA_INVALID           ACQIRIS_ERROR + 0x8c2
#define ACQIRIS_ERROR_INIT_STRING_INVALID           ACQIRIS_ERROR + 0x8c3 
#define ACQIRIS_ERROR_INSTRUMENT_NOT_FOUND          ACQIRIS_ERROR + 0x8c4 
#define ACQIRIS_ERROR_INSTRUMENT_RUNNING            ACQIRIS_ERROR + 0x8c5
#define ACQIRIS_ERROR_INSTRUMENT_STOPPED            ACQIRIS_ERROR + 0x8c6
#define ACQIRIS_ERROR_MODULES_NOT_ON_SAME_BUS       ACQIRIS_ERROR + 0x8c7
#define ACQIRIS_ERROR_NOT_ENOUGH_DEVICES            ACQIRIS_ERROR + 0x8c8
#define ACQIRIS_ERROR_NO_MASTER_DEVICE              ACQIRIS_ERROR + 0x8c9
#define ACQIRIS_ERROR_PARAM_STRING_INVALID          ACQIRIS_ERROR + 0x8ca 
#define ACQIRIS_ERROR_COULD_NOT_CALIBRATE           ACQIRIS_ERROR + 0x8cb
#define ACQIRIS_ERROR_CANNOT_READ_THIS_CHANNEL      ACQIRIS_ERROR + 0x8cc
#define ACQIRIS_ERROR_PRETRIGGER_STILL_RUNNING      ACQIRIS_ERROR + 0x8cd
#define ACQIRIS_ERROR_CALIBRATION_FAILED            ACQIRIS_ERROR + 0x8ce
#define ACQIRIS_ERROR_MODULES_NOT_CONTIGUOUS        ACQIRIS_ERROR + 0x8cf
#define ACQIRIS_ERROR_INSTRUMENT_ACQ_LOCKED         ACQIRIS_ERROR + 0x8d0
#define ACQIRIS_ERROR_INSTRUMENT_ACQ_NOT_LOCKED     ACQIRIS_ERROR + 0x8d1
#define ACQIRIS_ERROR_EEPROM2_DATA_INVALID          ACQIRIS_ERROR + 0x8d2

#define ACQIRIS_ERROR_INVALID_GEOMAP_FILE           ACQIRIS_ERROR + 0x8e0

#define ACQIRIS_ERROR_ACQ_TIMEOUT                   ACQIRIS_ERROR + 0x900
#define ACQIRIS_ERROR_TIMEOUT                       ACQIRIS_ERROR_ACQ_TIMEOUT    // For backwards compatibility
#define ACQIRIS_ERROR_OVERLOAD                      ACQIRIS_ERROR + 0x901
#define ACQIRIS_ERROR_PROC_TIMEOUT                  ACQIRIS_ERROR + 0x902
#define ACQIRIS_ERROR_LOAD_TIMEOUT                  ACQIRIS_ERROR + 0x903
#define ACQIRIS_ERROR_READ_TIMEOUT                  ACQIRIS_ERROR + 0x904
#define ACQIRIS_ERROR_INTERRUPTED                   ACQIRIS_ERROR + 0x905
#define ACQIRIS_ERROR_WAIT_TIMEOUT                  ACQIRIS_ERROR + 0x906

#define ACQIRIS_ERROR_FIRMWARE_NOT_AUTHORIZED       ACQIRIS_ERROR + 0xa00
#define ACQIRIS_ERROR_FPGA_1_LOAD                   ACQIRIS_ERROR + 0xa01
#define ACQIRIS_ERROR_FPGA_2_LOAD                   ACQIRIS_ERROR + 0xa02
#define ACQIRIS_ERROR_FPGA_3_LOAD                   ACQIRIS_ERROR + 0xa03
#define ACQIRIS_ERROR_FPGA_4_LOAD                   ACQIRIS_ERROR + 0xa04
#define ACQIRIS_ERROR_FPGA_5_LOAD                   ACQIRIS_ERROR + 0xa05
#define ACQIRIS_ERROR_FPGA_6_LOAD                   ACQIRIS_ERROR + 0xa06
#define ACQIRIS_ERROR_FPGA_7_LOAD                   ACQIRIS_ERROR + 0xa07
#define ACQIRIS_ERROR_FPGA_8_LOAD                   ACQIRIS_ERROR + 0xa08

#define ACQIRIS_ERROR_SELFCHECK_MEMORY              ACQIRIS_ERROR + 0xa20

#define ACQIRIS_ERROR_ATTR_NOT_FOUND                ACQIRIS_ERROR + 0xb00
#define ACQIRIS_ERROR_ATTR_WRONG_TYPE               ACQIRIS_ERROR + 0xb01
#define ACQIRIS_ERROR_ATTR_IS_READ_ONLY             ACQIRIS_ERROR + 0xb02
#define ACQIRIS_ERROR_ATTR_IS_WRITE_ONLY            ACQIRIS_ERROR + 0xb03
#define ACQIRIS_ERROR_ATTR_ALREADY_DEFINED          ACQIRIS_ERROR + 0xb04
#define ACQIRIS_ERROR_ATTR_IS_LOCKED                ACQIRIS_ERROR + 0xb05    
#define ACQIRIS_ERROR_ATTR_INVALID_VALUE            ACQIRIS_ERROR + 0xb06    

#define ACQIRIS_ERROR_KERNEL_VERSION                ACQIRIS_ERROR + 0xc00
#define ACQIRIS_ERROR_UNKNOWN_ERROR                 ACQIRIS_ERROR + 0xc01     
#define ACQIRIS_ERROR_OTHER_WINDOWS_ERROR           ACQIRIS_ERROR + 0xc02        
#define ACQIRIS_ERROR_VISA_DLL_NOT_FOUND            ACQIRIS_ERROR + 0xc03
#define ACQIRIS_ERROR_OUT_OF_MEMORY                 ACQIRIS_ERROR + 0xc04
#define ACQIRIS_ERROR_UNSUPPORTED_DEVICE            ACQIRIS_ERROR + 0xc05

#define ACQIRIS_ERROR_PARAMETER9                    ACQIRIS_ERROR + 0xd09
#define ACQIRIS_ERROR_PARAMETER10                   ACQIRIS_ERROR + 0xd0a
#define ACQIRIS_ERROR_PARAMETER11                   ACQIRIS_ERROR + 0xd0b
#define ACQIRIS_ERROR_PARAMETER12                   ACQIRIS_ERROR + 0xd0c
#define ACQIRIS_ERROR_PARAMETER13                   ACQIRIS_ERROR + 0xd0d
#define ACQIRIS_ERROR_PARAMETER14                   ACQIRIS_ERROR + 0xd0e
#define ACQIRIS_ERROR_PARAMETER15                   ACQIRIS_ERROR + 0xd0f

#define ACQIRIS_ERROR_NBR_SEG                       ACQIRIS_ERROR + 0xd10
#define ACQIRIS_ERROR_NBR_SAMPLE                    ACQIRIS_ERROR + 0xd11
#define ACQIRIS_ERROR_DATA_ARRAY                    ACQIRIS_ERROR + 0xd12
#define ACQIRIS_ERROR_SEG_DESC_ARRAY                ACQIRIS_ERROR + 0xd13    
#define ACQIRIS_ERROR_FIRST_SEG                     ACQIRIS_ERROR + 0xd14
#define ACQIRIS_ERROR_SEG_OFF                       ACQIRIS_ERROR + 0xd15
#define ACQIRIS_ERROR_FIRST_SAMPLE                  ACQIRIS_ERROR + 0xd16
#define ACQIRIS_ERROR_DATATYPE                      ACQIRIS_ERROR + 0xd17
#define ACQIRIS_ERROR_READMODE                      ACQIRIS_ERROR + 0xd18

#define ACQIRIS_ERROR_HW_FAILURE                    ACQIRIS_ERROR + 0xd80
#define ACQIRIS_ERROR_HW_FAILURE_CH1                ACQIRIS_ERROR + 0xd81
#define ACQIRIS_ERROR_HW_FAILURE_CH2                ACQIRIS_ERROR + 0xd82
#define ACQIRIS_ERROR_HW_FAILURE_CH3                ACQIRIS_ERROR + 0xd83
#define ACQIRIS_ERROR_HW_FAILURE_CH4                ACQIRIS_ERROR + 0xd84
#define ACQIRIS_ERROR_HW_FAILURE_CH5                ACQIRIS_ERROR + 0xd85
#define ACQIRIS_ERROR_HW_FAILURE_CH6                ACQIRIS_ERROR + 0xd86
#define ACQIRIS_ERROR_HW_FAILURE_CH7                ACQIRIS_ERROR + 0xd87
#define ACQIRIS_ERROR_HW_FAILURE_CH8                ACQIRIS_ERROR + 0xd88
#define ACQIRIS_ERROR_HW_FAILURE_EXT1               ACQIRIS_ERROR + 0xda0

#define ACQIRIS_WARN_SETUP_ADAPTED                  ACQIRIS_WARNING + 0xe00
#define ACQIRIS_WARN_READPARA_NBRSEG_ADAPTED        ACQIRIS_WARNING + 0xe10
#define ACQIRIS_WARN_READPARA_NBRSAMP_ADAPTED       ACQIRIS_WARNING + 0xe11
#define ACQIRIS_WARN_EEPROM_AND_DLL_MISMATCH        ACQIRIS_WARNING + 0xe12
#define ACQIRIS_WARN_ACTUAL_DATASIZE_ADAPTED        ACQIRIS_WARNING + 0xe13
#define ACQIRIS_WARN_UNEXPECTED_TRIGGER             ACQIRIS_WARNING + 0xe14
#define ACQIRIS_WARN_READPARA_FLAGS_ADAPTED         ACQIRIS_WARNING + 0xe15

#endif // _ACQIRIS_ERROR_CODES_H

// End of file ///////////////////////////////////////////////////////////////////////////
