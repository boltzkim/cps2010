%module(directors="1") cpsjavamodule 
%{ 


#include <core.hh>
#include "jni.h"

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

%}


%include "windows.i"
%include "wchar.i"
%include "typemaps.i"
%include "cpointer.i"
%include "boost_shared_ptr.i"
#include "carrays.i"

typedef unsigned int  uint32_t;



%include "typemaps.i"
%include "arrays_java.i"
%apply int[] {int *};


#define SWIG_ON 1

%feature("director")DomainParticipantListenerCallBack;
%feature("director")PublisherListenerCallBack;
%feature("director")TopicListenerCallBack;
%feature("director")DataWriterListenerCallBack;
%feature("director")DataReaderListenerCallBack;

%include "../wrapper.hh"
%include <core.hh>
%include <seq.hh>
%include <foo.hh>
%include <qos.hh>
%include <sampleinfo.hh>
%include <listener.hh>
%include <condition.hh>
%include <waitset.hh>
%include <entity.hh>
%include <topic.hh>
%include <datareader.hh>
%include <datawriter.hh>
%include <subscriber.hh>
%include <publisher.hh>
%include <domainparticipant.hh>
%include <domainparticipantfactory.hh>



%inline %{



%}
