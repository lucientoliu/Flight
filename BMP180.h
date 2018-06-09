#ifndef BMP180_H
#define BMP180_H

#include "stm32f10x.h"


#define BMP180_ADDR			0XEE


/*BMP180¼Ä´æÆ÷µØÖ·*/
#define BMP180_ID			0XD0	//Ð¾Æ¬ID
#define BMP180_SOFT_RESET	0XE0	//Ð¾Æ¬ÖØÖÃ
#define BMP180_STATUS		0XF3	//Ð¾Æ¬×´Ì¬
#define BMP180_CTRL_MEAS	0XF4	//
#define BMP180_OUT_MSB		0XF6
#define BMP180_OUT_LSB		0XF7
#define BMP180_OUT_XLSB		0XF8

#define BMP180_CALIB00		0XAA
#define BMP180_CALIB01		0XAB
#define BMP180_CALIB02		0XAC
#define BMP180_CALIB03		0XAD
#define BMP180_CALIB04		0XAE
#define BMP180_CALIB05		0XAF
#define BMP180_CALIB06		0XB0
#define BMP180_CALIB07		0XB1
#define BMP180_CALIB08		0XB2
#define BMP180_CALIB09		0XB3
#define BMP180_CALIB10		0XB4
#define BMP180_CALIB11		0XB5
#define BMP180_CALIB12		0XB6
#define BMP180_CALIB13		0XB7
#define BMP180_CALIB14		0XB8
#define BMP180_CALIB15		0XB9
#define BMP180_CALIB16		0XBA
#define BMP180_CALIB17		0XBB
#define BMP180_CALIB18		0XBC
#define BMP180_CALIB19		0XBD
#define BMP180_CALIB20		0XBE
#define BMP180_CALIB21		0XBF


/*
#define	BMP180_CONFIG		0XF5
#define BMP180_PRESS_MSB	0XF7
#define BMP180_PRESS_LSB	0XF8
#define BMP180_PRESS_XLSB	0XF9
#define BMP180_TEMP_MSB		0XFA
#define BMP180_TEMP_LSB		0XFB
#define BMP180_TEMP_XLSB	0XFC

#define BMP180_CALIB00		0X88
#define BMP180_CALIB01		0X89
#define BMP180_CALIB02		0X8A
#define BMP180_CALIB03		0X8B
#define BMP180_CALIB04		0X8C
#define BMP180_CALIB05		0X8D
#define BMP180_CALIB06		0X8E
#define BMP180_CALIB07		0X8F
#define BMP180_CALIB08		0X90
#define BMP180_CALIB09		0X91
#define BMP180_CALIB10		0X92
#define BMP180_CALIB11		0X93
#define BMP180_CALIB12		0X94
#define BMP180_CALIB13		0X95
#define BMP180_CALIB14		0X96
#define BMP180_CALIB15		0X97
#define BMP180_CALIB16		0X98
#define BMP180_CALIB17		0X99
#define BMP180_CALIB18		0X9A
#define BMP180_CALIB19		0X9B
#define BMP180_CALIB20		0X9C
#define BMP180_CALIB21		0X9D
#define BMP180_CALIB22		0X9E
#define BMP180_CALIB23		0X9F
#define BMP180_CALIB24		0XA0
#define BMP180_CALIB25		0XA1
*/

/*bmp180¼Ä´æÆ÷Öµ*/
#define BMP180_ID_VALUE				0X55
#define BMP180_RESET_VALUE			0X55
#define BMP180_TEMPERATURE_MODE		0X2E
#define BMP180_PRESSURE_OSS0		0X34
#define BMP180_PRESSURE_OSS1		0X74
#define BMP180_PRESSURE_OSS2		0XB4
#define BMP180_PRESSURE_OSS3		0XF4

#define BMP180_PREOSS0		0
#define BMP180_PREOSS1		1
#define BMP180_PREOSS2		2
#define BMP180_PREOSS3		3




/*BMP180Òì³£Âë*/
#define BMP180_OK			0
#define	BMP180_READERR		1
#define	BMP180_NOCHECK  	2
#define BMP180_CALIBERR		3


typedef struct{
	u8 mode;
	u8 OverSamplingSetting;
	u8 osrs_p;
	u8 osrs_t;
	u8 IIRFilterCoeff;
	float IDD;
	float ODD;
	float RMS;
}T_FilterSetting;


typedef struct{
	s16 AC1;
	s16 AC2;
	s16 AC3;
	u16 AC4;
	u16 AC5;
	u16 AC6;
	s16 B1;
	s16 B2;
	s16 MB;
	s16 MC;
	s16 MD;
}T_BMP180CalibCoe;

typedef struct{
	u8 mode;
	u8 regValue;
	u8 delay;
}T_BMP180PressureMode;


extern s32 BMP180Temperature, BMP180Pressure;


u8 ucBMP180Check();
void ucBMP180GetTempAndPressure(u8 pressmode);
u8 ucBMP280GetCalibrationCoeff();


#endif