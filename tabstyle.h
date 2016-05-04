#ifndef TABSTYLE_H
#define TABSTYLE_H

#include <QProxyStyle>

class TabStyle : public QProxyStyle
{
    Q_OBJECT

public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
    void drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const;
};

#endif // TABSTYLE_H
