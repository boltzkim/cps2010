/*
	메모리 할당 및 해제에 대해서 트레이스 목적으로만 사용
	디버그시만 사용한다. 
	
	작성자 : 
	이력
	2010-07-28 : 시작
*/
/**
	@file tracemalloc.c
	@date 2013.04.01
	@author ETRI
	@version 1.0
	@brief 	DDS 실행시 어디까지 수행되고 있는지 메모리를 추적하기 위한 함수들을 정의한 파일
	@details 메모리 할당 및 해제에 대하여 추적기능에서 사용하기 위한 목적으로만 사용, 디버그시에만 사용함
*/
#include <core.h>

#ifdef __MEMEMORY_TRACING

static mutex_t	structure_lock;
bool structure_lock_initialize = false;


int64_t malloced_size = 0;

#undef malloc
#undef free
#undef strdup
#undef realloc

typedef struct malloc_t malloc_t;

//포인터(malloc_t::ptr), 함수이름, 이름에 대한 용어 수정 요망(5.08)
/**
	@brief DDS를 디버깅하기 위한 Trace 정보를 가진 속성을 가지고 있는 구조체
	@details 포인터 정보와 크기정보, 이름, 함수이름 등에 대한 속성을 가지고 있음
*/
struct malloc_t
{
	void	*ptr;
	size_t	size;
	char	name[1024];
	char	fname[1024];
	char	filename[1024];
	int		line;
};


static int			i_objects = 0;
static malloc_t		**pp_objects = NULL;

static int i_count=0 ;

static print_malloc_mount(const char *p_filename, const int line, const char *pfunctionname, const char *p_name, const char *psz, void *ptr, int i_size, int i_alloc_size);

