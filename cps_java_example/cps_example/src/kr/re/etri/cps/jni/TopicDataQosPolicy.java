/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class TopicDataQosPolicy {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected TopicDataQosPolicy(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(TopicDataQosPolicy obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_TopicDataQosPolicy(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setValue(StringSeq value) {
    cpsjavamoduleJNI.TopicDataQosPolicy_value_set(swigCPtr, this, StringSeq.getCPtr(value), value);
  }

  public StringSeq getValue() {
    long cPtr = cpsjavamoduleJNI.TopicDataQosPolicy_value_get(swigCPtr, this);
    return (cPtr == 0) ? null : new StringSeq(cPtr, false);
  }

  public TopicDataQosPolicy() {
    this(cpsjavamoduleJNI.new_TopicDataQosPolicy(), true);
  }

}
