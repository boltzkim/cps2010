/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class WaitSet {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected WaitSet(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WaitSet obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_WaitSet(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setI_conditions(int value) {
    cpsjavamoduleJNI.WaitSet_i_conditions_set(swigCPtr, this, value);
  }

  public int getI_conditions() {
    return cpsjavamoduleJNI.WaitSet_i_conditions_get(swigCPtr, this);
  }

  public WaitSet() {
    this(cpsjavamoduleJNI.new_WaitSet(), true);
  }

}
