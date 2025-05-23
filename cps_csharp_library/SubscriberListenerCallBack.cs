/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class SubscriberListenerCallBack : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal SubscriberListenerCallBack(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(SubscriberListenerCallBack obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~SubscriberListenerCallBack() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_SubscriberListenerCallBack(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public virtual void on_requested_deadline_missed(DataReader the_reader, RequestedDeadlineMissedStatus status) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_requested_deadline_missed(swigCPtr, DataReader.getCPtr(the_reader), RequestedDeadlineMissedStatus.getCPtr(status));
  }

  public virtual void on_requested_incompatible_qos(DataReader the_reader, RequestedIncompatibleQosStatus status) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_requested_incompatible_qos(swigCPtr, DataReader.getCPtr(the_reader), RequestedIncompatibleQosStatus.getCPtr(status));
  }

  public virtual void on_sample_rejected(DataReader the_reader, SampleRejectedStatus status) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_sample_rejected(swigCPtr, DataReader.getCPtr(the_reader), SampleRejectedStatus.getCPtr(status));
  }

  public virtual void on_liveliness_changed(DataReader the_reader, LivelinessChangedStatus status) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_liveliness_changed(swigCPtr, DataReader.getCPtr(the_reader), LivelinessChangedStatus.getCPtr(status));
  }

  public virtual void on_data_available(DataReader p_reader) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_data_available(swigCPtr, DataReader.getCPtr(p_reader));
  }

  public virtual void on_subscription_matched(DataReader the_reader, SubscriptionMatchedStatus status) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_subscription_matched(swigCPtr, DataReader.getCPtr(the_reader), SubscriptionMatchedStatus.getCPtr(status));
  }

  public virtual void on_sample_lost(DataReader the_reader, SampleLostStatus status) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_sample_lost(swigCPtr, DataReader.getCPtr(the_reader), SampleLostStatus.getCPtr(status));
  }

  public virtual void on_data_on_readers(Subscriber p_subscriber) {
    cpscsharpmodulePINVOKE.SubscriberListenerCallBack_on_data_on_readers(swigCPtr, Subscriber.getCPtr(p_subscriber));
  }

}
