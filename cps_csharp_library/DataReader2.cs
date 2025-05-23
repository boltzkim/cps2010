/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class DataReader2 : Entity2 {
  private HandleRef swigCPtr;

  internal DataReader2(IntPtr cPtr, bool cMemoryOwn) : base(cpscsharpmodulePINVOKE.DataReader2_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(DataReader2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~DataReader2() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_DataReader2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public DataReader2(DataReader dataReader) : this(cpscsharpmodulePINVOKE.new_DataReader2(DataReader.getCPtr(dataReader)), true) {
  }

  public DataReader getDataReader() {
    IntPtr cPtr = cpscsharpmodulePINVOKE.DataReader2_getDataReader(swigCPtr);
    DataReader ret = (cPtr == IntPtr.Zero) ? null : new DataReader(cPtr, false);
    return ret;
  }

  public ReadCondition2 create_readcondition(uint sample_states, uint view_states, uint instance_states) {
    ReadCondition2 ret = new ReadCondition2(cpscsharpmodulePINVOKE.DataReader2_create_readcondition(swigCPtr, sample_states, view_states, instance_states), true);
    return ret;
  }

  public QueryCondition2 create_querycondition(uint sample_states, uint view_states, uint instance_states, string query_expression, StringSeq query_parameters) {
    QueryCondition2 ret = new QueryCondition2(cpscsharpmodulePINVOKE.DataReader2_create_querycondition(swigCPtr, sample_states, view_states, instance_states, query_expression, StringSeq.getCPtr(query_parameters)), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_readcondition(ReadCondition2 condition) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_delete_readcondition(swigCPtr, ReadCondition2.getCPtr(condition));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_contained_entities() {
    int ret = cpscsharpmodulePINVOKE.DataReader2_delete_contained_entities(swigCPtr);
    return ret;
  }

  public int set_qos(DataReaderQos qos) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_set_qos(swigCPtr, DataReaderQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_qos(DataReaderQos qos) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_qos(swigCPtr, DataReaderQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int set_listener(DataReaderListener2 listener, uint mask) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_set_listener(swigCPtr, DataReaderListener2.getCPtr(listener), mask);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public DataReaderListener2 get_listener() {
    DataReaderListener2 ret = new DataReaderListener2(cpscsharpmodulePINVOKE.DataReader2_get_listener(swigCPtr), true);
    return ret;
  }

  public TopicDescription2 get_topicdescription() {
    TopicDescription2 ret = new TopicDescription2(cpscsharpmodulePINVOKE.DataReader2_get_topicdescription(swigCPtr), true);
    return ret;
  }

  public Subscriber2 get_subscriber() {
    Subscriber2 ret = new Subscriber2(cpscsharpmodulePINVOKE.DataReader2_get_subscriber(swigCPtr), true);
    return ret;
  }

  public int get_sample_rejected_status(SampleRejectedStatus status) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_sample_rejected_status(swigCPtr, SampleRejectedStatus.getCPtr(status));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_liveliness_changed_status(LivelinessChangedStatus status) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_liveliness_changed_status(swigCPtr, LivelinessChangedStatus.getCPtr(status));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_requested_deadline_missed_status(RequestedDeadlineMissedStatus status) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_requested_deadline_missed_status(swigCPtr, RequestedDeadlineMissedStatus.getCPtr(status));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_requested_incompatible_qos_status(RequestedIncompatibleQosStatus status) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_requested_incompatible_qos_status(swigCPtr, RequestedIncompatibleQosStatus.getCPtr(status));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_subscription_matched_status(SubscriptionMatchedStatus status) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_subscription_matched_status(swigCPtr, SubscriptionMatchedStatus.getCPtr(status));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_sample_lost_status(SampleLostStatus status) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_sample_lost_status(swigCPtr, SampleLostStatus.getCPtr(status));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int wait_for_historical_data(Duration_t max_wait) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_wait_for_historical_data(swigCPtr, Duration_t.getCPtr(max_wait));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_matched_publications(InstanceHandleSeq publication_handles) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_matched_publications(swigCPtr, InstanceHandleSeq.getCPtr(publication_handles));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_matched_publication_data(PublicationBuiltinTopicData publication_data, int publication_handle) {
    int ret = cpscsharpmodulePINVOKE.DataReader2_get_matched_publication_data(swigCPtr, PublicationBuiltinTopicData.getCPtr(publication_data), publication_handle);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}
