/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class DataReaderListener {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected DataReaderListener(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(DataReaderListener obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_DataReaderListener(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setNot_used(int value) {
    cpsjavamoduleJNI.DataReaderListener_not_used_set(swigCPtr, this, value);
  }

  public int getNot_used() {
    return cpsjavamoduleJNI.DataReaderListener_not_used_get(swigCPtr, this);
  }

  public DataReaderListener() {
    this(cpsjavamoduleJNI.new_DataReaderListener(), true);
  }

}
