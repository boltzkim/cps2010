/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class InconsistentTopicStatus : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal InconsistentTopicStatus(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(InconsistentTopicStatus obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~InconsistentTopicStatus() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_InconsistentTopicStatus(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public int total_count {
    set {
      cpscsharpmodulePINVOKE.InconsistentTopicStatus_total_count_set(swigCPtr, value);
    } 
    get {
      int ret = cpscsharpmodulePINVOKE.InconsistentTopicStatus_total_count_get(swigCPtr);
      return ret;
    } 
  }

  public int total_count_change {
    set {
      cpscsharpmodulePINVOKE.InconsistentTopicStatus_total_count_change_set(swigCPtr, value);
    } 
    get {
      int ret = cpscsharpmodulePINVOKE.InconsistentTopicStatus_total_count_change_get(swigCPtr);
      return ret;
    } 
  }

  public InconsistentTopicStatus() : this(cpscsharpmodulePINVOKE.new_InconsistentTopicStatus(), true) {
  }

}
