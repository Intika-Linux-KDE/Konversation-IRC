/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
    dccchat.cpp  -  The class that controls dcc chats
    begin:     Sun Nov 16 2003
    copyright: (C) 2002 by Dario Abatianni
    email:     eisfuchs@tigress.com
*/

#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <qvbox.h>
#include <qhostaddress.h>
#include <qtextcodec.h>

#include <klineedit.h>
#include <konversationapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kserversocket.h>
#include <ksocketaddress.h>
#include <kstreamsocket.h>

#include "irccharsets.h"
#include "ircview.h"
#include "ircinput.h"
#include "dccchat.h"

#define DCCCHAT_BUFFER_SIZE 1024

#ifdef USE_MDI
DccChat::DccChat(QString caption,Server* newServer,const QString& myNickname,const QString& nickname,const QStringList& parameters,bool listen) :
      ChatWindow(caption)
#else
DccChat::DccChat(QWidget* parent,Server* newServer,const QString& myNickname,const QString& nickname,const QStringList& parameters,bool listen) :
      ChatWindow(parent)
#endif
{
  kdDebug() << "DccChat::DccChat() [BEGIN]" << endl;
  m_dccSocket=0;
  m_listenSocket=0;
  port=0;

  setType(ChatWindow::DccChat);
  setChannelEncodingSupported(true);

  myNick=myNickname;
  nick=nickname;

  if(!listen)
  {
    host=parameters[1];
    port=parameters[2].toInt();
  }

  // this is the main box
  QVBox* mainBox=new QVBox(this);
  mainBox->setSpacing(spacing());

  sourceLine=new KLineEdit(mainBox);
  setTextView(new IRCView(mainBox,NULL));

  dccChatInput=new IRCInput(mainBox);

  sourceLine->setReadOnly(true);
  dccChatInput->setEnabled(false);
  
  setServer(newServer);
  ChatWindow::setName("-"+nickname+"-");
  ChatWindow::setLogfileName("-"+nickname+"-");
  
  // connect the signals and slots
  connect(dccChatInput,SIGNAL (submit()),this,SLOT (dccChatTextEntered()) );
  connect(dccChatInput,SIGNAL (textPasted(const QString&)),this,SLOT (textPasted(const QString&)) );
  connect(getTextView(), SIGNAL(textPasted()), dccChatInput, SLOT(paste()));

  connect(getTextView(),SIGNAL (newText(const QString&,bool)),this,SLOT (newTextInView(const QString&,bool)) );
  connect(getTextView(),SIGNAL (autoText(const QString&)),this,SLOT (sendDccChatText(const QString&)) );

  if(listen)
    listenForPartner();
  else
    connectToPartner();
  
  kdDebug() << "DccChat::DccChat() [END]" << endl;
}

DccChat::~DccChat()
{
  kdDebug() << "DccChat::~DccChat()" << endl;
  if(m_dccSocket)
    m_dccSocket->close();
  if(m_listenSocket)
    m_listenSocket->close();
}

void DccChat::listenForPartner()
{
  kdDebug() << "DccChat::listenForPartner() [BEGIN]" << endl;
  
  // Set up server socket
  m_listenSocket = new KNetwork::KServerSocket( this );
  m_listenSocket->setFamily(KNetwork::KResolver::InetFamily);
  
  if(KonversationApplication::preferences.getDccSpecificChatPorts())  // user specifies ports
  {
    // set port
    bool found = false;  // wheter succeeded to set port
    unsigned long port = KonversationApplication::preferences.getDccChatPortsFirst();
    for( ; port <= KonversationApplication::preferences.getDccChatPortsLast() ; ++port )
    {
      kdDebug() << "DccChat::listenForPartner(): trying port " << port << endl;
      m_listenSocket->setAddress(QString::number(port));
      bool success = m_listenSocket->listen();
      if( found = ( success && m_listenSocket->error() == KNetwork::KSocketBase::NoError ) )
        break;
      m_listenSocket->close();
    }
    if(!found)
    {
      KMessageBox::sorry(0, i18n("There is no vacant port for DCC chat."));
      return;
    }
  }
  else  // user doesn't specify ports
  {
    // Let the operating system choose a port
    m_listenSocket->setAddress("0");
    if(!m_listenSocket->listen())
    {
      kdDebug() << this << "DccChat::listenForPartner(): listen() failed!" << endl;
      return;
    }
  }
  
  connect( m_listenSocket, SIGNAL(readyAccept()), this, SLOT(heardPartner()) );
  
  // Get our own port number
  const KNetwork::KSocketAddress ipAddr=m_listenSocket->localAddress();
  const struct sockaddr_in* socketAddress=(sockaddr_in*)ipAddr.address();
  port=ntohs(socketAddress->sin_port);
  kdDebug() << "DccChat::listenForPartner(): using port " << port << endl;
  
  getTextView()->append(i18n("Info"),i18n("Offering DCC Chat connection to %1 on port %2...").arg(nick).arg(port));
  sourceLine->setText(i18n("DCC chat with %1 on port %2").arg(nick).arg(port));
  
  kdDebug() << "DccChat::listenForPartner() [END]" << endl;
}

