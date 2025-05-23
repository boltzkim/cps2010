/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class TopicListener2 : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal TopicListener2(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(TopicListener2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~TopicListener2() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_TopicListener2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public TopicListener2(TopicListener p_listener) : this(cpscsharpmodulePINVOKE.new_TopicListener2__SWIG_0(TopicListener.getCPtr(p_listener)), true) {
  }

  public TopicListener2() : this(cpscsharpmodulePINVOKE.new_TopicListener2__SWIG_1(), true) {
  }

  public TopicListener getListener() {
    TopicListener ret = new TopicListener(cpscsharpmodulePINVOKE.TopicListener2_getListener(swigCPtr), true);
    return ret;
  }

  public TopicListener getListenerPtr() {
    IntPtr cPtr = cpscsharpmodulePINVOKE.TopicListener2_getListenerPtr(swigCPtr);
    TopicListener ret = (cPtr == IntPtr.Zero) ? null : new TopicListener(cPtr, false);
    return ret;
  }

  public void setCallback(TopicListenerCallBack cb) {
    cpscsharpmodulePINVOKE.TopicListener2_setCallback(swigCPtr, TopicListenerCallBack.getCPtr(cb));
  }

  public static TopicListenerCallBack _callback {
    set {
      cpscsharpmodulePINVOKE.TopicListener2__callback_set(TopicListenerCallBack.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.TopicListener2__callback_get();
      TopicListenerCallBack ret = (cPtr == IntPtr.Zero) ? null : new TopicListenerCallBack(cPtr, false);
      return ret;
    } 
  }

}
