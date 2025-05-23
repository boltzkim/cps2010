/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class DomainParticipant2 : Entity2 {
  private HandleRef swigCPtr;

  internal DomainParticipant2(IntPtr cPtr, bool cMemoryOwn) : base(cpscsharpmodulePINVOKE.DomainParticipant2_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(DomainParticipant2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~DomainParticipant2() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_DomainParticipant2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public DomainParticipant2(DomainParticipant domainparticipant) : this(cpscsharpmodulePINVOKE.new_DomainParticipant2(DomainParticipant.getCPtr(domainparticipant)), true) {
  }

  public DomainParticipant getDomainParticipant() {
    IntPtr cPtr = cpscsharpmodulePINVOKE.DomainParticipant2_getDomainParticipant(swigCPtr);
    DomainParticipant ret = (cPtr == IntPtr.Zero) ? null : new DomainParticipant(cPtr, false);
    return ret;
  }

  public Publisher2 create_publisher(PublisherQos qos, PublisherListener2 listener, uint mask) {
    Publisher2 ret = new Publisher2(cpscsharpmodulePINVOKE.DomainParticipant2_create_publisher(swigCPtr, PublisherQos.getCPtr(qos), PublisherListener2.getCPtr(listener), mask), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_publisher(Publisher2 publisher) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_delete_publisher(swigCPtr, Publisher2.getCPtr(publisher));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Subscriber2 create_subscriber(SubscriberQos qos, SubscriberListener2 listener, uint mask) {
    Subscriber2 ret = new Subscriber2(cpscsharpmodulePINVOKE.DomainParticipant2_create_subscriber(swigCPtr, SubscriberQos.getCPtr(qos), SubscriberListener2.getCPtr(listener), mask), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_subscriber(Subscriber2 subscriber) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_delete_subscriber(swigCPtr, Subscriber2.getCPtr(subscriber));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Subscriber2 get_builtin_subscriber() {
    Subscriber2 ret = new Subscriber2(cpscsharpmodulePINVOKE.DomainParticipant2_get_builtin_subscriber(swigCPtr), true);
    return ret;
  }

  public Topic2 create_topic(string topic_name, string type_name, TopicQos qos, TopicListener2 listener, uint mask) {
    Topic2 ret = new Topic2(cpscsharpmodulePINVOKE.DomainParticipant2_create_topic(swigCPtr, topic_name, type_name, TopicQos.getCPtr(qos), TopicListener2.getCPtr(listener), mask), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_topic(Topic2 topic) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_delete_topic(swigCPtr, Topic2.getCPtr(topic));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Topic2 find_topic(string topic_name, Duration_t timeout) {
    Topic2 ret = new Topic2(cpscsharpmodulePINVOKE.DomainParticipant2_find_topic(swigCPtr, topic_name, Duration_t.getCPtr(timeout)), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public TopicDescription2 lookup_topicdescription(string name) {
    TopicDescription2 ret = new TopicDescription2(cpscsharpmodulePINVOKE.DomainParticipant2_lookup_topicdescription(swigCPtr, name), true);
    return ret;
  }

  public ContentFilteredTopic2 create_contentfilteredtopic(string name, Topic2 related_topic, string filter_expression, StringSeq expression_parameters) {
    ContentFilteredTopic2 ret = new ContentFilteredTopic2(cpscsharpmodulePINVOKE.DomainParticipant2_create_contentfilteredtopic(swigCPtr, name, Topic2.getCPtr(related_topic), filter_expression, StringSeq.getCPtr(expression_parameters)), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_contentfilteredtopic(ContentFilteredTopic2 contentfilteredtopic) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_delete_contentfilteredtopic(swigCPtr, ContentFilteredTopic2.getCPtr(contentfilteredtopic));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MultiTopic2 create_multitopic(string name, string type_name, string subscription_expression, StringSeq expression_parameters) {
    MultiTopic2 ret = new MultiTopic2(cpscsharpmodulePINVOKE.DomainParticipant2_create_multitopic(swigCPtr, name, type_name, subscription_expression, StringSeq.getCPtr(expression_parameters)), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_multitopic(MultiTopic2 multitopic) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_delete_multitopic(swigCPtr, MultiTopic2.getCPtr(multitopic));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_contained_entities() {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_delete_contained_entities(swigCPtr);
    return ret;
  }

  public int set_qos(DomainParticipantQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_set_qos(swigCPtr, DomainParticipantQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_qos(DomainParticipantQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_qos(swigCPtr, DomainParticipantQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int set_listener(DomainParticipantListener2 listener, uint mask) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_set_listener(swigCPtr, DomainParticipantListener2.getCPtr(listener), mask);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public DomainParticipantListener2 get_listener() {
    DomainParticipantListener2 ret = new DomainParticipantListener2(cpscsharpmodulePINVOKE.DomainParticipant2_get_listener(swigCPtr), true);
    return ret;
  }

  public int ignore_participant(int handle) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_ignore_participant(swigCPtr, handle);
    return ret;
  }

  public int ignore_topic(int handle) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_ignore_topic(swigCPtr, handle);
    return ret;
  }

  public int ignore_publication(int handle) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_ignore_publication(swigCPtr, handle);
    return ret;
  }

  public int ignore_subscription(int handle) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_ignore_subscription(swigCPtr, handle);
    return ret;
  }

  public int get_domain_id(DomainParticipant domainparticipant) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_domain_id(swigCPtr, DomainParticipant.getCPtr(domainparticipant));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int assert_liveliness() {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_assert_liveliness(swigCPtr);
    return ret;
  }

  public int set_default_publisher_qos(PublisherQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_set_default_publisher_qos(swigCPtr, PublisherQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_default_publisher_qos(PublisherQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_default_publisher_qos(swigCPtr, PublisherQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int set_default_subscriber_qos(SubscriberQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_set_default_subscriber_qos(swigCPtr, SubscriberQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_default_subscriber_qos(SubscriberQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_default_subscriber_qos(swigCPtr, SubscriberQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int set_default_topic_qos(TopicQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_set_default_topic_qos(swigCPtr, TopicQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_default_topic_qos(TopicQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_default_topic_qos(swigCPtr, TopicQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_discovered_participants(InstanceHandleSeq participant_handles) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_discovered_participants(swigCPtr, InstanceHandleSeq.getCPtr(participant_handles));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_discovered_participant_data(ParticipantBuiltinTopicData participant_data, int participant_handle) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_discovered_participant_data(swigCPtr, ParticipantBuiltinTopicData.getCPtr(participant_data), participant_handle);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_discovered_topics(InstanceHandleSeq topic_handles) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_discovered_topics(swigCPtr, InstanceHandleSeq.getCPtr(topic_handles));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_discovered_topic_data(TopicBuiltinTopicData topic_data, int topic_handle) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_discovered_topic_data(swigCPtr, TopicBuiltinTopicData.getCPtr(topic_data), topic_handle);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool contains_entity(int a_handle) {
    bool ret = cpscsharpmodulePINVOKE.DomainParticipant2_contains_entity(swigCPtr, a_handle);
    return ret;
  }

  public int get_current_time(Time_t current_time) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipant2_get_current_time(swigCPtr, Time_t.getCPtr(current_time));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}
