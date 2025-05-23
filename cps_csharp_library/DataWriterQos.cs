/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class DataWriterQos : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal DataWriterQos(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(DataWriterQos obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~DataWriterQos() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cpscsharpmodulePINVOKE.delete_DataWriterQos(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public DurabilityQosPolicy durability {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_durability_set(swigCPtr, DurabilityQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_durability_get(swigCPtr);
      DurabilityQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new DurabilityQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public DurabilityServiceQosPolicy durability_service {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_durability_service_set(swigCPtr, DurabilityServiceQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_durability_service_get(swigCPtr);
      DurabilityServiceQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new DurabilityServiceQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public DeadlineQosPolicy deadline {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_deadline_set(swigCPtr, DeadlineQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_deadline_get(swigCPtr);
      DeadlineQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new DeadlineQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public LatencyBudgetQosPolicy latency_budget {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_latency_budget_set(swigCPtr, LatencyBudgetQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_latency_budget_get(swigCPtr);
      LatencyBudgetQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new LatencyBudgetQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public LivelinessQosPolicy liveliness {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_liveliness_set(swigCPtr, LivelinessQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_liveliness_get(swigCPtr);
      LivelinessQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new LivelinessQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public ReliabilityQosPolicy reliability {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_reliability_set(swigCPtr, ReliabilityQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_reliability_get(swigCPtr);
      ReliabilityQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new ReliabilityQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public DestinationOrderQosPolicy destination_order {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_destination_order_set(swigCPtr, DestinationOrderQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_destination_order_get(swigCPtr);
      DestinationOrderQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new DestinationOrderQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public HistoryQosPolicy history {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_history_set(swigCPtr, HistoryQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_history_get(swigCPtr);
      HistoryQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new HistoryQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public ResourceLimitsQosPolicy resource_limits {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_resource_limits_set(swigCPtr, ResourceLimitsQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_resource_limits_get(swigCPtr);
      ResourceLimitsQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new ResourceLimitsQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public TransportPriorityQosPolicy transport_priority {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_transport_priority_set(swigCPtr, TransportPriorityQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_transport_priority_get(swigCPtr);
      TransportPriorityQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new TransportPriorityQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public LifespanQosPolicy lifespan {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_lifespan_set(swigCPtr, LifespanQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_lifespan_get(swigCPtr);
      LifespanQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new LifespanQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public UserDataQosPolicy user_data {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_user_data_set(swigCPtr, UserDataQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_user_data_get(swigCPtr);
      UserDataQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new UserDataQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public OwnershipQosPolicy ownership {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_ownership_set(swigCPtr, OwnershipQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_ownership_get(swigCPtr);
      OwnershipQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new OwnershipQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public OwnershipStrengthQosPolicy ownership_strength {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_ownership_strength_set(swigCPtr, OwnershipStrengthQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_ownership_strength_get(swigCPtr);
      OwnershipStrengthQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new OwnershipStrengthQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public WriterDataLifecycleQosPolicy writer_data_lifecycle {
    set {
      cpscsharpmodulePINVOKE.DataWriterQos_writer_data_lifecycle_set(swigCPtr, WriterDataLifecycleQosPolicy.getCPtr(value));
    } 
    get {
      IntPtr cPtr = cpscsharpmodulePINVOKE.DataWriterQos_writer_data_lifecycle_get(swigCPtr);
      WriterDataLifecycleQosPolicy ret = (cPtr == IntPtr.Zero) ? null : new WriterDataLifecycleQosPolicy(cPtr, false);
      return ret;
    } 
  }

  public DataWriterQos() : this(cpscsharpmodulePINVOKE.new_DataWriterQos(), true) {
  }

}
