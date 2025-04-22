/*
	���� ���� UTIL �Լ�.

	�ۼ��� : 
	�̷�
	2010-08-5 : ����
*/
/**
	@file file.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DCPS���� ���Ǵ� file�� ó���ϴ� ����
*/

#include <core.h>

/**
	@brief DCPS���� ���Ǵ� file�� open�ϴ� �Լ� 
	@param[in] psz_file_path Open�� ������ ���  ������
	@param[in] psz_mode ���� open ��� ������
	@return	Open�� ������ ������ ��ȯ
*/
FILE* open_file( const char* psz_file_path, const char* psz_mode )
{
	FILE *file=NULL;

	file = fopen(psz_file_path, psz_mode);

	return file;
}

/**
	@brief DCPS���� ���Ǵ� file�� close�ϴ� �Լ� 
	@param[in] p_file close�� ���� ������ 
	@return	file close error �ڵ� �� ��ȯ 
*/
int close_file( FILE* p_file )
{
	if(p_file == NULL)
	{
		return MODULE_FAIL;
	}

	return fclose(p_file);
}
