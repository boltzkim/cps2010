/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class Subscriber2 : Entity2 {
  private HandleRef swigCPtr;

  internal Subscriber2(IntPtr cPtr, bool cMemoryOwn) : base(cpscsharpmodulePINVOKE.Subscriber2_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(Subscriber2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~Subscriber2() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_Subscriber2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public Subscriber2(Subscriber subscriber) : this(cpscsharpmodulePINVOKE.new_Subscriber2(Subscriber.getCPtr(subscriber)), true) {
  }

  public Subscriber getSubscriber() {
    IntPtr cPtr = cpscsharpmodulePINVOKE.Subscriber2_getSubscriber(swigCPtr);
    Subscriber ret = (cPtr == IntPtr.Zero) ? null : new Subscriber(cPtr, false);
    return ret;
  }

  public FooDataReader2 create_datareader(Topic2 topic, DataReaderQos qos, DataReaderListener2 listener, uint mask) {
    FooDataReader2 ret = new FooDataReader2(cpscsharpmodulePINVOKE.Subscriber2_create_datareader(swigCPtr, Topic2.getCPtr(topic), DataReaderQos.getCPtr(qos), DataReaderListener2.getCPtr(listener), mask), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_datareader(DataReader2 datareader) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_delete_datareader(swigCPtr, DataReader2.getCPtr(datareader));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_contained_entities() {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_delete_contained_entities(swigCPtr);
    return ret;
  }

  public FooDataReader2 lookup_datareader(string topic_name) {
    FooDataReader2 ret = new FooDataReader2(cpscsharpmodulePINVOKE.Subscriber2_lookup_datareader(swigCPtr, topic_name), true);
    return ret;
  }

  public int get_datareaders(DataReaderSeq readers, uint sample_states, uint view_states, uint instance_states) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_get_datareaders(swigCPtr, DataReaderSeq.getCPtr(readers), sample_states, view_states, instance_states);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int notify_datareaders() {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_notify_datareaders(swigCPtr);
    return ret;
  }

  public int set_qos(SubscriberQos qos) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_set_qos(swigCPtr, SubscriberQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_qos(SubscriberQos qos) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_get_qos(swigCPtr, SubscriberQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int set_listener(SubscriberListener2 listener, uint mask) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_set_listener(swigCPtr, SubscriberListener2.getCPtr(listener), mask);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public SubscriberListener2 get_listener() {
    SubscriberListener2 ret = new SubscriberListener2(cpscsharpmodulePINVOKE.Subscriber2_get_listener(swigCPtr), true);
    return ret;
  }

  public int begin_access() {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_begin_access(swigCPtr);
    return ret;
  }

  public int end_access() {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_end_access(swigCPtr);
    return ret;
  }

  public DomainParticipant2 get_participant() {
    DomainParticipant2 ret = new DomainParticipant2(cpscsharpmodulePINVOKE.Subscriber2_get_participant(swigCPtr), true);
    return ret;
  }

  public int set_default_datareader_qos(DataReaderQos qos) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_set_default_datareader_qos(swigCPtr, DataReaderQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_default_datareader_qos(DataReaderQos qos) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_get_default_datareader_qos(swigCPtr, DataReaderQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int copy_from_topic_qos(DataReaderQos datareader_qos, TopicQos topic_qos) {
    int ret = cpscsharpmodulePINVOKE.Subscriber2_copy_from_topic_qos(swigCPtr, DataReaderQos.getCPtr(datareader_qos), TopicQos.getCPtr(topic_qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}
