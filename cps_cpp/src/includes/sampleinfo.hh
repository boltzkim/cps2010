#if !defined(__SAMPLEINFO_HH__)
#define __SAMPLEINFO_HH__

namespace cps
{

class SampleInfo2
{
public:
	SampleInfo2() : p_sampleInfo(NULL)
	{
		
	}

	SampleInfo *getSampleInfo()
	{
		return p_sampleInfo;
	}

	/*void setFoo(Foo *foo)
	{
		p_foo = foo;
	}*/
private:
	SampleInfo *p_sampleInfo;
};


}

#endif