/*
	파일 관련 UTIL 함수.

	작성자 : 
	이력
	2010-08-5 : 시작
*/
/**
	@file file.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DCPS에서 사용되는 file을 처리하는 파일
*/

#include <core.h>

/**
	@brief DCPS에서 사용되는 file을 open하는 함수 
	@param[in] psz_file_path Open할 파일의 경로  포인터
	@param[in] psz_mode 파일 open 모드 포인터
	@return	Open된 파일의 포인터 반환
*/
FILE* open_file( const char* psz_file_path, const char* psz_mode )
{
	FILE *file=NULL;

	file = fopen(psz_file_path, psz_mode);

	return file;
}

/**
	@brief DCPS에서 사용되는 file을 close하는 함수 
	@param[in] p_file close할 파일 포인터 
	@return	file close error 코드 값 반환 
*/
int close_file( FILE* p_file )
{
	if(p_file == NULL)
	{
		return MODULE_FAIL;
	}

	return fclose(p_file);
}
