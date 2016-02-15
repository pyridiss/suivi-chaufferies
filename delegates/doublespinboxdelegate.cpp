#include <QDoubleSpinBox>
#include <QApplication>

#include "doublespinboxdelegate.h"

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject *parent) :
    QItemDelegate(parent)
{
    mMaximum = 1000000;
    mPrecision = 4;
}

QWidget *DoubleSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setSuffix(mSuffix);
    editor->setGroupSeparatorShown(true);
    editor->setMaximum(mMaximum);
    return editor;
}

void DoubleSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    spinBox->setValue(value);
}

void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    model->setData(index, spinBox->value(), Qt::EditRole);
}

void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void DoubleSpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem myOption = option;

    QLocale locale;
    QString text = locale.toString(index.model()->data(index, Qt::EditRole).toDouble(), 'g', mPrecision);

    myOption.text = text + mSuffix;
    myOption.displayAlignment = Qt::AlignCenter;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}

void DoubleSpinBoxDelegate::setSuffix(QString suffix)
{
    mSuffix = suffix;
}

void DoubleSpinBoxDelegate::setMaximum(int maximum)
{
    mMaximum = maximum;
}

void DoubleSpinBoxDelegate::setPrecision(int precision)
{
    mPrecision = precision;
}