/**
	@brief DDS를 디버깅하기 위한 Trace 구조체를 생성하는 함수
	@details 생성한 Trace 구조체를 전역 배열에 추가함
	@param[in] p_funcname 함수 이름
	@param[in] p_name 속성 이름
	@param[in] size 포인터 크기
	@return	생성한 Trace 구조체의 p_ptr
*/
void *tracemalloc(const char *p_filename, const int line, const char *p_funcname, const char *p_name, size_t size)
{
	malloc_t *p_malloc = malloc(sizeof(malloc_t));
	
	void *p_ptr=NULL;

	memset(p_malloc, 0, sizeof(malloc_t));

	if(!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	mutex_lock(&structure_lock);

	malloced_size += size;
	p_ptr = malloc(size);
	memset(p_ptr,0, size);

	p_malloc->ptr = p_ptr;
	p_malloc->size = size;
	memcpy(p_malloc->name, p_name, strlen(p_name));
	memcpy(p_malloc->fname, p_funcname, strlen(p_funcname));
	memcpy(p_malloc->filename, p_filename, strlen(p_filename));
	p_malloc->line = line;


	INSERT_ELEM(pp_objects, i_objects, i_objects, p_malloc);

	print_malloc_mount(p_filename, line, p_funcname,p_name,"malloc",p_ptr,i_objects, size);
	mutex_unlock(&structure_lock);
	return p_ptr;
}


/**
	@brief DDS를 디버깅하기 위한 Trace 구조체 크기를 재할당하는 함수
	@details Trace 구조체의 p_ptr의 크기를 재 할당하여 전역 배열에 추가함
	@param[in] p_funcname 함수 이름
	@param[in] p_name 속성 이름
	@param[in] p_ptr 크기를 재할당할 포인터
	@param[in] size 포인터 크기
	@return	크기를 재할당한 Trace 구조체의 p_ptr 
*/
void *tracerealloc(const char *p_filename, const int line, const char *p_funcname, const char *p_name, void *p_ptr, size_t size)
{
	malloc_t *p_malloc = NULL;
	int i;
	void	*p_ptrtmp = realloc(p_ptr, size);

	if(p_ptrtmp == p_ptr) return p_ptrtmp;

	p_malloc = malloc(sizeof(malloc_t));
	memset(p_malloc, 0, sizeof(malloc_t));
	if(!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	mutex_lock(&structure_lock);

	for(i=0; i < i_objects; i++)
	{
		if(pp_objects[i]->ptr == p_ptr)
		{
			malloced_size -= pp_objects[i]->size;

			FREE(pp_objects[i]);

			REMOVE_ELEM( pp_objects, i_objects, i );
			
			//FREE(p_ptr);
			break;
		}
	}

	

	malloced_size += size;
	p_malloc->ptr = p_ptrtmp;
	p_malloc->size = size;
	memcpy(p_malloc->name, p_name, strlen(p_name));
	memcpy(p_malloc->fname, p_funcname, strlen(p_funcname));
	memcpy(p_malloc->filename, p_filename, strlen(p_filename));
	p_malloc->line = line;

	INSERT_ELEM(pp_objects, i_objects, i_objects, p_malloc);

	print_malloc_mount(p_filename, line, p_funcname,p_name,"realloc",p_ptr,i_objects, size);
	mutex_unlock(&structure_lock);
	return p_ptrtmp;
}

void *wrong_obj = NULL;

void Check_Memory_Wrong_FREE(void *obj)
{
	wrong_obj = obj;
}


/**
	@brief DDS를 디버깅하기 위한 Trace 구조체를 삭제하는 함수
	@details Trace 구조체를 전역 배열에서 제거함
	@param[in] p_funcname 함수 이름
	@param[in] p_name 속성 이름
	@param[in] ptr 삭제할 Trace 구조체의 포인터 정보
*/
void tracefree(const char *p_filename, const int line, const char *p_funcname, const char *p_name, void *ptr)
{
	int i;
	bool free_found = false;
	mutex_lock(&structure_lock);

	

	for(i=0; i < i_objects; i++)
	{
		if(pp_objects[i]->ptr == ptr)
		{
			int size = pp_objects[i]->size;
			malloced_size -= pp_objects[i]->size;

			FREE(pp_objects[i]);

			REMOVE_ELEM( pp_objects, i_objects, i );
			//printf("free : %s\r\n", ptr);
			print_malloc_mount(p_filename, line, p_funcname,p_name,"free", ptr, i_objects, size);

			if(wrong_obj == ptr)
			{
				free_found = true;
				break;
			}
			
			

			free_found = true;
			
			break;
		}
	}
	if(free_found==false)
	{
		print_malloc_mount(p_filename, line, p_funcname,p_name,"free", ptr, i_objects, 0);
	}else{
		FREE(ptr);
	}

//	print_malloc_mount(p_filename, line, p_funcname,p_name,"free", ptr, i_objects-1);

	
	mutex_unlock(&structure_lock);
}

/**
	@brief DDS를 디버깅하기 위한 Trace 구조체를 복사하는 함수
	@details Trace 구조체를 전역 배열에 복사함, 문자형 상수 포인터를 사용하여 Trace 구조체의 p_ptr에 복사함
	@param[in] p_funcname 함수 이름
	@param[in] p_name 속성 이름
	@param[in] psz Trace 구조체의 포인터에 복사할 문자열
	@return	복사한 문자형 상수 포인터
*/
char *tracestrdup(const char *p_filename, const int line, const char *p_funcname, const char *p_name, const char *psz)
{
	char *p_sz;
	malloc_t *p_malloc = malloc(sizeof(malloc_t));
	
	int len = strlen(psz);
	memset(p_malloc, 0, sizeof(malloc_t));

	if(!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	mutex_lock(&structure_lock);

	malloced_size += len;
	p_sz = strdup(psz);

	p_malloc->ptr = p_sz;
	p_malloc->size = len;
	memcpy(p_malloc->name, p_name, strlen(p_name));
	memcpy(p_malloc->fname, p_funcname, strlen(p_funcname));
	memcpy(p_malloc->filename, p_filename, strlen(p_filename));
	p_malloc->line = line;

	/*if(len == 80)
	{
		p_malloc->size = len;
		printf("%d : %x Mem amount[%d:%lld] %s, %s\r\n", ++i_count, p_malloc->ptr, len, malloced_size,p_name, p_funcname);
	
	}*/

	INSERT_ELEM(pp_objects, i_objects, i_objects, p_malloc);

	print_malloc_mount(p_filename, line, p_funcname,p_name,"strdup",p_malloc->ptr, i_objects, len);

	mutex_unlock(&structure_lock);

	return p_sz;
}


static print_malloc_mount(const char *p_filename, const int line, const char *pfunctionname, const char *p_name, const char *psz, void *ptr, int i_size, int i_alloc_size)
{

//	int a_size = strlen("F:\\01.Develope\\2014\\05.cps\\cps_org_NEO\\cps2010\\cps2010\\src");
//	int a_size = 58;

	//if(i_alloc_size == 1500)
	//	printf("[%6X]%-7s:Mem amount[%d:%d:%lld] %s, %s  [%d]%s\r\n",ptr, psz,i_size, i_alloc_size, malloced_size,p_name,pfunctionname, line, &p_filename[a_size]);
}


void print_not_free_object()
{
	int a_size = 58;
	int i;

	for(i=0; i < i_objects; i++)
	{
		printf("[%3d][%6X][%6d]%s, %s\t [%d]%s \r\n",i,  pp_objects[i]->ptr, pp_objects[i]->size,pp_objects[i]->name, pp_objects[i]->fname, pp_objects[i]->line, &pp_objects[i]->filename[a_size]);
	}

	printf("\r\n\r\n Last remind memory : %d", malloced_size);
}

#else
void print_not_free_object()
{
}


#endif