void DccChat::newTextInView(const QString& highlightColor, bool important)
{
  emit newText(this,highlightColor,important);
}

void DccChat::connectToPartner()
{
  QHostAddress ip;

  ip.setAddress(host.toUInt());
  host=ip.toString();

  getTextView()->append(i18n("Info"),i18n("Establishing DCC Chat connection to %1 (%2:%3)...").arg(nick).arg(host).arg(port));
  sourceLine->setText(i18n("DCC chat with %1 on %2:%3").arg(nick).arg(host).arg(port));

  m_dccSocket = new KNetwork::KStreamSocket(host, QString::number(port), this);

  m_dccSocket->setBlocking(false);
  m_dccSocket->setFamily(KNetwork::KResolver::InetFamily);
  m_dccSocket->enableRead(false);
  m_dccSocket->enableWrite(false);
  m_dccSocket->setTimeout(5000);

  connect( m_dccSocket, SIGNAL( hostFound() ),                        this, SLOT( lookupFinished() )           );
  connect( m_dccSocket, SIGNAL( connected( const KResolverEntry& ) ), this, SLOT( dccChatConnectionSuccess() ) );
  connect( m_dccSocket, SIGNAL( gotError( int ) ),                    this, SLOT( dccChatBroken( int ) )       );
  connect( m_dccSocket, SIGNAL( readyRead() ),                        this, SLOT( readData() )                 );
  connect( m_dccSocket, SIGNAL( closed() ),                           this, SLOT( socketClosed() )             );

  m_dccSocket->connect();

  getTextView()->append(i18n("Info"),i18n("Looking for host %1...").arg(host));
}

void DccChat::lookupFinished()
{
  getTextView()->append(i18n("Info"),i18n("Host found, connecting..."));
}

void DccChat::dccChatConnectionSuccess()
{
  getTextView()->append(i18n("Info"),i18n("Connection established."));

  m_dccSocket->enableRead(true);
  dccChatInput->setEnabled(true);
}

void DccChat::dccChatBroken(int error)
{
  getTextView()->append(i18n("Error"),i18n("Connection broken, error code %1.").arg(error));
}

void DccChat::readData()
{
  kdDebug() << k_funcinfo << " BEGIN" << endl;
  int available=0;
  int actual=0;
  char* buffer=0;
  QString line;
  QTextCodec* codec = QTextCodec::codecForName(m_encoding.isEmpty() ? IRCCharsets::encodingForLocale().ascii() : m_encoding.ascii());
  
  available = m_dccSocket->bytesAvailable();
  if( available > 0 )
  {
    buffer = new char[ available + 1 ];
    actual = m_dccSocket->readBlock( buffer, available );
    buffer[ actual ] = 0;
    line.append( codec->toUnicode( buffer ) );
    delete[] buffer;
  }
  
  if(!line.isEmpty())
  {
    QStringList lines=QStringList::split('\n',line);

    for(unsigned int index=0;index<lines.count();index++)
    {
      if(line.startsWith("\x01"))
      {
        // cut out the CTCP command
        QString ctcp=line.mid(1,line.find(1,1)-1);

        QString ctcpCommand=ctcp.section(" ",0,0);
        QString ctcpArgument=ctcp.section(" ",1);

        if(ctcpCommand.lower()=="action")
          getTextView()->appendAction(nick,ctcpArgument);
        else
          getTextView()->append(i18n("CTCP"),i18n("Received unknown CTCP-%1 request from %2").arg(ctcp).arg(nick));
      }
      else getTextView()->append(nick,lines[index]);
    } // endfor
  }
  kdDebug() << k_funcinfo << " END" << endl;
}

