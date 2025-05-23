/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class OwnershipQosPolicy : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal OwnershipQosPolicy(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(OwnershipQosPolicy obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~OwnershipQosPolicy() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_OwnershipQosPolicy(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public OwnershipQosPolicyKind kind {
    set {
      cpscsharpmodulePINVOKE.OwnershipQosPolicy_kind_set(swigCPtr, (int)value);
    } 
    get {
      OwnershipQosPolicyKind ret = (OwnershipQosPolicyKind)cpscsharpmodulePINVOKE.OwnershipQosPolicy_kind_get(swigCPtr);
      return ret;
    } 
  }

  public OwnershipQosPolicy() : this(cpscsharpmodulePINVOKE.new_OwnershipQosPolicy(), true) {
  }

}
