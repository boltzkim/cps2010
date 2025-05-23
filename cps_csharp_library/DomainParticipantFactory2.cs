/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class DomainParticipantFactory2 : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal DomainParticipantFactory2(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(DomainParticipantFactory2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~DomainParticipantFactory2() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_DomainParticipantFactory2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public DomainParticipantFactory2() : this(cpscsharpmodulePINVOKE.new_DomainParticipantFactory2(), true) {
  }

  public static DomainParticipantFactory2 get_instance() {
    DomainParticipantFactory2 ret = new DomainParticipantFactory2(cpscsharpmodulePINVOKE.DomainParticipantFactory2_get_instance(), true);
    return ret;
  }

  public DomainParticipant2 create_participant(int domain_id, DomainParticipantQos qos, DomainParticipantListener2 listener, uint mask) {
    DomainParticipant2 ret = new DomainParticipant2(cpscsharpmodulePINVOKE.DomainParticipantFactory2_create_participant(swigCPtr, domain_id, DomainParticipantQos.getCPtr(qos), DomainParticipantListener2.getCPtr(listener), mask), true);
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int delete_participant(DomainParticipant2 participant) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipantFactory2_delete_participant(swigCPtr, DomainParticipant2.getCPtr(participant));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public DomainParticipant2 lookup_participant(int domain_id) {
    DomainParticipant2 ret = new DomainParticipant2(cpscsharpmodulePINVOKE.DomainParticipantFactory2_lookup_participant(swigCPtr, domain_id), true);
    return ret;
  }

  public int set_default_participant_qos(DomainParticipantQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipantFactory2_set_default_participant_qos(swigCPtr, DomainParticipantQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_default_participant_qos(DomainParticipantQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipantFactory2_get_default_participant_qos(swigCPtr, DomainParticipantQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int set_qos(DomainParticipantFactoryQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipantFactory2_set_qos(swigCPtr, DomainParticipantFactoryQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int get_qos(DomainParticipantFactoryQos qos) {
    int ret = cpscsharpmodulePINVOKE.DomainParticipantFactory2_get_qos(swigCPtr, DomainParticipantFactoryQos.getCPtr(qos));
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}
