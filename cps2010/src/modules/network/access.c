/*
	access 관련 구현 부분
	
	작성자 : 
	이력
	2010-08-10 : 시작
*/

#include <core.h>


access_t* access_new( module_object_t* p_obj, char* psz_access, char* psz_path )
{
    access_t *p_access = object_create( p_obj, OBJECT_ACCESS );

    if( p_access == NULL )
    {
        return NULL;
    }


	if(psz_access)	p_access->psz_path   = strdup( psz_path );
	if(psz_path)	p_access->psz_access = strdup( psz_access );

	p_access->p_sys = NULL;
	p_access->pf_data = NULL;

    object_attach( OBJECT(p_access), p_obj );

	p_access->p_moduleload = moduleUse( OBJECT(p_access), "access", p_access->psz_access, false);

    if( p_access->p_moduleload == NULL )
    {
        object_detach( OBJECT(p_access) );
        FREE( p_access->psz_access );
        FREE( p_access->psz_path );
        object_destroy( OBJECT(p_access) );
        return NULL;
    }

    return p_access;
}


void access_delete( access_t* p_access )
{
	moduleUnuse( OBJECT(p_access), p_access->p_moduleload );
    object_detach( OBJECT(p_access) );

    FREE( p_access->psz_access );
    FREE( p_access->psz_path );

    object_destroy( OBJECT(p_access) );
}


access_out_t* access_out_new( module_object_t* p_this, char* psz_access, char* p_name, int fd )
{
    access_out_t	  *p_access;

    if( !( p_access = object_create( OBJECT(p_this), OBJECT_ACCESS_OUT) ) )
    {
       
        return NULL;
    }

    p_access->p_name   = strdup( p_name ? p_name : "" );
    p_access->p_sys = NULL;
    p_access->pf_write   = NULL;
	p_access->p_moduleload   = NULL;
	p_access->fd = fd;

	

    object_attach( OBJECT(p_access), OBJECT(p_this) );

    p_access->p_moduleload = moduleUse( OBJECT(p_access), "access out", psz_access, true );

    if( !p_access->p_moduleload )
    {
        FREE( p_access->p_name );
        object_detach( OBJECT(p_access) );
        object_destroy( OBJECT(p_access) );
        return( NULL );
    }

    return p_access;
}


void access_out_delete( access_out_t* p_access )
{
    object_detach( OBJECT(p_access) );
    if( p_access->p_moduleload )
    {
        moduleUnuse( OBJECT(p_access), p_access->p_moduleload );
    }
    FREE( p_access->p_name );

    object_destroy( OBJECT(p_access) );
}

