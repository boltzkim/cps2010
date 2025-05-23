/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class MultiTopic : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal MultiTopic(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(MultiTopic obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~MultiTopic() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_MultiTopic(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public int i_entity_type {
    set {
      cpscsharpmodulePINVOKE.MultiTopic_i_entity_type_set(swigCPtr, value);
    } 
    get {
      int ret = cpscsharpmodulePINVOKE.MultiTopic_i_entity_type_get(swigCPtr);
      return ret;
    } 
  }

  public MultiTopic() : this(cpscsharpmodulePINVOKE.new_MultiTopic(), true) {
  }

}
