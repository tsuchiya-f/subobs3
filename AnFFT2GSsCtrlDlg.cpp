#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "AcqirisD1Import.h"

const double pi=3.1415926535;
enum AcqMode	{AcqStop,AcqSingle,AcqAuto,AcqCont,nbrAcqModes};
enum DShift	{ShiftNone,ShiftBy4,ShiftBy8,ShiftBy12,nbrShifts};
enum MonAccMode	{MonNormal,MonBkgrnd,MonAccrue,nbrMonAccModes};
enum MonRunMode	{MonStop,MonSingle,MonRepeat,nbrMonRunModes};
enum RdMode	{ReadLo32,ReadHi32,Read36,nbrReadModes};
enum SpectRead	{SpectNorm,SpectBkgd,SpectClr,nbrSpectReads};
enum Window	{WinNone,WinHanning,WinHamming,	WinFlattop,WinBlackmann,nbrWindows};
enum ZoomFctr	{Zoom1,Zoom2,Zoom5,Zoom10,Zoom20,Zoom50,Zoom100,nbrZooms};
enum Constants	{
	Write=1,
	Read=0,
	nbrBytesInString= 256,		//Includes '\0' at end of string!
	nbrSpectralLines=16*1024,	//This application ALWAYS yields 16K spectral lines!
	maxADCSamples=100000,		//<= 100K samples for normal acquisition
};
enum FPGAregisters	{
//Registers in region "Acqiris reserved"
//These registers are common to all FPGA designs
	ReadAddrReg=0,			//Block readout register
	StartAddrReg=1,			//Start address within block
	IndirAddrReg=2,			//Block address (indirect addressing register)
	FPGACtrlReg=3,			//FPGA control register
	CodeProtectReg=4,		//FPGA code protection register
	FPGAStatusReg=6,		//FPGA status register
	TemperatureReg=7,		//Temperature register
	DECtrlReg=8,			//DE-bus control register (from MAC)
//Registers in region "User"
	MainCtrlReg=64,			//Main Control for FFT-Analyzer
	StatusReg=65,			//Status Register for FFT-Analyzer
	NbrAccReg=66,			//Number of Accumulations (Counts 'dual' spectra, 1 --> 2 spectra etc.)
	FFTConfReg=67,			//FFT Configuration register
	FPuDB_IOCtrlReg=68,		//Control register for front panel uDB IO-Connector
	FFTclearBufReg=69,		//Accumulator Buffer clear (uses only bit 0)
	FFToverflowReg=70,		//Overflow Status register
//Indirect address buffers
	SumOfPowerSpectrum=0x81,	//Indirect Address to summed FFT power spectrum
	FFTWindowBuffer=0x82,		//Indirect Address to FFT Window
};
typedef union	{
//User Register 65: Status Register (all bits RO)
	long value;
	struct	{
		unsigned sftEnabled:1;	//1=core was enabled through software (Main Control Register, bit 0)
		unsigned uDBenabled:1;	//1=core was enabled through frontpanel uDB input 0
		unsigned enabled:1;	//0=core is disabled	1=core is enabled
		unsigned :1;		//unused
		unsigned running:1;	//0=not acquiring data	1=acquiring data
		unsigned :3;		//unused 
//-------------------------------------
		unsigned dataA:1;	//0=input channel A is off	1=input channel A delivers data
		unsigned dataB:1;	//0=input channel B is off	1=input channel B delivers data
		unsigned trigMode:1;	//0=trigger mode off		1=trigger mode on
		unsigned triggered:1;	//0=no trigger received		1=trigger received
		unsigned :4;		//unused  
//-------------------------------------
		unsigned busy:1;	//0=core not busy		1=core is busy
		unsigned :14;		//unused 
		unsigned full:1;	//0=buffer not full		1=buffer is full
	} b;
} MainStatus;
static const char* dataShiftTxt[nbrShifts]={"No Shift","Down by 4 bits","Down by 8 bits","Down by 12 bits"};
static const char* windowTypeTxt[nbrWindows]={"None","Hanning","Hamming","Flattop","Blackmann"};
static const long zoomFactor[nbrZooms]={1,2,5,10,20,50,100};
static const char* zoomText[nbrZooms]={" x 1"," x 2"," x 5"," x 10"," x 20"," x 50"," x 100"};
char	m_adcBuf[maxADCSamples];		//ADC data, for normal acquisitions
long	m_spectrum[2*nbrSpectralLines];		//Summed power spectrum, as read from FPGA
double	m_nrmSpectrum[nbrSpectralLines];	//Summed power spectrum, renormalized for data shift and readmode 
double	m_accSpectrum[nbrSpectralLines];	//Accrued  power spectrum, = sum of several "m_nrmSpectrum"
double	m_bkgSpectrum[nbrSpectralLines];	//Background power spectrum, normalized like 'm_nrmSpectrum'
double	m_dspSpectrum[100][nbrSpectralLines];	//Display power spectrum, normalized to a single FFT and the 
long	m_nbrSamplesRead;
double	m_sampIval=0.5e-9;		//m_sampIval=2.5e-9; // for low-reso mode
double	m_temperatureBoard=0.0;
double	m_temperatureFPGA=0.0;
AcqMode	m_acqMode=AcqStop;
AcqMode m_acqPrev=AcqStop;		//m_timePerSpect= 0.000016384*5; // for low-reso mode
double	m_timePerSpect=0.000016384;	//Time (in sec) to accumulate a single FFT
/////////////////////////////////////////////////////////////////////////////
double	m_averageTime=0.01;		//Time (in sec) per full average (at 2 GS/s)
/////////////////////////////////////////////////////////////////////////////
ZoomFctr m_hZoomFctr=Zoom1;	
bool	m_hposScrollActive=false;
MonAccMode	m_monAccMode=MonNormal;
MonRunMode	m_monRunMode=MonRepeat;
RdMode	m_readMode=Read36	;		//ALWAYS read all 36 bits
DShift	m_loadedDShift=ShiftNone;
DShift	m_wantedDShift=ShiftNone;
Window	m_wantedWindow=WinBlackmann;		//WinNone;WinHanning;WinHamming;WinBlackmann;WinFlattop
bool	m_accrueClr=false;
bool	m_newBkGrnd=false;
bool	m_newParams=false;		//New parameters for FPGA ==> stop and restart FPGA operation
bool	m_newDataBlock=false;
long	m_nbrSpectralLines=nbrSpectralLines;
long	m_nbrAccruedSpectra=0;
bool	m_adcOvfl=false;
bool	m_accOvfl=false;
ViSession m_ID;
long	m_actChan=1L;
//Number of Spectra in an accumulation (must be even!)
long m_nbrSpectra=((long)(m_averageTime/m_timePerSpect))&0xfffffffe;	//Even number!
//AnInstrument&	m_instr;
long	m_nbrAcqChannels;					//Number of input channels in this instrument
void	AcquireReadWform();
void	ControlDataAcquisition(); 
double	ConvertTodB(double linearValue);
void	FPGAInit();
void	FPGAReset();
long	FPGARead(long regID,  long nbrValues, long* dataArrayP);
long	FPGAWrite(long regID, long nbrValues, long* dataArrayP);
void	GetNewBackGround();
bool	GetRawSpectrum(bool* overflowChangedP);
bool	GetSimulatedRawSpectrum();
bool	GetSpectrum(bool* overflowChangedP, SpectRead whatFor);	//returns 'true', if new spectrum was retrieved
void	Init();
void	LoadFFTWindow();
void	ModAvgTime(long delta);
void	ModZoomHFactor(long delta);
void	ModZoomHPos(long delta);
void	ReadTemperature(double Temp[]);
void	ShowMonGroupEnabled(bool enable);
void	ShowZoomGroup(bool show);
void	StartContAcquisition();
void	StopContAcquisition();
bool	SumOfSpectraReady();
void	EndAll();
void	ReadTime(FILE *fp);
bool	redraw=false;
long ovflows = 0L;
FILE	*fst;
//EndAll no use
void EndAll()
{
	StopContAcquisition();
	AcqrsD1_closeAll();
}
//Acquisition of 'normal' waveform, for digitizer adjustment
void AcquireReadWform()
{
	ViStatus status;
	long nbrSamples,nbrSegments;
//Always make sure to be in 'normal' acquisition mode (NOT continuous)
	AcqrsD1_configMode(m_ID,0,0,0);
	AcqrsD1_getMemory(m_ID,&nbrSamples,&nbrSegments);
	if(nbrSamples<1L)nbrSamples=1L;
	if(nbrSamples>(long)maxADCSamples)nbrSamples=(long)maxADCSamples;
	AcqrsD1_acquire(m_ID);
	status=AcqrsD1_waitForEndOfAcquisition(m_ID,20);
	if(status==ACQIRIS_ERROR_ACQ_TIMEOUT)AcqrsD1_stopAcquisition(m_ID);
// Read the waveform
	AqReadParameters readPar;
	AqDataDescriptor dataDesc;
	AqSegmentDescriptor seqDesc;
	readPar.dataType=ReadInt8;
	readPar.readMode=ReadModeStdW;
	readPar.firstSegment=0;
	readPar.nbrSegments=1;
	readPar.firstSampleInSeg=0;
	readPar.nbrSamplesInSeg=nbrSamples;
	readPar.segmentOffset=0;
	readPar.dataArraySize=maxADCSamples;
	readPar.segDescArraySize=sizeof(seqDesc);
	status=AcqrsD1_readData(m_ID,m_actChan,&readPar,m_adcBuf,&dataDesc,&seqDesc);
	fprintf(fst,"%d\n",status);
	m_nbrSamplesRead=nbrSamples;
}
//ControlDataAcquisition
void ControlDataAcquisition() 
{
	bool redraw=false,timeout=false;
	AcqrsD1_calibrate(m_ID);
	if((m_acqPrev==AcqCont)&&(m_acqMode!=AcqCont)){
		StopContAcquisition();
		m_adcOvfl=false;
		m_accOvfl=false;
	}
	switch(m_acqMode){
		case AcqCont:	if (m_acqMode!=m_acqPrev)StartContAcquisition();
				else if (m_newParams){
					StopContAcquisition();
					StartContAcquisition();
					m_accrueClr=true;		//Clear accumulated data because parameters changed
					m_newParams=false;
					GetSpectrum(&redraw,SpectClr);	//Clean out the (partially) accumulated read-buffer
				}
				switch(m_monRunMode){
					case MonStop:break;
					case MonSingle:	m_newDataBlock=GetSpectrum(&redraw,SpectNorm);
							if(m_newDataBlock){
								m_monRunMode=MonStop;
								redraw=true;
							}
							break;
					case MonRepeat:	m_newDataBlock=GetSpectrum(&redraw,SpectNorm);
							break;
				}
				break;
	}
	m_acqPrev = m_acqMode;
}
//Convert to "dBc full scale", i.e. 0 dBc corresponds to a full scale input signal (DC)
//If an "underflow" DC signal is applied to the input, all ADC values are - 128. In this case,
//the observed DC component of the FFT is 4'246'732'800. Thus, we subtract 10log10(4'246'...) = 96.28.
double ConvertTodB(double linearValue)
{
	if(linearValue>0.0)return(-96.28+10.0*log10(linearValue+1.0));
	else return -100.0;
}
//FPGAInit
void FPGAInit()
{
	ViStatus stat;
	long mainStatus;
	LoadFFTWindow();
//Start the DE interface
	long deCtrl=0;
	stat=FPGAWrite(DECtrlReg,1,&deCtrl);		//Write control values
//	fprintf(fst,"FPGAInit stat=%d\n",stat);
	deCtrl=0x80000000;
	stat=FPGAWrite(DECtrlReg, 1, &deCtrl);		//Need a SEPARATE write to start DE
//	fprintf(fst,"FPGAInit stat=%d\n",stat);
	long fpgaCtrl=0;
	stat=FPGAWrite(FPGACtrlReg,1,&fpgaCtrl);
//	fprintf(fst,"FPGAInit stat=%d\n",stat);
	long nbrDoubleBlocks=m_nbrSpectra/2;
	stat=FPGAWrite(NbrAccReg,1,&nbrDoubleBlocks);
//	fprintf(fst,"FPGAInit stat=%d\n",stat);
//	fprintf(fst,"nbrDoubleBlocks=%ld\n",nbrDoubleBlocks);
//Configure FFT
	long fftConf=0;
	fftConf|=(0x2<<18);
//	fprintf(fst,"%x\n",fftConf);
	stat=FPGAWrite(FFTConfReg,1,&fftConf);
//	fprintf(fst,"FPGAInit stat=%d\n",stat);
	Sleep(1000);
//Enable the FPGA-core
	long mainCtrl=1;
	stat=FPGAWrite(MainCtrlReg,1,&mainCtrl);
//	fprintf(fst,"FPGAInit stat=%d\n",stat);
}
//FPGAReset
void FPGAReset()
{
	long mainCtrl=0x0;				//Disable FFT computation
	ViStatus stat = FPGAWrite(MainCtrlReg, 1, &mainCtrl);
//	fprintf(fst,"FPGA stat=%d\n",stat);
	Sleep(10);					//Wait until FFT terminated (check a bit?)
	long deCtrl = 0x0;				//Stop the DE interface in the FPGA
	stat=FPGAWrite(DECtrlReg,1,&deCtrl);
//	fprintf(fst,"FPGA stat=%d\n",stat);
	long fpgaCtrl=0x0;				//Reset FPGA control
	stat=FPGAWrite(FPGACtrlReg,1,&fpgaCtrl);
//	fprintf(fst,"FPGA stat=%d\n",stat);
}
//FPGARead
long FPGARead(long regID,long nbrValues,long* dataArrayP)
{
	return AcqrsD1_logicDeviceIO(m_ID,"Block1Dev1",regID,nbrValues,dataArrayP,Read,0);
}
//FAGAWrite
long FPGAWrite(long regID,long nbrValues,long* dataArrayP)
{
	return AcqrsD1_logicDeviceIO(m_ID,"Block1Dev1",regID,nbrValues,dataArrayP,Write,0);
}
//Get Raw
bool GetRawSpectrum(bool* overflowChangedP)
{
	*overflowChangedP=false;
	ViStatus stat;
//wait until Accumulated Spectra is ready
	bool full= false;
	long mainStatus;
	while(!full){
		stat=FPGARead(StatusReg,1,&mainStatus);
//		fprintf(fst,"GetRawSpec mainStatus=%x\n",mainStatus);
		if((mainStatus&0x80000000)!=0){
			full=true;	
//			fprintf(fst,"GetRawSpec stat=%d\n",stat);
//			fprintf(fst,"GetRawSpec mainStatus=%ld\n",mainStatus);
		}
	}
//Read the accumulated power spectrum
	long startAddr=0x0;
	long bufAddress=SumOfPowerSpectrum;
	stat=FPGAWrite(StartAddrReg,1,&startAddr);
//	fprintf(fst,"GetRawSpec stat=%d\n",stat);
	stat=FPGAWrite(IndirAddrReg,1,&bufAddress);
//	fprintf(fst,"GetRawSpec stat=%d\n",stat);
	if(m_readMode==Read36){
		stat=FPGARead(ReadAddrReg,2*nbrSpectralLines,m_spectrum);
//		fprintf(fst,"GetRawSpec If stat=%d\n",stat);
	}
	else{
		stat=FPGARead(ReadAddrReg,nbrSpectralLines,m_spectrum);
//		fprintf(fst,"GetRawSpec Else stat=%d\n",stat);
	}
	m_nbrSpectralLines=nbrSpectralLines;
//Check if there were overflows // 6.2.7
//	long ovflows = 0L;
	stat=FPGARead(FFToverflowReg,1,&ovflows);
//	fprintf(fst,"GetRawSpec overflow stat=%d\n",stat);

//*********************************************************************
//	overflowをつかさどる重要な行
//	変更時は必ずログを残すこと
//	if(ovflows!=0)fprintf(fst,"#overflow stat=%x\n",ovflows);
//	if(ovflows!=0){
//		printf("OverFlow");
//		for(int i=0;i<nbrSpectralLines;i++){
//			int(m_spectrum) = -100;
//		}
//	}
//*********************************************************************


	return true;					//New Accumulated Spectra ready
}
//Get Spectrum
bool GetSpectrum(bool* overflowChangedP,SpectRead whatFor)
{
	long i;
	int fileno;
	FILE *fpt;
	char fnmt[100];
	clock_t start,time;

	fileno = 0;

/*	sprintf(fnmt,"test_%d.txt",fileno);
	fpt=fopen(fnmt,"w+");
	start=clock();
	fprintf(fpt,"time: %f\n",(double)start);
*/	
//Get raw accumulated spectrum
	if(!GetRawSpectrum(overflowChangedP))return false;	//New sum is not yet ready
	
	//	time=clock();
//	fprintf(fpt,"time: %f\n",(double)time);
	
//Convert to normalized raw spectrum 'm_nrmSpectrum', i.e. corrected for data shift and readout mode
//Renormalize the data ALWAYS to the FFT output range
	double renormFactor;
	switch(m_loadedDShift){
		case ShiftNone:	renormFactor=1.0;
				break;
		case ShiftBy4:	renormFactor=16.0;
				break;
		case ShiftBy8:	renormFactor=256.0;
				break;
		case ShiftBy12:	renormFactor=4096.0;
				break;
	}
	for(i=0;i<m_nbrSpectralLines;i++){
		switch(m_readMode){
			case ReadLo32:	m_nrmSpectrum[i]=(double)((unsigned long)m_spectrum[i])*renormFactor;
					break;
			case ReadHi32:	m_nrmSpectrum[i]=(double)((unsigned long)m_spectrum[i])*renormFactor*16.0;
					break;
			case Read36:{
					double hi=16.0*(double)((unsigned long)m_spectrum[2*i]);	//upper 32 bits (of 36)
					double lo=(double)(m_spectrum[2*i + 1]&0xf);			//lower 32 bits (use only 4 lowest!)
					m_nrmSpectrum[i]=(hi + lo)*renormFactor;
			}
		}
	}
//	time=clock();
//	fprintf(fpt,"time: %f\n",(double)time);

//The averaged 'final' power spectrum 'm_dspSpectrum' is normalized to a SINGLE power spectrum
//(16 us data) as it appears at the 36-bit output of the FFT core.
	m_nbrAccruedSpectra+=m_nbrSpectra;
//	m_nbrAccruedSpectra = 100;
	for(i=0;i<m_nbrSpectralLines;i++)m_accSpectrum[i]+=m_nrmSpectrum[i];
	
//	fclose(fpt);
	
	return true;
}
//Init
void Init()
{
	char m_name[32];
	long m_serialNbr;
	long m_busNbr;
	long m_slotNbr;
	long m_nbrPhysicalLinks;
	long m_nbrMBofMemory;
	long aaa;
aaa=VI_FALSE;
//section 6.1.3
	ViStatus stat=AcqrsD1_InitWithOptions("PCI::INSTR0",VI_FALSE,VI_FALSE,"",&m_ID);
//	fprintf(fst,"6.1.3. stat=%d\n",stat);
printf("Init 6.1.3. stat=%d\n",stat);

//section 6.1.4   important!!
	stat=AcqrsD1_configLogicDevice(m_ID,"Block1Dev1",NULL,1);
//	fprintf(fst,"6.1.4 clear stat=%d\n",stat);
	stat=AcqrsD1_configLogicDevice(m_ID,"Block1Dev1","AC240FFT2GSs.bit",0);
printf("BitLoad stat=%d\n",stat);
//	fprintf(fst,"6.1.4 stat 24bit=%d\n",stat);
	stat=AcqrsD1_configMode(m_ID,0,0,0);			//Configure for normal acquisition
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	stat=AcqrsD1_configExtClock(m_ID,0,0.0,0,0.0,0.0);	//with internal clock
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	stat=AcqrsD1_calibrate(m_ID);
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
//section 6.1.6
 	long fullscale=1.0;
	stat=AcqrsD1_configMode(m_ID,1,0,0);			//Set the mode to 'Streaming to DPU' ( = 1)
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	stat=AcqrsD1_configChannelCombination(m_ID,2,0x1);
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	stat=AcqrsD1_configHorizontal(m_ID,m_sampIval,0.0);
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	stat=AcqrsD1_configVertical(m_ID,1,fullscale,0.0,3, 0);
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	stat=AcqrsD1_configExtClock(m_ID,0,0.0,0,0.0,0.0);	//with internal clock
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	stat=AcqrsD1_calibrate(m_ID);
//	fprintf(fst,"6.1.4 stat=%d\n",stat);
	AcqrsD1_getNbrChannels(m_ID,&m_nbrAcqChannels);
	long value=0x84600000;					//enable monitoring bit of FPGA temperature sensor
	FPGAWrite(TemperatureReg,1,&value);
}
//LoadFFTWindow
void LoadFFTWindow()
{
	long i;
	double window[nbrSpectralLines];
	for(i=0;i<nbrSpectralLines;i++){
		double w=pi*i/nbrSpectralLines;
		double cos1=cos(w);
		double cos2=cos(2.0*w);
		switch(m_wantedWindow){
			case WinNone:	window[i]=1.0;
					break;
			case WinHanning:window[i]=0.5-0.5*cos1;
					break;
			case WinHamming:window[i]=0.54-0.46*cos1;
					break;
			case WinFlattop:window[i]=0.28106-0.5209*cos1+0.19804*cos2;
				 	break;
			case WinBlackmann:window[i]=0.42-0.5*cos1+0.08*cos2;
					break;
		}
	}
	long windowFunction[nbrSpectralLines/2];
	for(i=0;i<nbrSpectralLines/2;i++){
		long winEven=window[i*2]*256;
		long winOdd=window[i*2+1]*256;
		if(winEven<-256)winEven=-256;
		if(winEven>255)winEven=255;
		if(winOdd<-256)winOdd=-256;
		if(winOdd>255)winOdd=255;
		windowFunction[i]=((winEven&0x1ff)<<9)+(winOdd&0x1ff);
	}
	long startAddr=0x0;
	long bufAddress=FFTWindowBuffer;
	FPGAWrite(StartAddrReg,1,&startAddr);
	FPGAWrite(IndirAddrReg,1,&bufAddress);
	FPGAWrite(ReadAddrReg,nbrSpectralLines/2,windowFunction);
}
//ReadTemperature
void ReadTemperature(double Temp[])
{
	long tReg;
	ViStatus stat=FPGARead(TemperatureReg,1,&tReg);
//	fprintf(fst,"read Temp stat=%d\n",stat);
//	fprintf(fst,"tReg=%ld\n",tReg);
	if((tReg&0x8000)==0){
		tReg|=0x8000;
		stat=FPGAWrite(TemperatureReg,1,&tReg);
//		fprintf(fst,"read Temp stat loop1=%d\n",stat);
//		fprintf(fst,"tReg=%ld\n",tReg);
		stat=FPGARead(TemperatureReg,1,&tReg);
//		fprintf(fst,"read Temp stat loop2=%d\n",stat);
//		fprintf(fst,"tReg=%ld\n",tReg);
	}
//	fprintf(fst,"tReg=%ld\n",(tReg&0x1fff));
//	fprintf(fst,"tReg=%ld\n",(tReg&0x1fff)-0x1000);
//	long tValue=(tReg&0x1fff)-0x1000;
	long tValue=tReg&0x1fff;
//	fprintf(fst,"Temp=%ld\n",(long)(tValue*0.0625));
	Temp[0]=tValue*0.0625;
	long temp;
	stat=AcqrsD1_getInstrumentInfo(m_ID,"Temperature",&temp);
//	fprintf(fst,"read Temp stat=%d\n",stat);
	Temp[1]=(double)temp;
}
//StartContAcquisition
void StartContAcquisition()
{
	AcqrsD1_stopAcquisition(m_ID);		// Just in case
	AcqrsD1_configMode(m_ID,1,0,0);		// Set the mode to 'Streaming to DPU' ( = 1)
	AcqrsD1_acquire(m_ID);
	FPGAInit();				// MUST be called after Acquisition Start!
}
//StopContAcquisition
void StopContAcquisition() 
{
	FPGAReset();				// MUST be called before Acquisition Stop!
	AcqrsD1_stopAcquisition(m_ID);
	AcqrsD1_configMode(m_ID,0,0,0);		// Set the mode to 'Normal' ( = 0) 
}
//SumOfSpectraReady no use
bool SumOfSpectraReady()
{
	MainStatus status;
	FPGARead(StatusReg,1,&status.value);
	return (status.b.full==1 ? true : false);
}
//ReadTIme
void ReadTime(FILE *fp)
{
	time_t tdat;
	struct tm *timeptr;
	tdat=time(NULL);
	timeptr=localtime(&tdat);
	fprintf(fp,"#%s",asctime(timeptr));
}

