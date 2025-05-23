/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package kr.re.etri.cps.jni;

public class ContentFilteredTopic2 extends Entity2 {
  private long swigCPtr;

  protected ContentFilteredTopic2(long cPtr, boolean cMemoryOwn) {
    super(cpsjavamoduleJNI.ContentFilteredTopic2_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ContentFilteredTopic2 obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        cpsjavamoduleJNI.delete_ContentFilteredTopic2(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public ContentFilteredTopic2(ContentFilteredTopic topic) {
    this(cpsjavamoduleJNI.new_ContentFilteredTopic2(ContentFilteredTopic.getCPtr(topic), topic), true);
  }

  public ContentFilteredTopic getContentFilteredTopic() {
    long cPtr = cpsjavamoduleJNI.ContentFilteredTopic2_getContentFilteredTopic(swigCPtr, this);
    return (cPtr == 0) ? null : new ContentFilteredTopic(cPtr, false);
  }

  public String get_filter_expression() {
    return cpsjavamoduleJNI.ContentFilteredTopic2_get_filter_expression(swigCPtr, this);
  }

  public int get_expression_parameters(StringSeq expression_parameters) {
    return cpsjavamoduleJNI.ContentFilteredTopic2_get_expression_parameters(swigCPtr, this, StringSeq.getCPtr(expression_parameters), expression_parameters);
  }

  public int set_expression_parameters(StringSeq expression_parameters) {
    return cpsjavamoduleJNI.ContentFilteredTopic2_set_expression_parameters(swigCPtr, this, StringSeq.getCPtr(expression_parameters), expression_parameters);
  }

  public Topic2 get_related_topic() {
    return new Topic2(cpsjavamoduleJNI.ContentFilteredTopic2_get_related_topic(swigCPtr, this), true);
  }

}
