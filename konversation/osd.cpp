/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  osd.cpp -  Provides an interface to a plain QWidget, which is independent of KDE (bypassed to X11)
  begin:     Fre Sep 26 2003
  copyright: (C) 2003 by Christian Muehlhaeuser
  email:     chris@chris.de
*/

#include "osd.h"

#include <qapplication.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qregexp.h>
#include <qtimer.h>

#include <kcursor.h>


OSDWidget::OSDWidget( const QString &appName )
    : QWidget(NULL, "osd",
              WType_TopLevel | WStyle_StaysOnTop |
              WStyle_Customize | WStyle_NoBorder |
              WStyle_Tool | WRepaintNoErase | WX11BypassWM)
      , m_appName( appName )
      , m_duration( 5000 )
      , m_textColor( 0, 0 , 0 )
      , m_bgColor( 0x00, 0x00, 0x80 )
      , m_offset( 10, 40 )
      , m_position( TopLeft )
      , m_screen( 0 )
{
    setFocusPolicy( NoFocus );
    setBackgroundMode( NoBackground );
    timer = new QTimer( this );
    timerMin = new QTimer( this );
    connect( timer, SIGNAL( timeout() ), this, SLOT( removeOSD() ) );
    connect( timerMin, SIGNAL( timeout() ), this, SLOT( minReached() ) );
}


void OSDWidget::renderOSDText( const QString &text )
{
    m_currentText = text;
    QPainter paint;
    QColor bg( 0, 0, 0 );
    QColor fg( 255, 255, 255 );

    // Get desktop dimensions
    QWidget *d = QApplication::desktop()->screen( m_screen );

    QFontMetrics *fm = new QFontMetrics( font );
    /* AlignAuto = we want to align Arabic to the right, don't we? */
    /* Sane width and height limits - don't go over the screen dimensions,
       and don't cover whole desktop neither. */
    QRect fmRect = fm->boundingRect( 0, 0, d->width()-40, d->height()-100, AlignAuto | WordBreak, text );
    fmRect.addCoords( 0, 0, fmRect.width() + 2, fmRect.height() + 2);

    resize( fmRect.size() );
    QPixmap buffer = QPixmap( fmRect.size() );

    // Draw the OnScreenMessage
    QPainter bufferPainter( &buffer );

    bufferPainter.setFont( font );

    // Draw the border around the text
    bufferPainter.setPen( black );

    bufferPainter.drawText( 3, 3, width()-1, height()-1, AlignLeft | WordBreak, text );
      
    // Draw the text
    bufferPainter.setPen( m_textColor );
    bufferPainter.drawText( 1, 1, width()-1, height()-1, AlignLeft | WordBreak, text );
    bufferPainter.end();

    // Masking for transparency
    QBitmap bm( fmRect.size() );
    paint.begin( &bm );
    paint.fillRect( 0, 0, fmRect.width(), fmRect.height(), bg );
    paint.setPen( Qt::color0 );
    paint.setFont( font );
    paint.drawText( 1, 1, width()-1, height()-1, AlignLeft | WordBreak, text );
    paint.drawText( 3, 3, width()-1, height()-1, AlignLeft | WordBreak, text );
    paint.end();

    delete fm;

    osdBuffer = buffer;

    setMask( bm );
    rePosition();
}


void OSDWidget::showOSD( const QString &text, bool preemptive )
{
    if ( isEnabled() && !text.isEmpty() )
    {
        // Strip HTML tags, expand basic HTML entities
        QString plainText = text.copy();
        plainText.replace(QRegExp("</?(?:font|a|b|i)\\b[^>]*>"), QString(""));
        plainText.replace(QString("&lt;"), QString("<"));
        plainText.replace(QString("&gt;"), QString(">"));
        plainText.replace(QString("&amp;"), QString("&"));

        if ( preemptive == false && timerMin->isActive() )
        {
            textBuffer.append( plainText ); // queue
        }
        else
        {
            if ( timer->isActive() ) timer->stop();
            renderOSDText( plainText );
            if( !isVisible() )
            {
                raise();
                QWidget::show();
            }
            else
                repaint();

            // let it disappear via a QTimer
            if( m_duration ) // if duration is 0 stay forever
            {
                timer->start( m_duration, TRUE );
                timerMin->start( 150, TRUE );
            }
        }
    }
}


void OSDWidget::setDuration(int ms)
{
    m_duration = ms;
}


void OSDWidget::setFont(QFont newfont)
{
    font = newfont;
    if( isVisible() )
    {
        renderOSDText( m_currentText );
        repaint();
    }
}


void OSDWidget::setTextColor(QColor newcolor)
{
    m_textColor = newcolor;
    if( isVisible() )
    {
        renderOSDText( m_currentText );
        repaint();
    }
}