void DccChat::dccChatTextEntered()
{
  QString line=dccChatInput->text();
  dccChatInput->clear();
  if(line.lower()=="/clear") textView->clear();
  else
  {
    if(line.length()) sendDccChatText(line);
  }
}

void DccChat::sendDccChatText(const QString& sendLine)
{
  kdDebug() << k_funcinfo << " BEGIN" << endl;
  // create a work copy
  QString output(sendLine);
  QString cc=KonversationApplication::preferences.getCommandChar();

  if(!output.isEmpty())
  {
    QStringList lines=QStringList::split('\n',output);
    // wrap socket into a stream
    QTextStream stream(m_dccSocket);
    // init stream props
    stream.setCodec(QTextCodec::codecForName(m_encoding.isEmpty() ? IRCCharsets::encodingForLocale().ascii() : m_encoding.ascii()));
    //stream.setEncoding(QTextStream::Locale);

/*
      QString codecName=identity->getCodec();
      // convert encoded data to IRC ascii only when we don't have the same codec locally
      if(QString(QTextCodec::codecForLocale()->name()).lower()!=codecName.lower())
      {
        stream.setCodec(QTextCodec::codecForName(codecName.ascii()));
      }
*/
    for(unsigned int index=0;index<lines.count();index++)
    {
      QString line(lines[index]);

  // replace aliases and wildcards
  //  if(filter.replaceAliases(line)) line=server->parseWildcards(line,nick,getName(),QString::null,QString::null,QString::null);

  //  line=filter.parse(nick,line,getName());

      // convert /me actions
      if(line.lower().startsWith(cc+"me "))
      {
        getTextView()->appendAction(myNick,line.section(" ",1));
        line=QString("\x01%1 %2\x01").arg("ACTION").arg(line.section(" ",1));
      }
      else getTextView()->append(myNick,line);

      stream << line << endl;
    } // endfor

    // detach stream
    stream.unsetDevice();
  }
  kdDebug() << k_funcinfo << " END" << endl;
}

void DccChat::heardPartner()
{
  m_dccSocket = static_cast<KNetwork::KStreamSocket*>( m_listenSocket->accept() );
  
  if(!m_dccSocket)
  {
    this->deleteLater();
    return;
  }
  
  connect( m_dccSocket, SIGNAL( readyRead() ),     this, SLOT( readData() )           );
  connect( m_dccSocket, SIGNAL( closed() ),        this, SLOT( socketClosed() )       );
  connect( m_dccSocket, SIGNAL( gotError( int ) ), this, SLOT( dccChatBroken( int ) ) );
  
  m_listenSocket->close();
  m_listenSocket = 0;
    
  m_dccSocket->enableRead(true);
  dccChatInput->setEnabled(true);

  getTextView()->append(i18n("Info"),i18n("Connection established."));
}

void DccChat::socketClosed()
{
  getTextView()->appendServerMessage(i18n("Info"),"Connection closed.");
  dccChatInput->setEnabled(false);
  m_dccSocket = 0;
}

void DccChat::textPasted(const QString& text)
{
  sendDccChatText(text);
}

void DccChat::childAdjustFocus()
{
  dccChatInput->setFocus();
}

bool DccChat::frontView()        { return true; }
bool DccChat::searchView()       { return true; }

int DccChat::getPort()           { return port; }

QString DccChat::getTextInLine() { return dccChatInput->text(); }

void DccChat::appendInputText(const QString& s)
{
  dccChatInput->setText(dccChatInput->text() + s);
}

void DccChat::closeYourself()
{
  delete this;
}

void DccChat::setChannelEncoding(const QString& encoding)  // virtual
{
  m_encoding = encoding;
}

QString DccChat::getChannelEncoding()  // virtual
{
  return m_encoding;
}

QString DccChat::getChannelEncodingDefaultDesc()  // virtual
{
  return i18n("Default ( %1 )").arg(IRCCharsets::encodingForLocale());
}

#ifdef USE_MDI
void DccChat::closeYourself(ChatWindow*)
{
  emit chatWindowCloseRequest(this);
}
#endif

#include "dccchat.moc"
