// Konversation DCOP interface class
// by Alex Zepeda, March 7, 2003

#ifndef KONV_IFACE_H
#define KONV_IFACE_H "$Id$"

#include <qobject.h>

#include <dcopobject.h>

class KonvIface : virtual public DCOPObject
{
  K_DCOP

  k_dcop:
    virtual void raw(const QString& server,const QString& command) = 0;
    virtual void say(const QString& server,const QString& target,const QString& command) = 0;
    virtual void registerEventHook(const QString& type,const QString& criteria,const QString& signal) = 0;
    virtual void unregisterEventHook(int id) = 0;
    virtual void error(const QString& string) = 0;
};

#endif