void OSDWidget::setBackgroundColor(QColor newColor)
{
    m_bgColor = newColor;
    if( isVisible() )
    {
        renderOSDText( m_currentText );
        repaint();
    }
}


void OSDWidget::setOffset(int x, int y) // QPoint?
{
    m_offset.setX( x );
    m_offset.setY( y );
    if( isVisible() ) rePosition();
}


void OSDWidget::setPosition(Position pos)
{
    m_position = pos;
    if( isVisible() ) rePosition();
}


void OSDWidget::setScreen(uint screen)
{
    m_screen = screen;
    if( m_screen >= QApplication::desktop()->numScreens() )
        m_screen = QApplication::desktop()->numScreens() - 1;
    if( isVisible() ) rePosition();
}


//SLOT
void OSDWidget::minReached()
{
    if ( textBuffer.count() > 0 )
    {
        renderOSDText( textBuffer[0] );
        textBuffer.remove( textBuffer.at( 0 ) );
        repaint();

        if( m_duration ) // if duration is 0 stay forever
        {
            timer->start( m_duration, TRUE );
            timerMin->start( 150, TRUE );
        }
    }
}


//SLOT
void OSDWidget::removeOSD()
{
    hide();
}


void OSDWidget::paintEvent( QPaintEvent* )
{
    QPainter p( this );
    p.drawPixmap( 0, 0, osdBuffer );
    p.end();
}


void OSDWidget::mousePressEvent( QMouseEvent* )
{
    removeOSD();
}


void OSDWidget::rePosition()
{
    int newX = 0, newY = 0;
    QRect screenRect = QApplication::desktop()->screenGeometry( m_screen );
    switch( m_position )
    {
        case TopLeft:
            newX = m_offset.x();
            newY = m_offset.y();
            break;
        case TopRight:
            newX = screenRect.width() - m_offset.x() - osdBuffer.width();
            newY = m_offset.y();
            break;
        case BottomLeft:
            newX = m_offset.x();
            newY = screenRect.height() - m_offset.y() - osdBuffer.height();
            break;
        case BottomRight:
            newX = screenRect.width() - m_offset.x() - osdBuffer.width();
            newY = screenRect.height() - m_offset.y() - osdBuffer.height();
            break;
        case Center:
            newX = ( screenRect.width() - osdBuffer.width() ) / 2;
            newY = ( screenRect.height() - osdBuffer.height() ) / 2;
            break;
    }

    // correct for screen position
    newX += screenRect.x();
    newY += screenRect.y();

    // TODO: check for sanity?
    if( newX != x() || newY != y() ) move( newX, newY );
}


//////  OSDPreviewWidget below /////////////////////

OSDPreviewWidget::OSDPreviewWidget( const QString &appName ) : OSDWidget( appName )
{
    m_dragging = false;
    setDuration( 0 );
}


void OSDPreviewWidget::mousePressEvent( QMouseEvent *event )
{
    m_dragOffset = QCursor::pos() - QPoint( x(), y() );
    if( event->button() == LeftButton && !m_dragging ) {
        grabMouse( KCursor::sizeAllCursor() );
        m_dragging = true;
    }
}


void OSDPreviewWidget::mouseReleaseEvent( QMouseEvent */*event*/ )
{
    if( m_dragging )
    {
        m_dragging = false;
        releaseMouse();

        // compute current Position && offset
        QDesktopWidget *desktop = QApplication::desktop();
        int currentScreen = desktop->screenNumber( QPoint( x(), y() ) );
        Position pos = Center;
        if( currentScreen != -1 )
        {
            QRect screenRect = desktop->screenGeometry( currentScreen );
            // figure out what quadrant we are in and the offset in that quadrant
            int xoffset = 0, yoffset = 0;
            bool left = false;
            if( x() < screenRect.width() / 2 + screenRect.x() ) // left
            {
                left = true;
                xoffset = x() - screenRect.x();
            }
            else
            {
                xoffset = screenRect.x() + screenRect.width() - x() - width();
            }
            if( y() < screenRect.height() / 2 + screenRect.y() ) // top
            {
                yoffset = y() - screenRect.y();
                pos = left ? TopLeft : TopRight;
            }
            else
            {
                yoffset = screenRect.y() + screenRect.height() - y() - height();
                pos = left ? BottomLeft : BottomRight;
            }
            // set new data
            m_offset.setX( xoffset );
            m_offset.setY( yoffset );
            m_position = pos;
            m_screen = currentScreen;
            emit positionChanged( currentScreen, pos, xoffset, yoffset );
        }
    }
}


void OSDPreviewWidget::mouseMoveEvent( QMouseEvent */*event*/ )
{
    if( m_dragging && this == mouseGrabber() )
    {
        move( QCursor::pos() - m_dragOffset );
    }
}

#include "osd.moc"
