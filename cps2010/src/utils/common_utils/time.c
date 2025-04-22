/*
	시간 관련 UTIL 함수.

	작성자 : 
	이력
	2010-08-8 : 시작
*/
/**
	@file file.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DCPS에서 사용되는 Time 관련 파일 
*/

#define _CRT_SECURE_NO_WARNINGS
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <sys/timeb.h>
#include <time.h>


/** 
	@brief DCPS에서 현재 시간을 반환하는 함수 
	@return	DCPS의 현재 시간 반환
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
	@brief DCPS에서 현재 시간을 반환하는 함수 
	@return	DCPS의 현재 시간 반환
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
	@brief DCPS에서 두 시간을 더하여 더한 시간을 반환하는 함수 
	@param[in] augend_time 현재 시간 변수
	@param[in] addend_time 추가될 시간 변수
	@return	현재 시간과 추가될 시간의 합을 반
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
	@brief DCPS에서 두 시간의 차를 반환하는 함수 
	@param[in] minuend_time 현재 시간 변수
	@param[in] subtrahend_time 삭제될 시간 변수 
	@return	현재 시간과 삭제될 시간의 차를 반환
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
	@brief DCPS에서 두 시간을 비교하는 함수 
	@param[in] time_1 비교 시간 값 변수
	@param[in] time_2 비교 시간 값 변수
	@return	두 시간 중 왼 값이 크면 TURE 반환 
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
	@brief DCPS에서 두 시간을 더하는 함수 
	@param[in] augend_time 현재 시간 변수
	@param[in] addend_time 추가될 시간 변수
	@return	현재 시간과 추가될 시간의 합
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
	@brief DCPS에서 두 시간의 차를 구하는 함수  
	@param[in] minuend_time 비교 시간 값 변수
	@param[in] subtrahend_time 비교 시간 값 변수
	@return	두 시간의 차이 시간 반환
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
	@brief DCPS에서 두 시간을 비교하는 함수 
	@param[in] time_1 비교 시간 값 변수
	@param[in] time_2 비교 시간 값 변수
	@return	두 시간 중 오른 값이 크면 TURE 반환 
*/
bool time_left_bigger_d(const Duration_t time_1, const Duration_t time_2)
{
	return (time_1.sec > time_2.sec || (time_1.sec == time_2.sec && time_1.nanosec > time_2.nanosec));
}