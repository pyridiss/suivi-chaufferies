#include <QComboBox>
#include <QModelIndex>
#include <QApplication>
#include <QString>

#include "comboboxdelegate.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox* editor = new QComboBox(parent);
    for(int i = 0; i < mItems.size(); ++i)
    {
        editor->addItem(mItems[i]);
    }
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toUInt();
    comboBox->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem myOption = option;
    QString text = mItems[index.model()->data(index, Qt::EditRole).toUInt()];

    myOption.text = text;
    myOption.displayAlignment = Qt::AlignCenter;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}

void ComboBoxDelegate::setItems(QVector<QString> items)
{
    mItems = items;
}
