/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class QosPolicyCount {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected QosPolicyCount(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(QosPolicyCount obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_QosPolicyCount(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setPolicy_id(int value) {
    cpsjavamoduleJNI.QosPolicyCount_policy_id_set(swigCPtr, this, value);
  }

  public int getPolicy_id() {
    return cpsjavamoduleJNI.QosPolicyCount_policy_id_get(swigCPtr, this);
  }

  public void setCount(int value) {
    cpsjavamoduleJNI.QosPolicyCount_count_set(swigCPtr, this, value);
  }

  public int getCount() {
    return cpsjavamoduleJNI.QosPolicyCount_count_get(swigCPtr, this);
  }

  public QosPolicyCount() {
    this(cpsjavamoduleJNI.new_QosPolicyCount(), true);
  }

}
