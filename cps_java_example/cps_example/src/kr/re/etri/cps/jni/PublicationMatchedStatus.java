/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class PublicationMatchedStatus {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected PublicationMatchedStatus(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(PublicationMatchedStatus obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_PublicationMatchedStatus(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setTotal_count(int value) {
    cpsjavamoduleJNI.PublicationMatchedStatus_total_count_set(swigCPtr, this, value);
  }

  public int getTotal_count() {
    return cpsjavamoduleJNI.PublicationMatchedStatus_total_count_get(swigCPtr, this);
  }

  public void setTotal_count_change(int value) {
    cpsjavamoduleJNI.PublicationMatchedStatus_total_count_change_set(swigCPtr, this, value);
  }

  public int getTotal_count_change() {
    return cpsjavamoduleJNI.PublicationMatchedStatus_total_count_change_get(swigCPtr, this);
  }

  public void setCurrent_count(int value) {
    cpsjavamoduleJNI.PublicationMatchedStatus_current_count_set(swigCPtr, this, value);
  }

  public int getCurrent_count() {
    return cpsjavamoduleJNI.PublicationMatchedStatus_current_count_get(swigCPtr, this);
  }

  public void setCurrent_count_change(int value) {
    cpsjavamoduleJNI.PublicationMatchedStatus_current_count_change_set(swigCPtr, this, value);
  }

  public int getCurrent_count_change() {
    return cpsjavamoduleJNI.PublicationMatchedStatus_current_count_change_get(swigCPtr, this);
  }

  public void setLast_subscription_handle(int value) {
    cpsjavamoduleJNI.PublicationMatchedStatus_last_subscription_handle_set(swigCPtr, this, value);
  }

  public int getLast_subscription_handle() {
    return cpsjavamoduleJNI.PublicationMatchedStatus_last_subscription_handle_get(swigCPtr, this);
  }

  public PublicationMatchedStatus() {
    this(cpsjavamoduleJNI.new_PublicationMatchedStatus(), true);
  }

}
