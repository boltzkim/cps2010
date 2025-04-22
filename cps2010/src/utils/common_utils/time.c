/*
	�ð� ���� UTIL �Լ�.

	�ۼ��� : 
	�̷�
	2010-08-8 : ����
*/
/**
	@file file.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DCPS���� ���Ǵ� Time ���� ���� 
*/

#define _CRT_SECURE_NO_WARNINGS
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <sys/timeb.h>
#include <time.h>


/** 
	@brief DCPS���� ���� �ð��� ��ȯ�ϴ� �Լ� 
	@return	DCPS�� ���� �ð� ��ȯ
*/
Duration_t current_duration()
{
	Duration_t current;

#ifdef _MSC_VER
	struct __timeb64 timebuffer;
    _ftime64(&timebuffer);
#else
	struct timeb timebuffer;
    ftime(&timebuffer);
#endif

	current.sec = (int32_t)timebuffer.time;
    current.nanosec = timebuffer.millitm * 1000000;

	return current;
}
/** 
	@brief DCPS���� ���� �ð��� ��ȯ�ϴ� �Լ� 
	@return	DCPS�� ���� �ð� ��ȯ
*/
Time_t currenTime()
{
	Time_t current;

#ifdef _MSC_VER
	struct __timeb64 timebuffer;
    _ftime64(&timebuffer);
#else
	struct timeb timebuffer;
    ftime(&timebuffer);
#endif

	current.sec = (int32_t)timebuffer.time;
    current.nanosec = timebuffer.millitm * 1000000;

	return current;
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/** 
	@brief DCPS���� �� �ð��� ���Ͽ� ���� �ð��� ��ȯ�ϴ� �Լ� 
	@param[in] augend_time ���� �ð� ����
	@param[in] addend_time �߰��� �ð� ����
	@return	���� �ð��� �߰��� �ð��� ���� ��
*/
Time_t time_addition(const Time_t augend_time, const Time_t addend_time)
{
	Time_t sum_time;

	sum_time.sec = augend_time.sec + addend_time.sec;
	sum_time.nanosec = augend_time.nanosec + addend_time.nanosec;
	if (sum_time.nanosec >= 1000000000)
	{
		sum_time.sec += 1;
		sum_time.nanosec -= 1000000000;
	}

	return sum_time;
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/** 
	@brief DCPS���� �� �ð��� ���� ��ȯ�ϴ� �Լ� 
	@param[in] minuend_time ���� �ð� ����
	@param[in] subtrahend_time ������ �ð� ���� 
	@return	���� �ð��� ������ �ð��� ���� ��ȯ
*/
Time_t time_subtraction(const Time_t minuend_time, const Time_t subtrahend_time)
{
	Time_t difference_time;

	difference_time.sec = minuend_time.sec - subtrahend_time.sec;
	difference_time.nanosec = minuend_time.nanosec - subtrahend_time.nanosec;
	if (minuend_time.nanosec < subtrahend_time.nanosec)
	{
		difference_time.sec -= 1;
		difference_time.nanosec += 1000000000;
	}

	return difference_time;
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/** 
	@brief DCPS���� �� �ð��� ���ϴ� �Լ� 
	@param[in] time_1 �� �ð� �� ����
	@param[in] time_2 �� �ð� �� ����
	@return	�� �ð� �� �� ���� ũ�� TURE ��ȯ 
*/
bool time_left_bigger(const Time_t time_1, const Time_t time_2)
{
	return (time_1.sec > time_2.sec || (time_1.sec == time_2.sec && time_1.nanosec > time_2.nanosec));
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
Time_t to_time(const Duration_t duration)//by kki
{
	Time_t time;
	time.sec = duration.sec;
	time.nanosec = duration.nanosec;
	return time;
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/** 
	@brief DCPS���� �� �ð��� ���ϴ� �Լ� 
	@param[in] augend_time ���� �ð� ����
	@param[in] addend_time �߰��� �ð� ����
	@return	���� �ð��� �߰��� �ð��� ��
*/
Duration_t time_addition_d(const Duration_t augend_time, const Duration_t addend_time)
{
	Duration_t sum_time;

	sum_time.sec = augend_time.sec + addend_time.sec;
	sum_time.nanosec = augend_time.nanosec + addend_time.nanosec;
	if (sum_time.nanosec >= 1000000000)
	{
		sum_time.sec += 1;
		sum_time.nanosec -= 1000000000;
	}

	return sum_time;
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/** 
	@brief DCPS���� �� �ð��� ���� ���ϴ� �Լ�  
	@param[in] minuend_time �� �ð� �� ����
	@param[in] subtrahend_time �� �ð� �� ����
	@return	�� �ð��� ���� �ð� ��ȯ
*/
Duration_t time_subtraction_d(const Duration_t minuend_time, const Duration_t subtrahend_time)
{
	Duration_t difference_time;

	difference_time.sec = minuend_time.sec - subtrahend_time.sec;
	difference_time.nanosec = minuend_time.nanosec - subtrahend_time.nanosec;
	if (minuend_time.nanosec < subtrahend_time.nanosec)
	{
		difference_time.sec -= 1;
		difference_time.nanosec += 1000000000;
	}

	return difference_time;
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/** 
	@brief DCPS���� �� �ð��� ���ϴ� �Լ� 
	@param[in] time_1 �� �ð� �� ����
	@param[in] time_2 �� �ð� �� ����
	@return	�� �ð� �� ���� ���� ũ�� TURE ��ȯ 
*/
bool time_left_bigger_d(const Duration_t time_1, const Duration_t time_2)
{
	return (time_1.sec > time_2.sec || (time_1.sec == time_2.sec && time_1.nanosec > time_2.nanosec));
}