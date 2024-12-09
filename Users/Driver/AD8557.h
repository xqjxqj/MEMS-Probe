#ifndef _AD8557_H_
#define _AD8557_H_

#define START								0x801

#define FUNCTION_CHANGE_SENSE_CURRENT		0x00
#define FUNCTION_SIMULATE_PARAMETER_VALUE	0x01   
#define FUNCTION_PROGRAM_PARAMETER_VALUE	0x02
#define FUNCTION_READ_PARAMETER_VALUE		0x03

#define PARAMETER_SECOND_STAGE_GAIN_CODE	0x00
#define LOW_CURRENT							0x01
#define HIGH_CURRENT						0x00

#define PARAMETER_FIRST_STAGE_GAIN_CODE		0x01
#define PARAMETER_OUTPUT_OFFSET_CODE		0x02
#define PARAMETER_OTHER_FUNCTIONS			0x03

#define MASTER_FUSE							0x01	
#define PARITY_FUSE							0x04	// only for AD8556				

#define DUMMY								0x02

#define STOP								0x7FE

#define MagCH0   0
#define MagCH1   1
#define MagCH2   2

typedef struct AD855XDATASTRUCTURE
{
	unsigned start:12;
	unsigned function:2;
	unsigned parameter:2;
	unsigned dummy:2;
	unsigned value:8;
	unsigned end:12;
	unsigned char SerialBit[38];
}AD855XDataStructure;

void SetMagGain(unsigned char FirstStageGainCode,unsigned char SecondStageGainCode,unsigned char OffsetCode,unsigned char ch);

#endif
