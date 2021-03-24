// 2020.03.31 - Victor Dods

#pragma once

// #include <cstdint>
#include <lvd/util.hpp>
#include <QStyledItemDelegate>

class SEPTItemDelegate : public QStyledItemDelegate {
public:

    SEPTItemDelegate (QObject *parent = nullptr);

    virtual QString displayText (const QVariant &value, const QLocale &locale) const override;
    virtual QWidget *createEditor (QWidget *parent, QStyleOptionViewItem const &option, QModelIndex const &index) const override;

    char double_format_char () const { return m_double_format_char; }
    int double_precision () const { return m_double_precision; }
    lvd::Radix signed_integer_radix () const { return m_signed_integer_radix; }

    void set_double_format_char (char double_format_char) { m_double_format_char = double_format_char; }
    void set_double_precision (int double_precision) { m_double_precision = double_precision; }
    void set_signed_integer_radix (lvd::Radix signed_integer_radix) { m_signed_integer_radix = signed_integer_radix; }
    void cycle_signed_integer_radix () { m_signed_integer_radix = lvd::cycled_radix(m_signed_integer_radix); }

private:

    // The various parameters for each view type.

    char m_double_format_char;
    int m_double_precision;

    lvd::Radix m_signed_integer_radix;
};