//------ main ------
int main()
{
	FILE *fp,*flag,*fp2,*fp3,*ptr,*ptr2;
	long i,j,k;
	char fnm[100],fnm2[100],fnm2_1[100],fnm3[100];
	int loop=1,addnum=1,task,num=1,loadadd=1,perfile=12000,loop2=1,of1024, fino;
	int yy,mm,dd,st_hr,st_mi,st_sec,ed_hr,ed_mi,ed_sec;
	double Temp[2],buff[16384],QLdata[1024],nodata[1024];
	char ope[20],ope_1[20],sch[20],sch_1[20];
	clock_t start,start2,start3,end;
	char cmd[100];

	time_t st_timer, ed_timer, present;
    struct tm *now;
	struct tm str_time;
	struct tm ed_time;
	double from_str, from_ed;

	printf("Initializing FFT ....");
	Init();
	printf(".....");
	ViStatus stat=AcqrsD1_acquire(m_ID);
	StopContAcquisition();
	StartContAcquisition();
	printf("FFT START 11\n");

while(1){			//この無限ループは毎日観測するぞという意気込みの表れ

	for(i=0;i<nbrSpectralLines;i++){
		m_accSpectrum[i]=0.0;
		for(j=0;j<100;j++){
			m_dspSpectrum[j][i]=0.0;
		}
	}
	for(i=0;i<1024;i++){
		QLdata[i] = 0.0;
		nodata[i] = 0.0;
	}
	m_nbrAccruedSpectra=0;
	printf("Execute read_sch\n");
	system("\"read_sch.exe\"");	//スケジュールファイルを読む　ないときは待ち続ける

///////////////////////////////////////////////////////////////////////
//スタートストップ時刻をファイルから呼んでくる
    present = time(NULL); //+ 60*60*8; // JST補正 (なぜか8h)
    now = localtime(&present);
	printf("  current time %02d:%02d:%02d (%d)\n", now->tm_hour, now->tm_min, now->tm_sec, present);

	yy = now->tm_year+1900;
	mm = now->tm_mon+1;
	dd = now->tm_mday;

	sprintf(ope,"ope_aq_%d%02d%02d.txt",yy,mm,dd);
	printf("..reading %s\n",ope);
	ptr = fopen(ope, "r");
	if (ptr == NULL){
		puts("  File Open Error: no ope_aq");
		return 0;
	}
	fscanf(ptr, "%02d%02d%02d", &st_hr,&st_mi,&st_sec);
	printf("  start time %02d:%02d:%02d\n", st_hr,st_mi,st_sec);
	fscanf(ptr, "%02d%02d%02d", &ed_hr,&ed_mi,&ed_sec);
	printf("  end time %02d:%02d:%02d\n", ed_hr,ed_mi,ed_sec);
	fclose(ptr);
/////////////////////////////////////////////////////////////////////
//スタート時刻になるまで待つ
// 観測開始時刻を構造体に渡す
    str_time.tm_sec = st_sec;       // 秒 
    str_time.tm_min = st_mi;       // 分 
    str_time.tm_hour = st_hr;      // 時 
    str_time.tm_mday = dd;      //日 
    str_time.tm_mon = mm - 1;       // 月 ( 1月＝0 ) 
    str_time.tm_year = yy - 1900;    // 西暦年 - 1900 
    str_time.tm_isdst = -1;    // サマータイムフラグ 
// 西暦 1970 年1月1日から観測開始時刻までの経過秒 
    st_timer = mktime(&str_time);
	//printf("%d\n", st_timer);

// 観測終了時刻を構造体に渡す
    ed_time.tm_sec = ed_sec;       // 秒 
    ed_time.tm_min = ed_mi;       // 分 
    ed_time.tm_hour =ed_hr;      // 時 
    ed_time.tm_mday = dd;      //日 
    ed_time.tm_mon = mm - 1;       // 月 ( 1月＝0 ) 
    ed_time.tm_year = yy - 1900;    // 西暦年 - 1900 
    ed_time.tm_isdst = -1;    // サマータイムフラグ 
// 西暦 1970 年1月1日から観測終了時刻までの経過秒 
    ed_timer = mktime(&ed_time);

	while(1){
		//西暦 1970 年1月1日から現在時刻までの経過秒数 
		present = time(NULL); //+ 60*60*8; // JST補正 (なぜか8h)
		now = localtime(&present);
		printf("  current time %02d:%02d:%02d (%d)\n", now->tm_hour, now->tm_min, now->tm_sec, present);
	    printf("  start   time %02d:%02d:%02d (%d)\n", st_hr,st_mi,st_sec, st_timer);
	    printf("  ed      time %02d:%02d:%02d (%d)\n", ed_hr,ed_mi,ed_sec, ed_timer);
		// 観測開始時刻と現在時刻の差 
		from_str = difftime(present, st_timer);
//		printf("  present    %d\n",present);
//		printf("  start      %d\n",st_timer);
//		printf("  defference %f\n",from_str);
		// 現在時刻が正なら計測開始
		if(from_str > 0){
			printf("Observation start\n",from_str);
//			printf("%8.0lf sec from start time\n",from_str);
			break;
		}
		Sleep(10000);
		printf("\n");
	}
/////////////////////////////////////////////////////////////////////

	//sch to old_sch
	sprintf(sch,"SUN_%d%02d%02d.sch",yy,mm,dd);
	sprintf(sch_1,"old_SUN_%d%02d%02d.sch",yy,mm,dd);
	rename(sch,sch_1);

	system("\"mail\\send_mail.exe\"");	//観測開始通知

	sprintf(fnm2,"%d%02d%02d_QLcheck.txt",yy,mm,dd);
	sprintf(fnm3,"%d%02d%02d_head.txt",yy,mm,dd);
	fp2=fopen(fnm2,"wb+");

	sprintf(ope,"Qlhead_%d%02d%02d.txt",yy,mm,dd);
	ptr2 = fopen(ope, "w");
	present = time(NULL); //+ 60*60*8;
	now = localtime(&present);
	fprintf(ptr2, "%d\n", now->tm_hour);
	fprintf(ptr2, "%d\n", now->tm_min);
	fprintf(ptr2, "%d\n", now->tm_sec);
	start = clock();
	fino = 0;
	fclose(ptr2);

	while(1){
		sprintf(fnm,"%d%02d%02d_%d.txt",yy,mm,dd,fino);
		fp=fopen(fnm,"wb+");
		for(int kazu=0;kazu<perfile;kazu++){
			ReadTemperature(Temp);
			for(j=0;j<loop2;j++){
				of1024=0;
				for(int ave=0;ave<1;ave++){
					m_newDataBlock=GetSpectrum(&redraw,SpectNorm);
					if(ovflows != 0){
						of1024=1;
					}
				}
				for(i=0;i<m_nbrSpectralLines;i++){
					m_dspSpectrum[j][i]=m_accSpectrum[i]/(double)m_nbrAccruedSpectra;
					m_accSpectrum[i]=0;
				}
				for(i=0;i<1024;i++){
					QLdata[i] = m_dspSpectrum[j][i*16];
				}
				m_nbrAccruedSpectra=0.0;
			}
/////////////////////////////////////////////////////////////////////////////
//データの記録。サチレーションがおきていても記録する
			fwrite(&m_dspSpectrum,sizeof(double),16384*loop2,fp);
			if(kazu%100==0){
				fwrite(&QLdata,sizeof(double),1024*loop2,fp2);
				end=clock();
				fp3=fopen(fnm3,"a");
				fprintf(fp3,"%f %d %d %d\n",((double)end-start)/1000,ovflows,kazu,fino);
				fclose(fp3);
			}

///////////////////////////////////////////////////////////////////////////
			m_nbrAccruedSpectra=0;
		}
		fclose(fp);	//ここで１枚の生データファイルができあがる
		fino = fino + 1; //ファイル番号更新
		printf("ok->cont. "); 

		present = time(NULL); //+ 60*60*8;	//西暦 1970 年1月1日から現在時刻までの経過秒数
		from_ed = difftime(present, ed_timer);	// 観測開始時刻と現在時刻の差 
		if(from_ed>0){		//観測終了時刻に達したか判断　達したら１日分の無限ループが終了
			break;
		}
	}
	fclose(fp2);	//ここで１日の観測終了　QLファイルができ上がる
	ptr2 = fopen(ope, "a");
	present = time(NULL); //+ 60*60*8;
	now = localtime(&present);
	fprintf(ptr2, "%d\n", now->tm_hour);
	fprintf(ptr2, "%d\n", now->tm_min);
	fprintf(ptr2, "%d\n", now->tm_sec);
	fclose(ptr2);
//////////////////////////////////////////////////////////////////////////////////////////

    printf("\nObservation end. Data pile line will start after 1 min.\n");
	Sleep(60000);	//少し時間をおいてから解析する(1min)
    printf("   Start AqDS.\n");
	system("\"AqDS.exe\"");	//解析プログラム開始
    printf("   Start AqDS_Chk_Sat.\n");
	system("\"AqDS_Chk_Sat.exe\"");
    printf("   Start AqDS_Chk_Sat2.\n");
	system("\"AqDS_Chk_Sat2.exe\"");
	//system("\"mail\\send_mail_ql.exe\"");	//観測結果通知
    printf("   Start AqDS_open_everyday.\n");
	system("\"AqDS_open_everyday.exe\"");	//アーカイブデータ作製
	system("\"copy_gif.exe\"");	//グラフを公開用WeBサーバーにアップ
    printf("   Start lawdata2savedata6.\n");
	system("\"lawdata2savedata6.exe\"");	//とりあえず8 bit で生データをアーカイブ
	sprintf(cmd,"plot_lv1_ql %04d%02d%02d",yy,mm,dd);
	system(cmd);	
	system("\"copy_pdf.exe\"");	
	system("\"mail\\send_mail_ql.exe\"");	//観測・解析完了通知
    printf("Data pile line have finished.\n");

//次の日のope_aqファイルができるまで待機して次の日のループに移行
/*	while(1){
		Sleep(60000);
		//printf("sleep");
		present = time(NULL);
		if(present - st_timer > 86400 - 3600){	//前の日の開始から１時間早くなることは無い
			break;
		}
	}
*/
	if(present < 0){	//つまり永遠に止まらない
		break;
	}
	sprintf(ope_1,"old_Qlhead_%d%02d%02d.txt",yy,mm,dd);
	rename(ope,ope_1);
}
///////////////////////////////////////////////////////////////////////////////////////////

	FPGAReset();
//	section 6.1.11
	stat = AcqrsD1_stopAcquisition(m_ID);
//	fprintf(fst,"6.1.11. stat=%d\n",stat);
	stat = AcqrsD1_closeAll();
//	fprintf(fst,"6.1.12. stat=%d\n",stat);
	printf("Program Exit\n");
	return 0;
}
