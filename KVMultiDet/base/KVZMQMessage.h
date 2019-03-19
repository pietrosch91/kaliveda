#ifndef KVZMQMESSAGE_H
#define KVZMQMESSAGE_H
#include "TMessage.h"
#include "TClass.h"
#include "zmq.hpp"
#include "KVConfig.h"

class KVZMQMessage : public TMessage {
   unique_ptr<TObject> fObject;
   Bool_t fReadNewObject;

public:
   KVZMQMessage()
      : TMessage(kMESS_OBJECT)
   {
      fReadNewObject = kFALSE;
   }
   KVZMQMessage(zmq::message_t& g)
      : TMessage(g.data(), g.size())
   {
      // stop the underlying TBuffer from deleting the buffer
      // which in our case belongs to the zmq::message_t object
      ResetBit(kIsOwner);
      fReadNewObject = kTRUE;
   }
   virtual ~KVZMQMessage() {}
   virtual void WriteObject(const TObject* obj, Bool_t cacheReuse = kTRUE)
   {
      TMessage::WriteObject(obj, cacheReuse);
      SetLength();
   }
   zmq::message_t GetMessage()
   {
      zmq::message_t m(Length());
      memcpy(m.data(), Buffer(), Length());
      return m;
   }
   TObject* GetObject()
   {
      if (fReadNewObject) {
         fObject.reset(ReadObject(GetClass()));
         fReadNewObject = kFALSE;
      }
      return fObject.get();
   }
   template <class T>
   T* GetObject()
   {
      if (GetClass()->InheritsFrom(T::Class())) {
         return (T*)(GetObject());
      }
      return nullptr;
   }
};

#endif // KVMESSAGE_H
