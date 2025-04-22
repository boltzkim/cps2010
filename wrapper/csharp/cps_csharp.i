%module(directors="1") cpscsharpmodule 
%{ 


#include <core.hh>


#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

%}

typedef void *HANDLE;
typedef wchar_t WCHAR; 
typedef WCHAR TCHAR, *PTCHAR;

%include "wchar.i"
%include "arrays_csharp.i"
%include "typemaps.i"
%include "cpointer.i"
%include "boost_shared_ptr.i"
#include "carrays.i"

typedef unsigned int  uint32_t;

%apply int INOUT[] {int *x}
%apply unsigned int INOUT[] {unsigned int *x}
%apply uint32_t INOUT[] {uint32_t *x}


%typemap(cstype) void* "IntPtr" 
%typemap(csin) void* "new HandleRef(null, $csinput)" 
%typemap(csout, excode=SWIGEXCODE) void* "{IntPtr res = $imcall; $excode; return res;}" 
%typemap(csvarout, excode=SWIGEXCODE2) void* "get{IntPtr res = $imcall; $excode; return res;}" 
%typemap(csdirectorin) void* "$iminput" 
%typemap(csdirectorout) void* "$cscall" 

// This will convert a given integral pointer type to pointer in any occasion... 
%define %INTEGRAL_ARRAY_TYPEMAP(CTYPE, CSTYPE) 
        %typemap(cstype) CTYPE* "CSTYPE*" 
        %typemap(csin) CTYPE* "new HandleRef(null, (IntPtr)$csinput)" 
        %typemap(csout, excode=SWIGEXCODE) CTYPE* "{CSTYPE* res = (CSTYPE*)($imcall); $excode; return res;}" 
        %typemap(csvarout, excode=SWIGEXCODE2) CTYPE* "get{CSTYPE* res = (CSTYPE*)$imcall; $excode; return res;}" 
        %typemap(csdirectorin) CTYPE* "(CSTYPE*)$iminput" 
        %typemap(csdirectorout) CTYPE* "(IntPtr)$cscall" 
%enddef 


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
