#if !defined(__SEQ_HH__)
#define __SEQ_HH__

namespace cps
{

class StringSeq2
{
public:
	StringSeq2()
	{
		memset(&seq, 0, sizeof(StringSeq));
	}

	StringSeq getStringSeq()
	{
		return seq;
	}

	StringSeq *getStringSeqPtr()
	{
		return &seq;
	}
#undef MEDIACVP
#define MEDIACVP (string *)
	void add(string value)
	{
		INSERT_ELEM(seq.pp_string, seq.i_string, seq.i_string, strdup(value));
	}
#undef MEDIACVP

private:
	StringSeq seq;
};


class ConditionSeq2
{
public:
	ConditionSeq2()
	{
		memset(&condseq, 0, sizeof(ConditionSeq));
	}

	ConditionSeq getConditionSeq()
	{
		return condseq;
	}

	ConditionSeq *getConditionSeqPtr()
	{
		return &condseq;
	}

private:
	ConditionSeq condseq;
};


class SampleInfoSeq2
{
public:
	SampleInfoSeq2()
	{
		memset(&sampleinfoseq, 0, sizeof(SampleInfoSeq));
	}

	~SampleInfoSeq2()
	{
		/*printf("~SampleInfoSeq2\r\n");
		fflush(stdout);*/
	}

	SampleInfoSeq getSampleInfoSeq()
	{
		return sampleinfoseq;
	}

	SampleInfoSeq *getSampleInfoSeqPtr()
	{
		return &sampleinfoseq;
	}

private:
	SampleInfoSeq sampleinfoseq;
};


class Message2
{
public:
	Message2()
	{
		p_message = NULL;
	}

	Message2(message_t *message)
	{
		p_message=message;
	}
	
	~Message2()
	{
		
	}

	void removeMessage()
	{
		if(p_message)message_release(p_message);
	}

	void *getValue()
	{
		return p_message->v_data;
	}

	

private:
	message_t *p_message;
};

class FooSeq2
{
public:
	FooSeq2()
	{
		memset(&fooseq, 0, sizeof(FooSeq));
	}

	~FooSeq2()
	{
		/*printf("~FooSeq2\r\n");
		fflush(stdout);*/
	}

	FooSeq getFooSeq()
	{
		return fooseq;
	}

	FooSeq *getFooSeqPtr()
	{
		return &fooseq;
	}

	bool has_data()
	{
		if(fooseq.i_seq == 0)
		{
			return false;
		}else{
			return true;
		}
	}

	int32_t getSize()
	{
		return fooseq.i_seq;
	}

#undef MEDIACVP
#define MEDIACVP (Foo **)
	Message2 getMessage()
	{
		message_t *p_message = (message_t *)fooseq.pp_foo[0];
		REMOVE_ELEM(fooseq.pp_foo, fooseq.i_seq, 0);
		return Message2(p_message);
	}
#undef MEDIACVP

private:
	FooSeq fooseq;
};




} // namespace cps end




#endif