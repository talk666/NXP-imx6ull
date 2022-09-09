#include "bsp_icm20608.h"

/*初始化icm20608*/
unsigned char icm20608_init(void)
{
    /*引脚初始化*/
    //1、复用引脚
    IOMUX_SW_MUX->UART2_RX_DATA = 0x8;//1000 ECSPI3_SCLK of instance: ecspi3
    IOMUX_SW_MUX->UART2_CTS_B = 0x8;       //1000 ECSPI3_MOSI of instance: ecspi3
    IOMUX_SW_MUX->UART2_RTS_B = 0x8;        //1000 ECSPI3_MISO of instance: ecspi3
    //input reg
    IMUXC_ECSP3_SCLK_SELECT_INPUT = 0;
    IMUXC_ECSP3_MISO_SELECT_INPUT = 0;
    IMUXC_ECSP3_MOSI_SELECT_INPUT = 0;

    //片选引脚   软件
    IOMUX_SW_MUX->UART2_TX_DATA = 0x5; // 1000 ECSPI3_SS0 of instance: ecspi3  (0101设置为gpio？软件片选 GPIO1_IO20 of instance: gpio1) 
        //gpio初始化
    gpio_pin_config gpio_pin_config_t;
	gpio_pin_config_t.direction = KGPIO_DigitalOutput;
	gpio_pin_config_t.outputLogic = 0; //设置默认电平 
	gpio_init(GPIO1, 20, &gpio_pin_config_t);

    //2、电气配置
    IOMUX_SW_PAD->UART2_RX_DATA = 0x10b1;
    IOMUX_SW_PAD->UART2_CTS_B = 0x10b1;
    IOMUX_SW_PAD->UART2_RTS_B = 0x10b1;

    IOMUX_SW_PAD->UART2_TX_DATA = 0x10b0;
    
    /*spi控制器初始化*/
    spi_init(ECSPI3);
    
    /*icm20608初始化*/
    icm20608_write_reg(ICM20_PWR_MGMT_1, 0x80);		/* 复位，复位后为0x40,睡眠模式 			*/
	delay_ms(50);
	icm20608_write_reg(ICM20_PWR_MGMT_1, 0x01);		/* 关闭睡眠，自动选择时钟 					*/
	delay_ms(50);
    
    unsigned char regvalue = icm20608_read_reg(ICM20_WHO_AM_I);
    if(regvalue != ICM20608G_ID && regvalue != ICM20608D_ID)
    {
	    printf("icm20608 id = %#X,error\r\n", regvalue);
        return 1;
    }

    icm20608_write_reg(ICM20_SMPLRT_DIV, 0x00); 	/* 输出速率是内部采样率					*/
	icm20608_write_reg(ICM20_GYRO_CONFIG, 0x18); 	/* 陀螺仪±2000dps量程 				*/
	icm20608_write_reg(ICM20_ACCEL_CONFIG, 0x18); 	/* 加速度计±16G量程 					*/
	icm20608_write_reg(ICM20_CONFIG, 0x04); 		/* 陀螺仪低通滤波BW=20Hz 				*/
	icm20608_write_reg(ICM20_ACCEL_CONFIG2, 0x04); 	/* 加速度计低通滤波BW=21.2Hz 			*/
	icm20608_write_reg(ICM20_PWR_MGMT_2, 0x00); 	/* 打开加速度计和陀螺仪所有轴 				*/
	icm20608_write_reg(ICM20_LP_MODE_CFG, 0x00); 	/* 关闭低功耗 						*/
	icm20608_write_reg(ICM20_FIFO_EN, 0x00);		/* 关闭FIFO						*/

#if 0
    regvalue = icm20608_read_reg(ICM20_ACCEL_CONFIG);
	printf("icm20608 id = %#X\r\n", regvalue);
#endif

    return 0;
}

/*ICM20608读数据*/
unsigned char icm20608_read_reg(unsigned char reg)
{   
    reg |= (0x80); /*地址的bit7 置1 为读*/
    unsigned char reg_val = 0;
    //片选拉低选中芯片
    ICM20608_CSN(0);
    spi0_readwrite_byte(ECSPI3, reg);/*读取地址*/
    reg_val = spi0_readwrite_byte(ECSPI3, 0xff);/*从此返回数据*/
    //片选拉高结束访问
    ICM20608_CSN(1);

    return reg_val;
}

