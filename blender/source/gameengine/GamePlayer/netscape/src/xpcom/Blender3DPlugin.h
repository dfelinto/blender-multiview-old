/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM Blender3DPlugin.idl
 */

#ifndef __gen_Blender3DPlugin_h__
#define __gen_Blender3DPlugin_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    Blender3DPlugin */
#define BLENDER3DPLUGIN_IID_STR "31c3cc88-f787-47d1-b5ea-ed1b5c18b103"

#define BLENDER3DPLUGIN_IID \
  {0x31c3cc88, 0xf787, 0x47d1, \
    { 0xb5, 0xea, 0xed, 0x1b, 0x5c, 0x18, 0xb1, 0x03 }}

/** scriptable: Make this interface available for marshalling in
 * scripting languages, uid: the unique id of this interface. */
/** Peer class for a native plugin. The peer is created by the plugin,
 * so a peer will always have a destination for the calls here . */
class NS_NO_VTABLE Blender3DPlugin : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(BLENDER3DPLUGIN_IID)

  /** Load a new Blender file in this plugin from the mentioned
	 * location. 
	 * @param url (in) The url of the new .blend resource.
	 */
  /* void blenderURL (in string url); */
  NS_IMETHOD BlenderURL(const char *url) = 0;

  /** Send a message to the running gameengine, if applicable.
	 * @param to (in) The recipient of the message
	 * @param from (in) The sender of the message
	 * @param subject (in) The subject of the message
	 * @param body (in) The body of the message
	 */
  /* void SendMessage (in string to, in string from, in string subject, in string body); */
  NS_IMETHOD SendMessage(const char *to, const char *from, const char *subject, const char *body) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_BLENDER3DPLUGIN \
  NS_IMETHOD BlenderURL(const char *url); \
  NS_IMETHOD SendMessage(const char *to, const char *from, const char *subject, const char *body); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_BLENDER3DPLUGIN(_to) \
  NS_IMETHOD BlenderURL(const char *url) { return _to BlenderURL(url); } \
  NS_IMETHOD SendMessage(const char *to, const char *from, const char *subject, const char *body) { return _to SendMessage(to, from, subject, body); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_BLENDER3DPLUGIN(_to) \
  NS_IMETHOD BlenderURL(const char *url) { return !_to ? NS_ERROR_NULL_POINTER : _to->BlenderURL(url); } \
  NS_IMETHOD SendMessage(const char *to, const char *from, const char *subject, const char *body) { return !_to ? NS_ERROR_NULL_POINTER : _to->SendMessage(to, from, subject, body); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public Blender3DPlugin
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_BLENDER3DPLUGIN

  _MYCLASS_();
  virtual ~_MYCLASS_();
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, Blender3DPlugin)

_MYCLASS_::_MYCLASS_()
{
  NS_INIT_ISUPPORTS();
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* void blenderURL (in string url); */
NS_IMETHODIMP _MYCLASS_::BlenderURL(const char *url)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SendMessage (in string to, in string from, in string subject, in string body); */
NS_IMETHODIMP _MYCLASS_::SendMessage(const char *to, const char *from, const char *subject, const char *body)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

#endif /* __gen_Blender3DPlugin_h__ */

