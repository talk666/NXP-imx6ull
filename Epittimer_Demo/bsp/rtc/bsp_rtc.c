#include "bsp_rtc.h"

/*RTC_INIT函数*/
void RTC_Init(void)
{
    SNVS->HPCOMR |= (1 << 31) | (1 << 8);//Non-Privileged Software Access Enable

	rtc_enable();	//使能RTC
#if 0
	struct rtc_datetime datetime;
    datetime.year = 2022;
    datetime.month = 9;
    datetime.day = 7;
    datetime.hour = 24;
    datetime.minute = 0;
    datetime.second = 0;
	rtc_setdatetime(&datetime);
#endif
}


/*使能RTC*/
void rtc_enable(void)
{
    SNVS->LPCR |= (1 << 0);//Secure Real Time Counter Enabled and Valid
    while((SNVS->LPCR & 0x01) == 0);
}

/*关闭RTC*/
void rtc_disable(void)
{
    SNVS->LPCR &= ~(1 << 0);
    while((SNVS->LPCR & 0x01) == 1);
}

/*移植
 * @description	: 判断指定年份是否为闰年，闰年条件如下:
 * @param - year: 要判断的年份
 * @return 		: 1 是闰年，0 不是闰年
 */
unsigned char rtc_isleapyear(unsigned short year)
{	
	unsigned char value=0;
	
	if(year % 400 == 0)
		value = 1;
	else 
	{
		if((year % 4 == 0) && (year % 100 != 0))
			value = 1;
		else 
			value = 0;
	}
	return value;
}

/* 移植
 * @description		: 将秒数转换为时间
 * @param - seconds	: 要转换的秒数
 * @param - datetime: 转换后的日期和时间
 * @return 			: 无
 */
void rtc_convertseconds_to_datetime(u64 seconds, struct rtc_datetime *datetime)
{
    u64 x;
    u64  secondsRemaining, days;
    unsigned short daysInYear;

    /* 每个月的天数       */
    unsigned char daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    secondsRemaining = seconds; /* 剩余秒数初始化 */
    days = secondsRemaining / SECONDS_IN_A_DAY + 1; 		/* 根据秒数计算天数,加1是当前天数 */
    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY; /*计算天数以后剩余的秒数 */

	/* 计算时、分、秒 */
    datetime->hour = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->minute = secondsRemaining / 60;
    datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

    /* 计算年 */
    daysInYear = DAYS_IN_A_YEAR;
    datetime->year = YEAR_RANGE_START;
    while(days > daysInYear)
    {
        /* 根据天数计算年 */
        days -= daysInYear;
        datetime->year++;

        /* 处理闰年 */
        if (!rtc_isleapyear(datetime->year))
            daysInYear = DAYS_IN_A_YEAR;
        else	/*闰年，天数加一 */
            daysInYear = DAYS_IN_A_YEAR + 1;
    }
	/*根据剩余的天数计算月份 */
    if(rtc_isleapyear(datetime->year)) /* 如果是闰年的话2月加一天 */
        daysPerMonth[2] = 29;

    for(x = 1; x <= 12; x++)
    {
        if (days <= daysPerMonth[x])
        {
            datetime->month = x;
            break;
        }
        else
        {
            days -= daysPerMonth[x];
        }
    }

    datetime->day = days;

}

/*年月日时分秒转换接口  移植*/
/*
 * @description		: 将时间转换为秒数
 * @param - datetime: 要转换日期和时间。
 * @return 			: 转换后的秒数
 */
unsigned int rtc_coverdate_to_seconds(struct rtc_datetime *datetime)
{	
	unsigned short i = 0;
	unsigned int seconds = 0;
	unsigned int days = 0;
	unsigned short monthdays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
	
	for(i = 1970; i < datetime->year; i++)
	{
		days += DAYS_IN_A_YEAR; 		/* 平年，每年365天 */
		if(rtc_isleapyear(i)) days += 1;/* 闰年多加一天 		*/
	}

	days += monthdays[datetime->month];
	if(rtc_isleapyear(i) && (datetime->month >= 3)) days += 1;/* 闰年，并且当前月份大于等于3月的话加一天 */

	days += datetime->day - 1;

	seconds = days * SECONDS_IN_A_DAY + 
				datetime->hour * SECONDS_IN_A_HOUR +
				datetime->minute * SECONDS_IN_A_MINUTE +
				datetime->second;

	return seconds;	
}


/*//就是往RTC计数器里面值进行操作
 * @description		: 设置时间和日期
 * @param - datetime: 要设置的日期和时间
 * @return 			: 无
 */
void rtc_setdatetime(struct rtc_datetime *datetime)
{
	
	unsigned int seconds = 0;
	unsigned int tmp = SNVS->LPCR; 
	
	rtc_disable();	/* 设置寄存器HPRTCMR和HPRTCLR的时候一定要先关闭RTC */

	
	/* 先将时间转换为秒         */
	seconds = rtc_coverdate_to_seconds(datetime);
	
	SNVS->LPSRTCMR = (unsigned int)(seconds >> 17); /* 设置高16位 就是取 */
	SNVS->LPSRTCLR = (unsigned int)(seconds << 15); /* 设置地16位 */

	/* 如果此前RTC是打开的在设置完RTC时间以后需要重新打开RTC */
	if (tmp & 0x1)
		rtc_enable();
}

/*读取秒数*/
uint64_t rtc_getseconds(void)
{
    //linux源码中42bit抛弃高15bit 
	uint64_t seconds = 0;
	seconds = (SNVS->LPSRTCMR << 17) | (SNVS->LPSRTCLR >> 15);  //LPSRTCMR低15（0-14）bit   和  LPSRTCLR高17（31:16）bits
	return seconds;
}

/*获取时间*/
void rtc_getdatetime(struct rtc_datetime *datetime)
{	//unsigned int seconds = 0;
	u64 seconds;
	//从寄存器读取计数器
	seconds = rtc_getseconds();

	rtc_convertseconds_to_datetime(seconds, datetime);	
}