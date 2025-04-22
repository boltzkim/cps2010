/*******************************************************************************
 * Copyright (c) 2011 Electronics and Telecommunications Research Institute
 * (ETRI) All Rights Reserved.
 * 
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 * 
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 ******************************************************************************/

/*
	Durability Service QoS Policy를 위한 모듈.

	이력
	2012-01-12
*/
#include <sys/stat.h>
#include "../../../src/modules/rtps/rtps.h"
#include "./sqlite/sqlite3.h"


/************************************************************************/
/*                                                                      */
/************************************************************************/
static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;
	for (i = 0; i < argc; i++)
	{
		trace_msg(OBJECT(p_module), TRACE_LOG, "%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	trace_msg(OBJECT(p_module), TRACE_LOG, "----");
	return 0;
}

#define DB_NAME "EDDS_%s.sqlite"
#define CREATE_TABLE_CACHECHANGE "DROP TABLE IF EXISTS cachechange_tbl; CREATE TABLE cachechange_tbl (id INTEGER PRIMARY KEY AUTOINCREMENT, cachechange BLOB, payload BLOB, payload_value BLOB, message BLOB, message_data BLOB, message_topic_name TEXT, message_type_name TEXT);"
#define INSERT_CACHECHANGE "INSERT INTO cachechange_tbl (cachechange, payload, payload_value, message, message_data, message_topic_name, message_type_name) VALUES (?, ?, ?, ?, ?, ?, ?);"
#define SELECT_CACHECHANGE "SELECT id, cachechange, payload, payload_value, message, message_data, message_topic_name, message_type_name FROM cachechange_tbl;"
#define CREATE_TABLE_CACHE_FOR_GUID "DROP TABLE IF EXISTS cache_for_guid_tbl; CREATE TABLE cache_for_guid_tbl (id INTEGER PRIMARY KEY AUTOINCREMENT, cachechange_id INTEGER, cache_for_guid BLOB);"
#define INSERT_CACHE_FOR_GUID "INSERT INTO cache_for_guid_tbl (cachechange_id, cache_for_guid) VALUES (?, ?);"
#define SELECT_CACHE_FOR_GUID "SELECT id, cache_for_guid FROM cache_for_guid_tbl WHERE cachechange_id = ?;"

static int save_historycache(const char* const p_topic_name, const rtps_historycache_t* const p_historycache)
{
	int n = 0;
	sqlite3* p_db = NULL; // SQLite DB 객체 저장 변수
	sqlite3_stmt* p_stmt_cachechange = NULL;
	sqlite3_stmt* p_stmt_cache_for_guid = NULL;
	char sz_query[0xFF];
	char* sz_error_message = NULL; // Error 발생시 메세지를 저장하는 변수
//	int i, j;
	int ret_code = -1;
	rtps_cachechange_t* p_change;
	sqlite3_int64 cachechange_id;
	rtps_cachechange_t *p_change_atom = NULL;
	cache_for_guid	*p_cache_atom = NULL;

	// SQLite DB파일을 열고 DB 연결 객체를 반환 한다.
	n = sprintf(sz_query, DB_NAME, p_topic_name);
	if (sqlite3_open(sz_query, &p_db) != SQLITE_OK)
	{
		printf("fail to sqlite3_open(), %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}

	// step 1. create table
	n = sprintf(sz_query, CREATE_TABLE_CACHECHANGE);
	if (sqlite3_exec(p_db, sz_query, NULL, NULL, NULL) != SQLITE_OK)
	{
		printf("fail to create table, %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}

	n = sprintf(sz_query, CREATE_TABLE_CACHE_FOR_GUID);
	if (sqlite3_exec(p_db, sz_query, NULL, NULL, NULL) != SQLITE_OK)
	{
		printf("fail to create table, %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}

	// step 2. insert blob value
	n = sprintf(sz_query, INSERT_CACHECHANGE);
	if (sqlite3_prepare_v2(p_db, sz_query, n, &p_stmt_cachechange, NULL) != SQLITE_OK)
	{
		printf("fail to sqlite3_prepare, %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}

	n = sprintf(sz_query, INSERT_CACHE_FOR_GUID);
	if (sqlite3_prepare_v2(p_db, sz_query, n, &p_stmt_cache_for_guid, NULL) != SQLITE_OK)
	{
		printf("fail to sqlite3_prepare, %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}


	p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;

	while(p_change_atom)
	{
		p_change = p_change_atom;
		sqlite3_bind_blob(p_stmt_cachechange, 1, (const void *)p_change, sizeof(rtps_cachechange_t), NULL);
		sqlite3_bind_blob(p_stmt_cachechange, 2, (SerializedPayloadForWriter*)p_change->p_data_value, sizeof(SerializedPayloadForWriter), NULL);
		sqlite3_bind_blob(p_stmt_cachechange, 3, ((SerializedPayloadForWriter*)p_change->p_data_value)->p_value, ((SerializedPayloadForWriter*)p_change->p_data_value)->i_size, NULL);
		sqlite3_bind_blob(p_stmt_cachechange, 4, p_change->p_org_message, sizeof(message_t), NULL);
		sqlite3_bind_blob(p_stmt_cachechange, 5, p_change->p_org_message->v_data, p_change->p_org_message->i_datasize, NULL);
		sqlite3_bind_text(p_stmt_cachechange, 6, p_change->p_org_message->p_topic_name, -1, NULL);
		sqlite3_bind_text(p_stmt_cachechange, 7, p_change->p_org_message->p_type_name, -1, NULL);
		sqlite3_step(p_stmt_cachechange);
		sqlite3_reset(p_stmt_cachechange);

		cachechange_id = sqlite3_last_insert_rowid(p_db);

		p_cache_atom = (cache_for_guid *)p_change->cache_for_guids.p_head_first;

		while(p_cache_atom)
		{
			sqlite3_reset(p_stmt_cache_for_guid);
			sqlite3_bind_int64(p_stmt_cache_for_guid, 1, cachechange_id);
			sqlite3_bind_blob(p_stmt_cache_for_guid, 2, (const void *)p_cache_atom, sizeof(cache_for_guid), NULL);
			sqlite3_step(p_stmt_cache_for_guid);
			p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
//			printf("***** %02d-%02d \n", i+1, j+1);
			
		}

		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
		
	}

	ret_code = 0;

FINAL:
	// 객체 해제
	if (p_stmt_cachechange != NULL && sqlite3_finalize(p_stmt_cachechange) != SQLITE_OK)
	{
		printf("fail to sqlite3_finalize, %s\n", sqlite3_errmsg(p_db));
	}
	if (p_stmt_cache_for_guid != NULL && sqlite3_finalize(p_stmt_cache_for_guid) != SQLITE_OK)
	{
		printf("fail to sqlite3_finalize, %s\n", sqlite3_errmsg(p_db));
	}
	if (p_db != NULL)
	{
		sqlite3_close(p_db);
		printf("Database closed!!\n");
	}
	return ret_code;
}


static bool is_user_writer(const rtps_writer_t* const p_rtps_writer)
{
	if (p_rtps_writer->is_builtin == false)
	{
		return true;
	}
	else 
	{
		return false;
	}
}


static bool is_transient_or_persistent(const DataWriter* const p_data_writer)
{
	if (p_data_writer->datawriter_qos.durability.kind == TRANSIENT_DURABILITY_QOS ||
		p_data_writer->datawriter_qos.durability.kind == PERSISTENT_DURABILITY_QOS)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void qosim_durability_service_save(qos_policy_t* const p_this, const rtps_writer_t* const p_rtps_writer)
{
	DataWriter* p_data_writer = p_rtps_writer->p_datawriter;

	if (is_user_writer(p_rtps_writer) && is_transient_or_persistent(p_data_writer))
	{
		save_historycache(p_data_writer->get_topic(p_data_writer)->get_name(p_data_writer->get_topic(p_data_writer)), p_rtps_writer->p_writer_cache);
	}
}


static bool file_exists(char* file_name)
{
	struct stat buf;
	int i = stat(file_name, &buf);
	// File found
	if (i == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


extern void rtps_historycache_add_re_ordering(rtps_historycache_t* p_historycache, rtps_cachechange_t* p_change);
static int load_historycache(const char* const p_topic_name, rtps_historycache_t* const p_historycache)
{
	sqlite3* p_db = NULL; // SQLite DB 객체 저장 변수
	sqlite3_stmt* p_stmt_cachechange = NULL;
	sqlite3_stmt* p_stmt_cache_for_guid = NULL;
	char sz_query[0xFF];
	char* sz_error_message = NULL; // Error 발생시 메세지를 저장하는 변수
	int n = 0;
	int n_ret = 0;
	int n_ret2 = 0;
	int ret_code = -1;
	int cachechange_id;
	rtps_cachechange_t* p_change = NULL;
	SerializedPayload* p_data_value = NULL;
	cache_for_guid* p_cache_for_guids = NULL;
	message_t* p_org_message = NULL;
	int i = 1, j = 1;

	n = sprintf(sz_query, DB_NAME, p_topic_name);
	if (!file_exists(sz_query))
	{
		return ret_code;
	}

	// SQLite DB파일을 열고 DB 연결 객체를 반환 한다.
	if (sqlite3_open(sz_query, &p_db) != SQLITE_OK)
	{
		printf("fail to sqlite3_open(), %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}

	// step 3. select blob value
	n = sprintf(sz_query, SELECT_CACHECHANGE);
	if (sqlite3_prepare_v2(p_db, sz_query, n, &p_stmt_cachechange, NULL) != SQLITE_OK)
	{
		printf("fail to select, %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}
	n = sprintf(sz_query, SELECT_CACHE_FOR_GUID);
	if (sqlite3_prepare_v2(p_db, sz_query, n, &p_stmt_cache_for_guid, NULL) != SQLITE_OK)
	{
		printf("fail to select, %s\n", sqlite3_errmsg(p_db));
		goto FINAL;
	}

	sqlite3_reset(p_stmt_cachechange);
	do
	{
		n_ret = sqlite3_step(p_stmt_cachechange);
		switch (n_ret)
		{
		case SQLITE_DONE:
			break;
		case SQLITE_BUSY:
			printf("database is busy.\n");
			break;
		case SQLITE_LOCKED:
			printf("database is locked.\n");
			break;
		case SQLITE_ROW:
			cachechange_id = sqlite3_column_int(p_stmt_cachechange, 0);
			p_change = malloc(sizeof(rtps_cachechange_t));
			memset(p_change, 0, sizeof(rtps_cachechange_t));
			memcpy(p_change, sqlite3_column_blob(p_stmt_cachechange, 1), sizeof(rtps_cachechange_t));
			p_data_value = malloc(sizeof(SerializedPayload));
			memcpy(p_data_value, sqlite3_column_blob(p_stmt_cachechange, 2), sizeof(SerializedPayload));
			p_data_value->p_value = malloc(sizeof(((SerializedPayloadForWriter*)p_data_value)->i_size));
			memcpy(p_data_value->p_value, sqlite3_column_blob(p_stmt_cachechange, 3), sizeof(((SerializedPayloadForWriter*)p_data_value)->i_size));
			p_org_message = malloc(sizeof(message_t));
			memcpy(p_org_message, sqlite3_column_blob(p_stmt_cachechange, 4), sizeof(message_t));
			p_org_message->v_data = malloc(sizeof(p_org_message->i_datasize));
			memcpy(p_org_message->v_data, sqlite3_column_blob(p_stmt_cachechange, 5), sizeof(p_org_message->i_datasize));
			p_org_message->p_topic_name = strdup((char*)sqlite3_column_text(p_stmt_cachechange, 6));
			p_org_message->p_type_name = strdup((char*)sqlite3_column_text(p_stmt_cachechange, 7));
			p_org_message->v_related_cachechange = p_change;

			p_change->p_data_value = p_data_value;
			p_change->p_org_message = p_org_message;
			p_change->cache_for_guids.p_head_first = NULL;
			p_change->cache_for_guids.p_head_last = NULL;
			p_change->cache_for_guids.i_linked_size = 0;
			

			sqlite3_bind_int(p_stmt_cache_for_guid, 1, cachechange_id);
			j = 1;
			do 
			{
				n_ret2 = sqlite3_step(p_stmt_cache_for_guid);
				switch (n_ret2)
				{
				case SQLITE_DONE:
					break;
				case SQLITE_BUSY:
					printf("database is busy.\n");
					break;
				case SQLITE_LOCKED:
					printf("database is locked.\n");
					break;
				case SQLITE_ROW:
					p_cache_for_guids = malloc(sizeof(cache_for_guid));
					memset(p_cache_for_guids, 0, sizeof(cache_for_guid));
					memcpy(p_cache_for_guids, sqlite3_column_blob(p_stmt_cache_for_guid, 1), sizeof(cache_for_guid));

					insert_linked_list(&p_change->cache_for_guids, (linked_list_atom_t *)p_cache_for_guids);

					//INSERT_ELEM( p_change->pp_cache_for_guids, p_change->i_cache_for_guids, p_change->i_cache_for_guids, p_cache_for_guids);
					printf("***** %02d-%02d \n", i, j);
					break;
				default:
					printf("sqlite3_step return '%d'\n", n_ret);
					break;
				}
				j++;
			} while (n_ret2 == SQLITE_ROW);

			rtps_historycache_add_re_ordering( p_historycache, p_change);
			sqlite3_reset(p_stmt_cache_for_guid);
			break;
		default:
			printf("sqlite3_step return '%d'\n", n_ret);
			break;
		}

		i++;
	} while (n_ret == SQLITE_ROW);


FINAL:
	// 객체 해제
	if (p_stmt_cachechange != NULL && sqlite3_finalize(p_stmt_cachechange) != SQLITE_OK)
	{
		printf("fail to sqlite3_finalize, %s\n", sqlite3_errmsg(p_db));
	}
	if (p_stmt_cache_for_guid != NULL && sqlite3_finalize(p_stmt_cache_for_guid) != SQLITE_OK)
	{
		printf("fail to sqlite3_finalize, %s\n", sqlite3_errmsg(p_db));
	}
	if (p_db != NULL)
	{
		sqlite3_close(p_db);
		printf("Database closed!!\n");
	}
	return ret_code;
}


void qosim_durability_service_load(qos_policy_t* const p_this, const rtps_writer_t* const p_rtps_writer)
{
	DataWriter* p_data_writer = p_rtps_writer->p_datawriter;

	if (is_user_writer(p_rtps_writer) && is_transient_or_persistent(p_data_writer))
	{
		load_historycache(p_data_writer->get_topic(p_data_writer)->get_name(p_data_writer->get_topic(p_data_writer)), p_rtps_writer->p_writer_cache);
	}
}
