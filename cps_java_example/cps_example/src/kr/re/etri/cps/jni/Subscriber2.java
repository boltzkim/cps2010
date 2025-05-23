/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class Subscriber2 extends Entity2 {
  private long swigCPtr;

  protected Subscriber2(long cPtr, boolean cMemoryOwn) {
    super(cpsjavamoduleJNI.Subscriber2_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Subscriber2 obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_Subscriber2(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public Subscriber2(Subscriber subscriber) {
    this(cpsjavamoduleJNI.new_Subscriber2(Subscriber.getCPtr(subscriber), subscriber), true);
  }

  public Subscriber getSubscriber() {
    long cPtr = cpsjavamoduleJNI.Subscriber2_getSubscriber(swigCPtr, this);
    return (cPtr == 0) ? null : new Subscriber(cPtr, false);
  }

  public FooDataReader2 create_datareader(Topic2 topic, DataReaderQos qos, DataReaderListener2 listener, long mask) {
    return new FooDataReader2(cpsjavamoduleJNI.Subscriber2_create_datareader(swigCPtr, this, Topic2.getCPtr(topic), topic, DataReaderQos.getCPtr(qos), qos, DataReaderListener2.getCPtr(listener), listener, mask), true);
  }

  public int delete_datareader(DataReader2 datareader) {
    return cpsjavamoduleJNI.Subscriber2_delete_datareader(swigCPtr, this, DataReader2.getCPtr(datareader), datareader);
  }

  public int delete_contained_entities() {
    return cpsjavamoduleJNI.Subscriber2_delete_contained_entities(swigCPtr, this);
  }

  public FooDataReader2 lookup_datareader(String topic_name) {
    return new FooDataReader2(cpsjavamoduleJNI.Subscriber2_lookup_datareader(swigCPtr, this, topic_name), true);
  }

  public int get_datareaders(DataReaderSeq readers, long sample_states, long view_states, long instance_states) {
    return cpsjavamoduleJNI.Subscriber2_get_datareaders(swigCPtr, this, DataReaderSeq.getCPtr(readers), readers, sample_states, view_states, instance_states);
  }

  public int notify_datareaders() {
    return cpsjavamoduleJNI.Subscriber2_notify_datareaders(swigCPtr, this);
  }

  public int set_qos(SubscriberQos qos) {
    return cpsjavamoduleJNI.Subscriber2_set_qos(swigCPtr, this, SubscriberQos.getCPtr(qos), qos);
  }

  public int get_qos(SubscriberQos qos) {
    return cpsjavamoduleJNI.Subscriber2_get_qos(swigCPtr, this, SubscriberQos.getCPtr(qos), qos);
  }

  public int set_listener(SubscriberListener2 listener, long mask) {
    return cpsjavamoduleJNI.Subscriber2_set_listener(swigCPtr, this, SubscriberListener2.getCPtr(listener), listener, mask);
  }

  public SubscriberListener2 get_listener() {
    return new SubscriberListener2(cpsjavamoduleJNI.Subscriber2_get_listener(swigCPtr, this), true);
  }

  public int begin_access() {
    return cpsjavamoduleJNI.Subscriber2_begin_access(swigCPtr, this);
  }

  public int end_access() {
    return cpsjavamoduleJNI.Subscriber2_end_access(swigCPtr, this);
  }

  public DomainParticipant2 get_participant() {
    return new DomainParticipant2(cpsjavamoduleJNI.Subscriber2_get_participant(swigCPtr, this), true);
  }

  public int set_default_datareader_qos(DataReaderQos qos) {
    return cpsjavamoduleJNI.Subscriber2_set_default_datareader_qos(swigCPtr, this, DataReaderQos.getCPtr(qos), qos);
  }

  public int get_default_datareader_qos(DataReaderQos qos) {
    return cpsjavamoduleJNI.Subscriber2_get_default_datareader_qos(swigCPtr, this, DataReaderQos.getCPtr(qos), qos);
  }

  public int copy_from_topic_qos(DataReaderQos datareader_qos, TopicQos topic_qos) {
    return cpsjavamoduleJNI.Subscriber2_copy_from_topic_qos(swigCPtr, this, DataReaderQos.getCPtr(datareader_qos), datareader_qos, TopicQos.getCPtr(topic_qos), topic_qos);
  }

}
