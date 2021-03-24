// 2020.03.31 - Victor Dods

#include "SEPTItemDelegate.hpp"

#include "SEPTItem.hpp"
#include <cassert>
#include <QDoubleSpinBox>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <type_traits>


SEPTItemDelegate::SEPTItemDelegate (QObject *parent)
    :   QStyledItemDelegate(parent)
    ,   m_double_format_char('g')
    ,   m_double_precision(17) // This is full precision.
    ,   m_signed_integer_radix(lvd::Radix::BIN)
{ }

QString SEPTItemDelegate::displayText (const QVariant &value, const QLocale &locale) const {
    switch (value.type()) {
//         case QVariant::Bool: return QString(value.toBool() ? "True" : "False");
        case QVariant::Double: return locale.toString(value.toDouble(), m_double_format_char, m_double_precision);
        // Ideally this lvd::int64_to_string would incorporate locale information (e.g. commas, sign chars)
        case QVariant::Int: return QString::fromStdString(LVD_FMT(lvd::int64_to_string(value.toInt(), m_signed_integer_radix, lvd::UseRadixPrefix::NO, lvd::SignOption::ONLY_PRINT_NEGATIVE_SIGN) << ' ' << m_signed_integer_radix));
//         case QVariant::Int: return QString::fromStdString(LVD_FMT(lvd::int64_to_string(value.toInt(), m_signed_integer_radix, lvd::UseRadixPrefix::NO, lvd::SignOption::ONLY_PRINT_NEGATIVE_SIGN) << " (b" << uint32_t(m_signed_integer_radix) << ')'));

        default: return QStyledItemDelegate::displayText(value, locale);
    }
}

QWidget *SEPTItemDelegate::createEditor (QWidget *parent, QStyleOptionViewItem const &option, QModelIndex const &index) const {
    QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);
    if (auto double_spin_box = qobject_cast<QDoubleSpinBox*>(editor)) {
        double_spin_box->setDecimals(m_double_precision);
        double_spin_box->setMinimum(-__DBL_MAX__);
        double_spin_box->setMaximum(__DBL_MAX__);
        double_spin_box->setAlignment(Qt::AlignRight);
    } else if (auto spin_box = qobject_cast<QSpinBox*>(editor)) { // Ints are handled by the baseclass QSpinBox.
        spin_box->setDisplayIntegerBase(int(m_signed_integer_radix));
        spin_box->setSuffix(QString::fromStdString(LVD_FMT(' ' << m_signed_integer_radix)));
//         spin_box->setSuffix(QString::fromStdString(LVD_FMT(" (b" << uint32_t(m_signed_integer_radix) << ')')));
        spin_box->setAlignment(Qt::AlignRight);
    }
    return editor;
}
