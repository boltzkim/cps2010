/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class FooSeq2 : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal FooSeq2(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(FooSeq2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~FooSeq2() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_FooSeq2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public FooSeq2() : this(cpscsharpmodulePINVOKE.new_FooSeq2(), true) {
  }

  public FooSeq getFooSeq() {
    FooSeq ret = new FooSeq(cpscsharpmodulePINVOKE.FooSeq2_getFooSeq(swigCPtr), true);
    return ret;
  }

  public FooSeq getFooSeqPtr() {
    IntPtr cPtr = cpscsharpmodulePINVOKE.FooSeq2_getFooSeqPtr(swigCPtr);
    FooSeq ret = (cPtr == IntPtr.Zero) ? null : new FooSeq(cPtr, false);
    return ret;
  }

  public bool has_data() {
    bool ret = cpscsharpmodulePINVOKE.FooSeq2_has_data(swigCPtr);
    return ret;
  }

  public int getSize() {
    int ret = cpscsharpmodulePINVOKE.FooSeq2_getSize(swigCPtr);
    return ret;
  }

  public Message2 getMessage() {
    Message2 ret = new Message2(cpscsharpmodulePINVOKE.FooSeq2_getMessage(swigCPtr), true);
    return ret;
  }

}
