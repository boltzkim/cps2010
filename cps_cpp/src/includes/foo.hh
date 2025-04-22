#if !defined(__FOO_HH__)
#define __FOO_HH__



namespace cps
{
class DomainParticipant2;

class FooTypeSupport2
{
public:
	FooTypeSupport2()
	{
		p_fooTypeSupport = get_new_foo_type_support();
	}

	FooTypeSupport *getFooTypeSupport()
	{
		
		return p_fooTypeSupport;
	}

	ReturnCode_t register_type(in_dds DomainParticipant2 participant, in_dds string type_name);
	string get_type_name();
	///////
	bool insert_parameter(string parameter_name, SUPPORT_TYPE type, bool is_key = false);
	bool insert_parameter_struct(string parameter_name, int size, bool is_key = false);
	void setFootypeSize(int size);


private:
	FooTypeSupport *p_fooTypeSupport;

	
};

typedef enum FooRet
{
	OK,
	PARAMETER_NOT_EXIST,
	TYPE_WRONG,
	FOO_NOT_EXIST,
	FooTypeSupport2_NOT_EXIST
}FooRet;

class Foo2
{
public:

	Foo2(void *data) : p_foo((Foo*)data), p_tsFoo(NULL), hasTypesupport(false)
	{
		
	}

	Foo2() : p_foo(NULL), p_tsFoo(NULL), hasTypesupport(false)
	{
		
	}


	Foo2(FooTypeSupport2 fooTS1) : typysupport(fooTS1), p_foo(NULL), p_tsFoo(NULL), hasTypesupport(true)
	{

	}

	Foo2(FooTypeSupport2 fooTS1, void *data) : typysupport(fooTS1), p_foo((Foo*)data), p_tsFoo(NULL), hasTypesupport(true)
	{

	}

	~Foo2()
	{
		
	}


	void removeFoo()
	{
		FREE(p_tsFoo);
	}

	Foo *getFoo();
	

	/*void setFoo(Foo *foo)
	{
		p_foo = foo;
	}*/

	FooRet setString(string parameter, string value);
	//FooRet setInt(string parameter, int value);
	FooRet setFloat(string parameter, float value);

	FooRet setInt8(string parameter, int8_t value);
	FooRet setInt16(string parameter, int16_t value);
	FooRet setInt32(string parameter, int32_t value);
	FooRet setInt64(string parameter, __int64 value);

	FooRet setUint8(string parameter,  uint8_t value);
	FooRet setUint16(string parameter, uint16_t value);
	FooRet setUint32(string parameter, uint32_t value);
	FooRet setUint64(string parameter, unsigned __int64 value);
	FooRet setDouble(string parameter, double value);
	FooRet setBoolean(string parameter, bool value);
	FooRet setByte(string parameter, int8_t value);

	string getString(string parameter);
	//int getInt(string parameter);
	float getFloat(string parameter);


	int8_t getInt8(string parameter);
	int16_t getInt16(string parameter);
	int32_t getInt32(string parameter);
	__int64 getInt64(string parameter);

	uint8_t getUint8(string parameter);
	uint16_t getUint16(string parameter);
	uint32_t getUint32(string parameter);
	unsigned __int64 getUint64(string parameter);
	double getDouble(string parameter);
	bool getBoolean(string parameter);
	int8_t getByte(string parameter);

private:
	Foo *p_foo;
	FooTypeSupport2 typysupport;
	Foo *p_tsFoo;
	bool hasTypesupport;
};




}

#endif