#include "webqqbridgecallback.h"


void ObjectInstance::callSignal(CallbackObject callback)
{
    emit signal_from_instance(callback);
}

ObjectInstance::ObjectInstance()
{
}

ObjectInstance *ObjectInstance::m_instance = 0;

ObjectInstance* ObjectInstance::instance()
{
  if (!m_instance) {
    if (!m_instance)
    m_instance = new ObjectInstance;
  }
 
    return m_instance;
}


#include "webqqbridgecallback.moc"