/*ICM20608写数据*/
void icm20608_write_reg(unsigned char reg, unsigned char buf)
{   
    reg &= ~(0x80); /*地址的bit7 置0 为写*/

    //片选拉低选中芯片
    ICM20608_CSN(0);
    spi0_readwrite_byte(ECSPI3, reg);/*读取地址*/
    spi0_readwrite_byte(ECSPI3, buf);/*从此返回数据*/
    //片选拉高结束访问
    ICM20608_CSN(1);

}

/*读取多个寄存器值*/
void icm20608_read_len(unsigned char reg, unsigned char *buf, unsigned char len)
{
    unsigned i = 0;
    reg |= 0x80;

    ICM20608_CSN(0);
    spi0_readwrite_byte(ECSPI3, reg);

    for(i = 0; i < len; i++)
    {
        buf[i] = spi0_readwrite_byte(ECSPI3, 0xFF);
    }
    ICM20608_CSN(1);

}
/*
 * @description : 获取陀螺仪的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
float icm20608_gyro_scaleget(void)
{
	unsigned char data;
	float gyroscale;
	
	data = (icm20608_read_reg(ICM20_GYRO_CONFIG) >> 3) & 0X3;
	switch(data) {
		case 0: 
			gyroscale = 131;
			break;
		case 1:
			gyroscale = 65.5;
			break;
		case 2:
			gyroscale = 32.8;
			break;
		case 3:
			gyroscale = 16.4;
			break;
	}
	return gyroscale;
}

/*
 * @description : 获取加速度计的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
unsigned short icm20608_accel_scaleget(void)
{
	unsigned char data;
	unsigned short accelscale;
	
	data = (icm20608_read_reg(ICM20_ACCEL_CONFIG) >> 3) & 0X3;
	switch(data) {
		case 0: 
			accelscale = 16384;
			break;
		case 1:
			accelscale = 8192;
			break;
		case 2:
			accelscale = 4096;
			break;
		case 3:
			accelscale = 2048;
			break;
	}
	return accelscale;
}

/*
 * @description : 读取ICM20608的加速度、陀螺仪和温度原始值
 * @param 		: 无
 * @return		: 无
 */
void icm20608_getdata(void)
{
	float gyroscale;
	unsigned short accescale;
	unsigned char data[14];
	
	icm20608_read_len(ICM20_ACCEL_XOUT_H, data, 14);
	
    //获取陀螺仪的分辨率 根据设置的分辨率 精度不同
	gyroscale = icm20608_gyro_scaleget();
    //获取加速度计的分辨率
	accescale = icm20608_accel_scaleget();

	icm20608_dev.accel_x_adc = (signed short)((data[0] << 8) | data[1]); 
	icm20608_dev.accel_y_adc = (signed short)((data[2] << 8) | data[3]); 
	icm20608_dev.accel_z_adc = (signed short)((data[4] << 8) | data[5]); 
	icm20608_dev.temp_adc    = (signed short)((data[6] << 8) | data[7]); 
	icm20608_dev.gyro_x_adc  = (signed short)((data[8] << 8) | data[9]); 
	icm20608_dev.gyro_y_adc  = (signed short)((data[10] << 8) | data[11]);
	icm20608_dev.gyro_z_adc  = (signed short)((data[12] << 8) | data[13]);

	/* 计算实际值 */
	icm20608_dev.gyro_x_act = ((float)(icm20608_dev.gyro_x_adc)  / gyroscale) * 100;
	icm20608_dev.gyro_y_act = ((float)(icm20608_dev.gyro_y_adc)  / gyroscale) * 100;
	icm20608_dev.gyro_z_act = ((float)(icm20608_dev.gyro_z_adc)  / gyroscale) * 100;

	icm20608_dev.accel_x_act = ((float)(icm20608_dev.accel_x_adc) / accescale) * 100;
	icm20608_dev.accel_y_act = ((float)(icm20608_dev.accel_y_adc) / accescale) * 100;
	icm20608_dev.accel_z_act = ((float)(icm20608_dev.accel_z_adc) / accescale) * 100;

	icm20608_dev.temp_act = (((float)(icm20608_dev.temp_adc) - 25 ) / 326.8 + 25) * 100;
}