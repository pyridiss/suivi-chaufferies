#include <QStyleOptionTab>

#include "tabstyle.h"

QSize TabStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                       const QSize &size, const QWidget *widget) const
{
    QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
    if (type == QStyle::CT_TabBarTab)
        s.transpose();
    s.setWidth(152);
    s.setHeight(128);
    return s;
}

void TabStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_TabBarTabLabel)
    {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
        {
            QStyleOptionTab opt(*tab);
            opt.shape = QTabBar::RoundedNorth;
            QProxyStyle::drawControl(element, &opt, painter, widget);
            return;
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

void TabStyle::drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    QRect newRect = rectangle;
    int newAlignment = alignment | Qt::TextWordWrap;
    newRect.setWidth(128);
    QProxyStyle::drawItemText(painter, newRect, newAlignment, palette, enabled, text, textRole);
}
