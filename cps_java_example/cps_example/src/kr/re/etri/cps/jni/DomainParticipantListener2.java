/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class DomainParticipantListener2 {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected DomainParticipantListener2(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(DomainParticipantListener2 obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_DomainParticipantListener2(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public DomainParticipantListener2(DomainParticipantListener p_listener) {
    this(cpsjavamoduleJNI.new_DomainParticipantListener2__SWIG_0(DomainParticipantListener.getCPtr(p_listener), p_listener), true);
  }

  public DomainParticipantListener2() {
    this(cpsjavamoduleJNI.new_DomainParticipantListener2__SWIG_1(), true);
  }

  public DomainParticipantListener getListener() {
    return new DomainParticipantListener(cpsjavamoduleJNI.DomainParticipantListener2_getListener(swigCPtr, this), true);
  }

  public DomainParticipantListener getListenerPtr() {
    long cPtr = cpsjavamoduleJNI.DomainParticipantListener2_getListenerPtr(swigCPtr, this);
    return (cPtr == 0) ? null : new DomainParticipantListener(cPtr, false);
  }

  public void setCallback(DomainParticipantListenerCallBack cb) {
    cpsjavamoduleJNI.DomainParticipantListener2_setCallback(swigCPtr, this, DomainParticipantListenerCallBack.getCPtr(cb), cb);
  }

  public static void set_callback(DomainParticipantListenerCallBack value) {
    cpsjavamoduleJNI.DomainParticipantListener2__callback_set(DomainParticipantListenerCallBack.getCPtr(value), value);
  }

  public static DomainParticipantListenerCallBack get_callback() {
    long cPtr = cpsjavamoduleJNI.DomainParticipantListener2__callback_get();
    return (cPtr == 0) ? null : new DomainParticipantListenerCallBack(cPtr, false);
  }

}
