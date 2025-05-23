/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class Publisher2 extends Entity2 {
  private long swigCPtr;

  protected Publisher2(long cPtr, boolean cMemoryOwn) {
    super(cpsjavamoduleJNI.Publisher2_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Publisher2 obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_Publisher2(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public Publisher2(Publisher publisher) {
    this(cpsjavamoduleJNI.new_Publisher2(Publisher.getCPtr(publisher), publisher), true);
  }

  public Publisher getPublisher() {
    long cPtr = cpsjavamoduleJNI.Publisher2_getPublisher(swigCPtr, this);
    return (cPtr == 0) ? null : new Publisher(cPtr, false);
  }

  public FooDataWriter2 create_datawriter(Topic2 topic, DataWriterQos qos, DataWriterListener2 listener, long mask) {
    return new FooDataWriter2(cpsjavamoduleJNI.Publisher2_create_datawriter(swigCPtr, this, Topic2.getCPtr(topic), topic, DataWriterQos.getCPtr(qos), qos, DataWriterListener2.getCPtr(listener), listener, mask), true);
  }

  public int delete_datawriter(DataWriter2 datawriter) {
    return cpsjavamoduleJNI.Publisher2_delete_datawriter(swigCPtr, this, DataWriter2.getCPtr(datawriter), datawriter);
  }

  public FooDataWriter2 lookup_datawriter(String topic_name) {
    return new FooDataWriter2(cpsjavamoduleJNI.Publisher2_lookup_datawriter(swigCPtr, this, topic_name), true);
  }

  public int delete_contained_entities() {
    return cpsjavamoduleJNI.Publisher2_delete_contained_entities(swigCPtr, this);
  }

  public int set_qos(PublisherQos qos) {
    return cpsjavamoduleJNI.Publisher2_set_qos(swigCPtr, this, PublisherQos.getCPtr(qos), qos);
  }

  public int get_qos(PublisherQos qos) {
    return cpsjavamoduleJNI.Publisher2_get_qos(swigCPtr, this, PublisherQos.getCPtr(qos), qos);
  }

  public int set_listener(PublisherListener2 listener, long mask) {
    return cpsjavamoduleJNI.Publisher2_set_listener(swigCPtr, this, PublisherListener2.getCPtr(listener), listener, mask);
  }

  public PublisherListener2 get_listener() {
    return new PublisherListener2(cpsjavamoduleJNI.Publisher2_get_listener(swigCPtr, this), true);
  }

  public int suspend_publications() {
    return cpsjavamoduleJNI.Publisher2_suspend_publications(swigCPtr, this);
  }

  public int resume_publications() {
    return cpsjavamoduleJNI.Publisher2_resume_publications(swigCPtr, this);
  }

  public int begin_coherent_changes() {
    return cpsjavamoduleJNI.Publisher2_begin_coherent_changes(swigCPtr, this);
  }

  public int end_coherent_changes() {
    return cpsjavamoduleJNI.Publisher2_end_coherent_changes(swigCPtr, this);
  }

  public int wait_for_acknowledgments(Duration_t max_wait) {
    return cpsjavamoduleJNI.Publisher2_wait_for_acknowledgments(swigCPtr, this, Duration_t.getCPtr(max_wait), max_wait);
  }

  public DomainParticipant2 get_participant() {
    return new DomainParticipant2(cpsjavamoduleJNI.Publisher2_get_participant(swigCPtr, this), true);
  }

  public int set_default_datawriter_qos(DataWriterQos qos) {
    return cpsjavamoduleJNI.Publisher2_set_default_datawriter_qos(swigCPtr, this, DataWriterQos.getCPtr(qos), qos);
  }

  public int get_default_datawriter_qos(DataWriterQos qos) {
    return cpsjavamoduleJNI.Publisher2_get_default_datawriter_qos(swigCPtr, this, DataWriterQos.getCPtr(qos), qos);
  }

  public int copy_from_topic_qos(DataWriterQos datawriter_qos, TopicQos topic_qos) {
    return cpsjavamoduleJNI.Publisher2_copy_from_topic_qos(swigCPtr, this, DataWriterQos.getCPtr(datawriter_qos), datawriter_qos, TopicQos.getCPtr(topic_qos), topic_qos);
  }

}
