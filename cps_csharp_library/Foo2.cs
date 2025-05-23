/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class Foo2 : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Foo2(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(Foo2 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~Foo2() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_Foo2(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public Foo2(IntPtr data) : this(cpscsharpmodulePINVOKE.new_Foo2__SWIG_0(new HandleRef(null, data)), true) {
  }

  public Foo2() : this(cpscsharpmodulePINVOKE.new_Foo2__SWIG_1(), true) {
  }

  public Foo2(FooTypeSupport2 fooTS1) : this(cpscsharpmodulePINVOKE.new_Foo2__SWIG_2(FooTypeSupport2.getCPtr(fooTS1)), true) {
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
  }

  public Foo2(FooTypeSupport2 fooTS1, IntPtr data) : this(cpscsharpmodulePINVOKE.new_Foo2__SWIG_3(FooTypeSupport2.getCPtr(fooTS1), new HandleRef(null, data)), true) {
    if (cpscsharpmodulePINVOKE.SWIGPendingException.Pending) throw cpscsharpmodulePINVOKE.SWIGPendingException.Retrieve();
  }

  public void removeFoo() {
    cpscsharpmodulePINVOKE.Foo2_removeFoo(swigCPtr);
  }

  public Foo getFoo() {
    IntPtr cPtr = cpscsharpmodulePINVOKE.Foo2_getFoo(swigCPtr);
    Foo ret = (cPtr == IntPtr.Zero) ? null : new Foo(cPtr, false);
    return ret;
  }

  public FooRet setString(string parameter, string value) {
    FooRet ret = (FooRet)cpscsharpmodulePINVOKE.Foo2_setString(swigCPtr, parameter, value);
    return ret;
  }

  public FooRet setInt(string parameter, int value) {
    FooRet ret = (FooRet)cpscsharpmodulePINVOKE.Foo2_setInt(swigCPtr, parameter, value);
    return ret;
  }

  public FooRet setFloat(string parameter, float value) {
    FooRet ret = (FooRet)cpscsharpmodulePINVOKE.Foo2_setFloat(swigCPtr, parameter, value);
    return ret;
  }

  public string getString(string parameter) {
    string ret = cpscsharpmodulePINVOKE.Foo2_getString(swigCPtr, parameter);
    return ret;
  }

  public int getInt(string parameter) {
    int ret = cpscsharpmodulePINVOKE.Foo2_getInt(swigCPtr, parameter);
    return ret;
  }

  public float getFloat(string parameter) {
    float ret = cpscsharpmodulePINVOKE.Foo2_getFloat(swigCPtr, parameter);
    return ret;
  }

}
