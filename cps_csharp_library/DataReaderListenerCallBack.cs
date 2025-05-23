/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class DataReaderListenerCallBack : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal DataReaderListenerCallBack(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(DataReaderListenerCallBack obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~DataReaderListenerCallBack() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_DataReaderListenerCallBack(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public virtual void on_inconsistent_topic(Topic the_topic, InconsistentTopicStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_inconsistent_topic(swigCPtr, Topic.getCPtr(the_topic), InconsistentTopicStatus.getCPtr(status));
  }

  public virtual void on_offered_deadline_missed(DataWriter writer, OfferedDeadlineMissedStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_offered_deadline_missed(swigCPtr, DataWriter.getCPtr(writer), OfferedDeadlineMissedStatus.getCPtr(status));
  }

  public virtual void on_offered_incompatible_qos(DataWriter writer, OfferedIncompatibleQosStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_offered_incompatible_qos(swigCPtr, DataWriter.getCPtr(writer), OfferedIncompatibleQosStatus.getCPtr(status));
  }

  public virtual void on_liveliness_lost(DataWriter writer, LivelinessLostStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_liveliness_lost(swigCPtr, DataWriter.getCPtr(writer), LivelinessLostStatus.getCPtr(status));
  }

  public virtual void on_publication_matched(DataWriter writer, PublicationMatchedStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_publication_matched(swigCPtr, DataWriter.getCPtr(writer), PublicationMatchedStatus.getCPtr(status));
  }

  public virtual void on_requested_deadline_missed(DataReader the_reader, RequestedDeadlineMissedStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_requested_deadline_missed(swigCPtr, DataReader.getCPtr(the_reader), RequestedDeadlineMissedStatus.getCPtr(status));
  }

  public virtual void on_requested_incompatible_qos(DataReader the_reader, RequestedIncompatibleQosStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_requested_incompatible_qos(swigCPtr, DataReader.getCPtr(the_reader), RequestedIncompatibleQosStatus.getCPtr(status));
  }

  public virtual void on_sample_rejected(DataReader the_reader, SampleRejectedStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_sample_rejected(swigCPtr, DataReader.getCPtr(the_reader), SampleRejectedStatus.getCPtr(status));
  }

  public virtual void on_liveliness_changed(DataReader the_reader, LivelinessChangedStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_liveliness_changed(swigCPtr, DataReader.getCPtr(the_reader), LivelinessChangedStatus.getCPtr(status));
  }

  public virtual void on_data_available(DataReader p_reader) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_data_available(swigCPtr, DataReader.getCPtr(p_reader));
  }

  public virtual void on_subscription_matched(DataReader the_reader, SubscriptionMatchedStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_subscription_matched(swigCPtr, DataReader.getCPtr(the_reader), SubscriptionMatchedStatus.getCPtr(status));
  }

  public virtual void on_sample_lost(DataReader the_reader, SampleLostStatus status) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_sample_lost(swigCPtr, DataReader.getCPtr(the_reader), SampleLostStatus.getCPtr(status));
  }

  public virtual void on_data_on_readers(Subscriber p_subscriber) {
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_on_data_on_readers(swigCPtr, Subscriber.getCPtr(p_subscriber));
  }

  public DataReaderListenerCallBack() : this(cpscsharpmodulePINVOKE.new_DataReaderListenerCallBack(), true) {
    SwigDirectorConnect();
  }

  private void SwigDirectorConnect() {
    if (SwigDerivedClassHasMethod("on_inconsistent_topic", swigMethodTypes0))
      swigDelegate0 = new SwigDelegateDataReaderListenerCallBack_0(SwigDirectoron_inconsistent_topic);
    if (SwigDerivedClassHasMethod("on_offered_deadline_missed", swigMethodTypes1))
      swigDelegate1 = new SwigDelegateDataReaderListenerCallBack_1(SwigDirectoron_offered_deadline_missed);
    if (SwigDerivedClassHasMethod("on_offered_incompatible_qos", swigMethodTypes2))
      swigDelegate2 = new SwigDelegateDataReaderListenerCallBack_2(SwigDirectoron_offered_incompatible_qos);
    if (SwigDerivedClassHasMethod("on_liveliness_lost", swigMethodTypes3))
      swigDelegate3 = new SwigDelegateDataReaderListenerCallBack_3(SwigDirectoron_liveliness_lost);
    if (SwigDerivedClassHasMethod("on_publication_matched", swigMethodTypes4))
      swigDelegate4 = new SwigDelegateDataReaderListenerCallBack_4(SwigDirectoron_publication_matched);
    if (SwigDerivedClassHasMethod("on_requested_deadline_missed", swigMethodTypes5))
      swigDelegate5 = new SwigDelegateDataReaderListenerCallBack_5(SwigDirectoron_requested_deadline_missed);
    if (SwigDerivedClassHasMethod("on_requested_incompatible_qos", swigMethodTypes6))
      swigDelegate6 = new SwigDelegateDataReaderListenerCallBack_6(SwigDirectoron_requested_incompatible_qos);
    if (SwigDerivedClassHasMethod("on_sample_rejected", swigMethodTypes7))
      swigDelegate7 = new SwigDelegateDataReaderListenerCallBack_7(SwigDirectoron_sample_rejected);
    if (SwigDerivedClassHasMethod("on_liveliness_changed", swigMethodTypes8))
      swigDelegate8 = new SwigDelegateDataReaderListenerCallBack_8(SwigDirectoron_liveliness_changed);
    if (SwigDerivedClassHasMethod("on_data_available", swigMethodTypes9))
      swigDelegate9 = new SwigDelegateDataReaderListenerCallBack_9(SwigDirectoron_data_available);
    if (SwigDerivedClassHasMethod("on_subscription_matched", swigMethodTypes10))
      swigDelegate10 = new SwigDelegateDataReaderListenerCallBack_10(SwigDirectoron_subscription_matched);
    if (SwigDerivedClassHasMethod("on_sample_lost", swigMethodTypes11))
      swigDelegate11 = new SwigDelegateDataReaderListenerCallBack_11(SwigDirectoron_sample_lost);
    if (SwigDerivedClassHasMethod("on_data_on_readers", swigMethodTypes12))
      swigDelegate12 = new SwigDelegateDataReaderListenerCallBack_12(SwigDirectoron_data_on_readers);
    cpscsharpmodulePINVOKE.DataReaderListenerCallBack_director_connect(swigCPtr, swigDelegate0, swigDelegate1, swigDelegate2, swigDelegate3, swigDelegate4, swigDelegate5, swigDelegate6, swigDelegate7, swigDelegate8, swigDelegate9, swigDelegate10, swigDelegate11, swigDelegate12);
  }

  private bool SwigDerivedClassHasMethod(string methodName, Type[] methodTypes) {
    System.Reflection.MethodInfo methodInfo = this.GetType().GetMethod(methodName, System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance, null, methodTypes, null);
    bool hasDerivedMethod = methodInfo.DeclaringType.IsSubclassOf(typeof(DataReaderListenerCallBack));
    return hasDerivedMethod;
  }

  private void SwigDirectoron_inconsistent_topic(IntPtr the_topic, IntPtr status) {
    on_inconsistent_topic((the_topic == IntPtr.Zero) ? null : new Topic(the_topic, false), (status == IntPtr.Zero) ? null : new InconsistentTopicStatus(status, false));
  }

  private void SwigDirectoron_offered_deadline_missed(IntPtr writer, IntPtr status) {
    on_offered_deadline_missed((writer == IntPtr.Zero) ? null : new DataWriter(writer, false), (status == IntPtr.Zero) ? null : new OfferedDeadlineMissedStatus(status, false));
  }

  private void SwigDirectoron_offered_incompatible_qos(IntPtr writer, IntPtr status) {
    on_offered_incompatible_qos((writer == IntPtr.Zero) ? null : new DataWriter(writer, false), (status == IntPtr.Zero) ? null : new OfferedIncompatibleQosStatus(status, false));
  }

  private void SwigDirectoron_liveliness_lost(IntPtr writer, IntPtr status) {
    on_liveliness_lost((writer == IntPtr.Zero) ? null : new DataWriter(writer, false), (status == IntPtr.Zero) ? null : new LivelinessLostStatus(status, false));
  }

  private void SwigDirectoron_publication_matched(IntPtr writer, IntPtr status) {
    on_publication_matched((writer == IntPtr.Zero) ? null : new DataWriter(writer, false), (status == IntPtr.Zero) ? null : new PublicationMatchedStatus(status, false));
  }

  private void SwigDirectoron_requested_deadline_missed(IntPtr the_reader, IntPtr status) {
    on_requested_deadline_missed((the_reader == IntPtr.Zero) ? null : new DataReader(the_reader, false), (status == IntPtr.Zero) ? null : new RequestedDeadlineMissedStatus(status, false));
  }

  private void SwigDirectoron_requested_incompatible_qos(IntPtr the_reader, IntPtr status) {
    on_requested_incompatible_qos((the_reader == IntPtr.Zero) ? null : new DataReader(the_reader, false), (status == IntPtr.Zero) ? null : new RequestedIncompatibleQosStatus(status, false));
  }

  private void SwigDirectoron_sample_rejected(IntPtr the_reader, IntPtr status) {
    on_sample_rejected((the_reader == IntPtr.Zero) ? null : new DataReader(the_reader, false), (status == IntPtr.Zero) ? null : new SampleRejectedStatus(status, false));
  }

  private void SwigDirectoron_liveliness_changed(IntPtr the_reader, IntPtr status) {
    on_liveliness_changed((the_reader == IntPtr.Zero) ? null : new DataReader(the_reader, false), (status == IntPtr.Zero) ? null : new LivelinessChangedStatus(status, false));
  }

  private void SwigDirectoron_data_available(IntPtr p_reader) {
    on_data_available((p_reader == IntPtr.Zero) ? null : new DataReader(p_reader, false));
  }

  private void SwigDirectoron_subscription_matched(IntPtr the_reader, IntPtr status) {
    on_subscription_matched((the_reader == IntPtr.Zero) ? null : new DataReader(the_reader, false), (status == IntPtr.Zero) ? null : new SubscriptionMatchedStatus(status, false));
  }

  private void SwigDirectoron_sample_lost(IntPtr the_reader, IntPtr status) {
    on_sample_lost((the_reader == IntPtr.Zero) ? null : new DataReader(the_reader, false), (status == IntPtr.Zero) ? null : new SampleLostStatus(status, false));
  }

  private void SwigDirectoron_data_on_readers(IntPtr p_subscriber) {
    on_data_on_readers((p_subscriber == IntPtr.Zero) ? null : new Subscriber(p_subscriber, false));
  }

  public delegate void SwigDelegateDataReaderListenerCallBack_0(IntPtr the_topic, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_1(IntPtr writer, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_2(IntPtr writer, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_3(IntPtr writer, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_4(IntPtr writer, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_5(IntPtr the_reader, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_6(IntPtr the_reader, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_7(IntPtr the_reader, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_8(IntPtr the_reader, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_9(IntPtr p_reader);
  public delegate void SwigDelegateDataReaderListenerCallBack_10(IntPtr the_reader, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_11(IntPtr the_reader, IntPtr status);
  public delegate void SwigDelegateDataReaderListenerCallBack_12(IntPtr p_subscriber);

  private SwigDelegateDataReaderListenerCallBack_0 swigDelegate0;
  private SwigDelegateDataReaderListenerCallBack_1 swigDelegate1;
  private SwigDelegateDataReaderListenerCallBack_2 swigDelegate2;
  private SwigDelegateDataReaderListenerCallBack_3 swigDelegate3;
  private SwigDelegateDataReaderListenerCallBack_4 swigDelegate4;
  private SwigDelegateDataReaderListenerCallBack_5 swigDelegate5;
  private SwigDelegateDataReaderListenerCallBack_6 swigDelegate6;
  private SwigDelegateDataReaderListenerCallBack_7 swigDelegate7;
  private SwigDelegateDataReaderListenerCallBack_8 swigDelegate8;
  private SwigDelegateDataReaderListenerCallBack_9 swigDelegate9;
  private SwigDelegateDataReaderListenerCallBack_10 swigDelegate10;
  private SwigDelegateDataReaderListenerCallBack_11 swigDelegate11;
  private SwigDelegateDataReaderListenerCallBack_12 swigDelegate12;

  private static Type[] swigMethodTypes0 = new Type[] { typeof(Topic), typeof(InconsistentTopicStatus) };
  private static Type[] swigMethodTypes1 = new Type[] { typeof(DataWriter), typeof(OfferedDeadlineMissedStatus) };
  private static Type[] swigMethodTypes2 = new Type[] { typeof(DataWriter), typeof(OfferedIncompatibleQosStatus) };
  private static Type[] swigMethodTypes3 = new Type[] { typeof(DataWriter), typeof(LivelinessLostStatus) };
  private static Type[] swigMethodTypes4 = new Type[] { typeof(DataWriter), typeof(PublicationMatchedStatus) };
  private static Type[] swigMethodTypes5 = new Type[] { typeof(DataReader), typeof(RequestedDeadlineMissedStatus) };
  private static Type[] swigMethodTypes6 = new Type[] { typeof(DataReader), typeof(RequestedIncompatibleQosStatus) };
  private static Type[] swigMethodTypes7 = new Type[] { typeof(DataReader), typeof(SampleRejectedStatus) };
  private static Type[] swigMethodTypes8 = new Type[] { typeof(DataReader), typeof(LivelinessChangedStatus) };
  private static Type[] swigMethodTypes9 = new Type[] { typeof(DataReader) };
  private static Type[] swigMethodTypes10 = new Type[] { typeof(DataReader), typeof(SubscriptionMatchedStatus) };
  private static Type[] swigMethodTypes11 = new Type[] { typeof(DataReader), typeof(SampleLostStatus) };
  private static Type[] swigMethodTypes12 = new Type[] { typeof(Subscriber) };
}
