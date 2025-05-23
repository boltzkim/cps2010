/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class DataWriterListener2 : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal DataWriterListener2(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(DataWriterListener2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~DataWriterListener2() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_DataWriterListener2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public DataWriterListener2(DataWriterListener p_listener) : this(cpscsharpmodulePINVOKE.new_DataWriterListener2__SWIG_0(DataWriterListener.getCPtr(p_listener)), true) {
  }

  public DataWriterListener2() : this(cpscsharpmodulePINVOKE.new_DataWriterListener2__SWIG_1(), true) {
  }

  public DataWriterListener getListener() {
    DataWriterListener ret = new DataWriterListener(cpscsharpmodulePINVOKE.DataWriterListener2_getListener(swigCPtr), true);
    return ret;
  }

  public DataWriterListener getListenerPtr() {
    IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterListener2_getListenerPtr(swigCPtr);
    DataWriterListener ret = (cPtr == IntPtr.Zero) ? null : new DataWriterListener(cPtr, false);
    return ret;
  }

  public void setCallback(DataWriterListenerCallBack cb) {
    cpscsharpmodulePINVOKE.DataWriterListener2_setCallback(swigCPtr, DataWriterListenerCallBack.getCPtr(cb));
  }

  public static DataWriterListenerCallBack _callback {
    set {
      cpscsharpmodulePINVOKE.DataWriterListener2__callback_set(DataWriterListenerCallBack.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterListener2__callback_get();
      DataWriterListenerCallBack ret = (cPtr == IntPtr.Zero) ? null : new DataWriterListenerCallBack(cPtr, false);
      return ret;
    } 
  }

}
