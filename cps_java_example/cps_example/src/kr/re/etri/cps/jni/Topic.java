/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class Topic {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected Topic(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Topic obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_Topic(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setI_entity_type(int value) {
    cpsjavamoduleJNI.Topic_i_entity_type_set(swigCPtr, this, value);
  }

  public int getI_entity_type() {
    return cpsjavamoduleJNI.Topic_i_entity_type_get(swigCPtr, this);
  }

  public Topic() {
    this(cpsjavamoduleJNI.new_Topic(), true);
  }

}
