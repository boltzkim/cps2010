/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class DataWriterStatus : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal DataWriterStatus(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(DataWriterStatus obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~DataWriterStatus() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_DataWriterStatus(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public OfferedDeadlineMissedStatus offered_deadline_missed {
    set {
      cpscsharpmodulePINVOKE.DataWriterStatus_offered_deadline_missed_set(swigCPtr, OfferedDeadlineMissedStatus.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterStatus_offered_deadline_missed_get(swigCPtr);
      OfferedDeadlineMissedStatus ret = (cPtr == IntPtr.Zero) ? null : new OfferedDeadlineMissedStatus(cPtr, false);
      return ret;
    } 
  }

  public OfferedIncompatibleQosStatus offered_incompatible_qos {
    set {
      cpscsharpmodulePINVOKE.DataWriterStatus_offered_incompatible_qos_set(swigCPtr, OfferedIncompatibleQosStatus.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterStatus_offered_incompatible_qos_get(swigCPtr);
      OfferedIncompatibleQosStatus ret = (cPtr == IntPtr.Zero) ? null : new OfferedIncompatibleQosStatus(cPtr, false);
      return ret;
    } 
  }

  public LivelinessLostStatus liveliness_lost {
    set {
      cpscsharpmodulePINVOKE.DataWriterStatus_liveliness_lost_set(swigCPtr, LivelinessLostStatus.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterStatus_liveliness_lost_get(swigCPtr);
      LivelinessLostStatus ret = (cPtr == IntPtr.Zero) ? null : new LivelinessLostStatus(cPtr, false);
      return ret;
    } 
  }

  public PublicationMatchedStatus publication_matched {
    set {
      cpscsharpmodulePINVOKE.DataWriterStatus_publication_matched_set(swigCPtr, PublicationMatchedStatus.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterStatus_publication_matched_get(swigCPtr);
      PublicationMatchedStatus ret = (cPtr == IntPtr.Zero) ? null : new PublicationMatchedStatus(cPtr, false);
      return ret;
    } 
  }

  public DataWriterStatus() : this(cpscsharpmodulePINVOKE.new_DataWriterStatus(), true) {
  }

}
