/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class SampleRejectedStatus {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected SampleRejectedStatus(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(SampleRejectedStatus obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_SampleRejectedStatus(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setTotal_count(int value) {
    cpsjavamoduleJNI.SampleRejectedStatus_total_count_set(swigCPtr, this, value);
  }

  public int getTotal_count() {
    return cpsjavamoduleJNI.SampleRejectedStatus_total_count_get(swigCPtr, this);
  }

  public void setTotal_count_change(int value) {
    cpsjavamoduleJNI.SampleRejectedStatus_total_count_change_set(swigCPtr, this, value);
  }

  public int getTotal_count_change() {
    return cpsjavamoduleJNI.SampleRejectedStatus_total_count_change_get(swigCPtr, this);
  }

  public void setLast_reason(SampleRejectedStatusKind value) {
    cpsjavamoduleJNI.SampleRejectedStatus_last_reason_set(swigCPtr, this, value.swigValue());
  }

  public SampleRejectedStatusKind getLast_reason() {
    return SampleRejectedStatusKind.swigToEnum(cpsjavamoduleJNI.SampleRejectedStatus_last_reason_get(swigCPtr, this));
  }

  public void setLast_instance_handle(int value) {
    cpsjavamoduleJNI.SampleRejectedStatus_last_instance_handle_set(swigCPtr, this, value);
  }

  public int getLast_instance_handle() {
    return cpsjavamoduleJNI.SampleRejectedStatus_last_instance_handle_get(swigCPtr, this);
  }

  public SampleRejectedStatus() {
    this(cpsjavamoduleJNI.new_SampleRejectedStatus(), true);
  }

}
