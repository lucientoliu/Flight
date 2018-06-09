#include "bmp180.h"
#include "softiic.h"
#include "uart.h"
#include "FreeRTOS.H"


u8 BMP180CalibrationBuf[22]; 
u16 buff[11];
//T_BMP180CalibCoe BMP180CalibCoe;
s32 BMP180Temperature, BMP180Pressure;

/*计算系数*/
long			X1, X2, X3, B3, B5, B6, B7, T, p, UT, UP;
unsigned long 	B4;
short 			AC1, AC2, AC3, B1, B2, MB, MC, MD;
unsigned short 	AC4, AC5, AC6;


const T_BMP180PressureMode PressMode[4] = {\
	{0, BMP180_PRESSURE_OSS0, 5},\
	{1, BMP180_PRESSURE_OSS1, 8},\
	{2, BMP180_PRESSURE_OSS2, 14},\
	{3, BMP180_PRESSURE_OSS3, 26}\
};

const u16 expr2[17] = {1, 2, 4, 8, 16, 32, 64, 128, 256, \
	512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};



u32 expr(u8 value, u8 n)
{
	u32 rev = 1;
	while (n-- != 0)
	{
		rev *= value;
	}
	return rev;
}



/***********************************************************
* 函数名称：ucBMP180Check
* 功能描述：BMP180检测
* 输入参数：
* 输出参数：
* 返 回 值：0 检测BMP180成功
			1 BMP180读取错误
			2 BMP180_ID寄存器值不对
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
u8 ucBMP180Check()
{
	u8 value;
    
	if(0 != SI2C_ReadByte(BMP180_ADDR, BMP180_ID, &value))
	{
		printf("BMP180 read err\r\n");
		return BMP180_READERR;
	}
	else
	{
		if (value == BMP180_ID_VALUE)
		{
			printf("BMP180 exist\r\n");
			return BMP180_OK;
		}
		else
		{
			printf("%x\r\n",value);
			printf("BMP180 not exist\r\n");
			return BMP180_NOCHECK;
		}
	}

}

/***********************************************************
* 函数名称：ucCheckCalibration
* 功能描述：检测校正系数是否正确
* 输入参数：
* 输出参数：
* 返 回 值：0 读取校正系数正确
			3 读取校正系数异常
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
u8 ucCheckCalibration()
{
	u8 i;
	for (i = 0; i < sizeof(BMP180CalibrationBuf)/2; i++)
	{
		if ((buff[i] == 0x0000)||(buff[i] == 0xffff))
			return BMP180_CALIBERR;
	}
	return BMP180_OK;
}

/***********************************************************
* 函数名称：ucBMP280GetCalibrationCoeff
* 功能描述：读取BMP180的校正数据
* 输入参数：
* 输出参数：
* 返 回 值：0 检测BMP180成功
			1 BMP180读取错误
			2 BMP180_ID寄存器值不对
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
u8 ucBMP280GetCalibrationCoeff()
{
	memset(BMP180CalibrationBuf, 0, sizeof(BMP180CalibrationBuf));
	if (0 != SI2C_ReadBytes(BMP180_ADDR, BMP180_CALIB00, BMP180CalibrationBuf, sizeof(BMP180CalibrationBuf)))
	{
		return BMP180_READERR;
	}
	else
	{
		buff[0] = (u16)BMP180CalibrationBuf[0]<<8 | BMP180CalibrationBuf[1];
		buff[1] = (u16)BMP180CalibrationBuf[2]<<8 | BMP180CalibrationBuf[3];
		buff[2] = (u16)BMP180CalibrationBuf[4]<<8 | BMP180CalibrationBuf[5];
		buff[3] = (u16)BMP180CalibrationBuf[6]<<8 | BMP180CalibrationBuf[7];
		buff[4] = (u16)BMP180CalibrationBuf[8]<<8 | BMP180CalibrationBuf[9];
		buff[5] = (u16)BMP180CalibrationBuf[10]<<8 | BMP180CalibrationBuf[11];
		buff[6] = (u16)BMP180CalibrationBuf[12]<<8 | BMP180CalibrationBuf[13];
		buff[7] = (u16)BMP180CalibrationBuf[14]<<8 | BMP180CalibrationBuf[15];
		buff[8] = (u16)BMP180CalibrationBuf[16]<<8 | BMP180CalibrationBuf[17];
		buff[9] = (u16)BMP180CalibrationBuf[18]<<8 | BMP180CalibrationBuf[19];
		buff[10] = (u16)BMP180CalibrationBuf[20]<<8 | BMP180CalibrationBuf[21];
		if (ucCheckCalibration() != BMP180_OK)
		{
			return BMP180_CALIBERR;
		}
		else
		{
			AC1 = (short)buff[0];
			AC2 = (short)buff[1];
			AC3 = (short)buff[2];
			AC4 = (unsigned short)buff[3];
			AC5 = (unsigned short)buff[4];
			AC6 = (unsigned short)buff[5];
			B1 = (short)buff[6];
			B2 = (short)buff[7];
			MB = (short)buff[8];
			MC = (short)buff[9];
			MD = (short)buff[10];
			
			return BMP180_OK;
		}
	}
}

/***********************************************************
* 函数名称：ucBMP180GetTemperature
* 功能描述：读取BMP180的温度数据
* 输入参数：
* 输出参数：
* 返 回 值：0 读取正确
			1 读取异常
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
u8 ucBMP180GetTemperature()
{
	u8 Temp[2], stat;
	
	stat = SI2C_WriteByte(BMP180_ADDR, BMP180_CTRL_MEAS, BMP180_TEMPERATURE_MODE);
	vTaskDelay(5/portTICK_RATE_MS);
	stat = SI2C_ReadBytes(BMP180_ADDR, BMP180_OUT_MSB, Temp, 2);
	UT = (long)((unsigned short)Temp[0]<<8 | Temp[1]);
	return stat;
}

/***********************************************************
* 函数名称：ucBMP180GetPressure
* 功能描述：读取BMP180的压力数据
* 输入参数：
* 输出参数：
* 返 回 值：0 读取正确
			1 读取异常
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
u8 ucBMP180GetPressure(u8 pressmode)
{
	u8 Press[3], stat;

	stat = SI2C_WriteByte(BMP180_ADDR, BMP180_CTRL_MEAS, PressMode[pressmode].regValue);
	vTaskDelay(PressMode[pressmode].delay/portTICK_RATE_MS);
	stat = SI2C_ReadBytes(BMP180_ADDR, BMP180_OUT_MSB, Press, 3);
	UP = ((s32)((u32)Press[0]<<16 + (u32)Press[1]<<8 + Press[2])) >> (8 - pressmode);
	return stat;
}

/***********************************************************
* 函数名称：vBMP180CalculateTemp
* 功能描述：计算实际温度
* 输入参数：
* 输出参数：
* 返 回 值：
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
void vBMP180CalculateTemp()
{
	X1 = ((UT - (u32)AC6) * AC5) >> 15;
	X2 = ((s32)MC << 11) / (X1 + MD);
	B5 = X1 + X2;
	BMP180Temperature = (B5 + 8) >> 4;
}


/***********************************************************
* 函数名称：vBMP180CalculatePressure
* 功能描述：计算实际压力
* 输入参数：
* 输出参数：
* 返 回 值：
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
void vBMP180CalculatePressure(u8 pressmode)
{
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 >> 12)) >> 11;
	X2 = AC2 * B6 >> 11;
	X3 = X1 + X2;
	B3 = ((AC1 << 2 + X3) << pressmode + 2) >> 2;
	X1 = AC3 * B6 >> 13;
	X2 = (B1 * (B6 * B6 >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = AC4 * (u32)(X3 + 32768) >> 15;
	B7 = ((u32)UP - B3) * (50000 >> pressmode);
	if (B7 < 0X80000000)
		p = (B7 << 1) / B4;
	else
		p = (B7 / B4) << 1;
	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * p) >> 16;
	p = p + (X1 + X2 + 3791) >> 4;
	BMP180Pressure = p;


}




void ucBMP180GetTempAndPressure(u8 pressmode)
{
	u8 stat;
	
	if (BMP180_OK == ucBMP180GetTemperature() && BMP180_OK == ucBMP180GetPressure(pressmode))
	{
		vBMP180CalculateTemp();
		vBMP180CalculatePressure(pressmode);
	}
	else
	{
		print("Acquire raw Temp&Press Data err"); 	//获取原始温度数据异常；
	}
}

/***********************************************************
* 函数名称：ucBMP180Reset
* 功能描述：复位BMP180
* 输入参数：
* 输出参数：
* 返 回 值：
			0 复位成功
			1 复位失败
* 其他说明：
*  修改日期   版本号    修改人   修改内容
* 2018-02-26    1.0      刘浩      创建
* ------------------------------------------------------

***********************************************************/
u8 ucBMP180Reset()
{
	return SI2C_WriteByte(BMP180_ADDR, BMP180_SOFT_RESET, BMP180_RESET_VALUE);
}


void vBMP180Init(u8 pressmode)
{
	SI2C_WriteByte(BMP180_ADDR, BMP180_CTRL_MEAS, PressMode[pressmode].regValue);
}
