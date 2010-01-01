/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2009 Bernd Buschinski <b.buschinski@web.de>
*/

#include "whiteboardcolorchooser.h"

#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>

#include <KColorDialog>
#include <kdebug.h>

namespace Konversation
{
    namespace DCC
    {
        WhiteBoardColorChooser::WhiteBoardColorChooser(QWidget* parent)
            : QFrame(parent),
              m_foregroundColor(Qt::black),
              m_backgroundColor(Qt::white),
              m_swapPixmap(16,16)
        {
            setFrameShadow(QFrame::Sunken);
            setMinimumSize(40,40);
            drawSwapPixmap();
        }

        WhiteBoardColorChooser::~WhiteBoardColorChooser()
        {
        }

        QColor WhiteBoardColorChooser::color(const ColorLayer& layer) const
        {
            switch (layer)
            {
            case BackgroundColor:
                return m_backgroundColor;
            case ForegroundColor:
                return m_foregroundColor;
            default:
                Q_ASSERT(false);
                return QColor(Qt::transparent);
            }
        }

        QColor WhiteBoardColorChooser::foregroundColor() const
        {
            return color(ForegroundColor);
        }

        QColor WhiteBoardColorChooser::backgroundColor() const
        {
            return color(BackgroundColor);
        }

        void WhiteBoardColorChooser::setColor(ColorLayer layer, const QColor& color)
        {
            switch (layer)
            {
            case ForegroundColor:
                setForegroundColor(color);
                emit foregroundColorChanged(color);
                break;
            case BackgroundColor:
                setBackgroundColor(color);
                emit backgroundColorChanged(color);
                break;
            default:
                Q_ASSERT(false);
                return;
            }
        }

        void WhiteBoardColorChooser::setForegroundColor (const QColor& color)
        {
            m_foregroundColor = color;
        }

        void WhiteBoardColorChooser::setBackgroundColor (const QColor& color)
        {
            m_backgroundColor = color;
        }

        void WhiteBoardColorChooser::mouseReleaseEvent(QMouseEvent *e)
        {
            kDebug() << "epos:"<< e->pos();
            kDebug() << "foregroundrect" << foregroundRect();
            kDebug() << "backgroundrect" << backgroundRect();
            kDebug() << "swap" << swapPixmapRect();

            ColorLayer whichColor = None;

            if (foregroundRect().contains(e->pos()))
            {
                whichColor = ForegroundColor;
                kDebug() << "> in foreground";
            }
            else if (backgroundRect().contains(e->pos()))
            {
                whichColor = BackgroundColor;
                kDebug() << "> in background";
            }
            else if (swapPixmapRect().contains(e->pos()))
            {
                kDebug() << "> in swap";
                QColor oldFore = m_foregroundColor;
                m_foregroundColor = m_backgroundColor;
                m_backgroundColor = oldFore;
                emit colorsSwapped(m_foregroundColor, m_backgroundColor);
                update();
                return;
            }

            if (whichColor == ForegroundColor || whichColor == BackgroundColor)
            {
                QColor col = color(whichColor);
                if (KColorDialog::getColor(col, this) == KColorDialog::Accepted)
                {
                    setColor(whichColor, col);
                    update();
                }
            }
        }

        void WhiteBoardColorChooser::paintEvent(QPaintEvent *e)
        {
            QFrame::paintEvent(e);

            QPainter tPaint;
            tPaint.begin(this);

            tPaint.drawPixmap(swapPixmapRect().topLeft(), m_swapPixmap);
            QRect bgRect = backgroundRect();
            QRect bgRectInside = QRect(bgRect.x () + 2, bgRect.y () + 2,
                                       bgRect.width () - 4, bgRect.height () - 4);
            tPaint.fillRect (bgRectInside, m_backgroundColor);
            qDrawShadePanel (&tPaint, bgRect, palette(),
                             false/*not sunken*/, 2/*lineWidth*/,
                             0/*never fill*/);

            QRect fgRect = foregroundRect();
            QRect fgRectInside = QRect(fgRect.x () + 2, fgRect.y () + 2,
                                       fgRect.width () - 4, fgRect.height () - 4);
            tPaint.fillRect(fgRectInside, m_foregroundColor);
            qDrawShadePanel (&tPaint, fgRect, palette (),
                             false/*not sunken*/, 2/*lineWidth*/,
                             0/*never fill*/);

            tPaint.end();
        }

        void WhiteBoardColorChooser::resizeEvent(QResizeEvent *e)
        {
            Q_UNUSED(e);
            const int minWidthHeight = qMin(width(),height());
            const int swapImageSize = minWidthHeight/3;
            m_swapPixmap = QPixmap(swapImageSize, swapImageSize);
            drawSwapPixmap();
        }

        QRect WhiteBoardColorChooser::swapPixmapRect() const
        {
            return QRect(contentsRect().width() - m_swapPixmap.width(),
                         0,
                         m_swapPixmap.width(),
                         m_swapPixmap.height());
        }

        QRect WhiteBoardColorChooser::foregroundBackgroundRect() const
        {
            QRect cr(contentsRect());
            return QRect(cr.width() / 8,
                         cr.height() / 8,
                         cr.width() * 6 / 8,
                         cr.height() * 6 / 8);
        }

        QRect WhiteBoardColorChooser::foregroundRect() const
        {
            QRect fbr(foregroundBackgroundRect());
            return QRect(fbr.x(),
                         fbr.y(),
                         fbr.width() * 3 / 4,
                         fbr.height() * 3 / 4);
        }

        QRect WhiteBoardColorChooser::backgroundRect() const
        {
            QRect fbr(foregroundBackgroundRect());
            return QRect(fbr.x() + fbr.width() / 4,
                         fbr.y() + fbr.height() / 4,
                         fbr.width() * 3 / 4,
                         fbr.height() * 3 / 4);
        }

        void WhiteBoardColorChooser::drawSwapPixmap()
        {
            const int arrowHeight = m_swapPixmap.height() / 4;
            const int arrowWidth = m_swapPixmap.width() / 2;
            const int imageHeight = m_swapPixmap.height();
            const int imageWidth = m_swapPixmap.width();

            const QPointF arrowLeftPolyGon[3] = {
                 QPointF(0, arrowHeight),
                 QPointF(arrowHeight, 0),
                 QPointF(arrowHeight, arrowHeight*2)
            };
            const QPointF arrowDownPolyGon[3] = {
                 QPointF(imageWidth-arrowWidth, imageHeight-arrowHeight-1),
                 QPointF(imageWidth, imageHeight-arrowHeight-1),
                 QPointF(imageWidth-arrowHeight, imageHeight-1)
            };

            m_swapPixmap.fill(Qt::transparent);

            QPainter tPainter(&m_swapPixmap);
            tPainter.setBrush(Qt::black);
            tPainter.drawPolygon(arrowLeftPolyGon, 3);
            tPainter.drawPolygon(arrowDownPolyGon, 3);

            QPoint tCenterLine(imageWidth-arrowHeight, arrowHeight);

            tPainter.drawLine(tCenterLine,QPoint(arrowHeight, arrowHeight));
            tPainter.drawLine(tCenterLine,QPoint(imageWidth-arrowHeight, imageWidth-arrowHeight));

            tPainter.end();
        }

    }
}
