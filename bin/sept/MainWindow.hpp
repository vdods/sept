// 2020.03.28 - Victor Dods

#pragma once

#include "SEPTItemDelegate.hpp"
#include <memory>
#include <QMainWindow>
#include "sept/Data.hpp"

class SEPTTreeView;

QT_BEGIN_NAMESPACE
class QAction;
class QMdiArea;
class QMdiSubWindow;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class QTreeView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    MainWindow ();

protected:

    void closeEvent (QCloseEvent *event) override;
//     void keyPressEvent (QKeyEvent *event) override;
//     void wheelEvent (QWheelEvent *event) override;

private slots:

    void set_double_precision (int double_precision);
    void set_double_precision_via_dialog ();

    void set_signed_integer_radix (lvd::Radix radix);
    void cycle_signed_integer_radix ();
    void set_signed_integer_radix_to_bin ();
    void set_signed_integer_radix_to_oct ();
    void set_signed_integer_radix_to_dec ();
    void set_signed_integer_radix_to_hex ();

    void create_new_document ();

    void subwindow_activated ();

    void insert_value_before () { insert_value(InsertActionKind::BEFORE); }
    void insert_value_after () { insert_value(InsertActionKind::AFTER); }
    void delete_selected_values ();

    void extract_elements ();

    void integrate_data ();
    void integrate_data_and_close ();

    void model_data_changed (QModelIndex const &top_left, QModelIndex const &bottom_right, QVector<int> const &roles);
    void view_collapsed (QModelIndex const &index);
    void view_expanded (QModelIndex const &index);

    void about ();

#ifndef QT_NO_SESSIONMANAGER
    void commit_data (QSessionManager &);
#endif

private:

    SEPTTreeView *active_mdi_child () const;

    SEPTTreeView *create_mdi_child ();

    enum class InsertActionKind {
        BEFORE,
        AFTER
    };

    void create_new_document_with_content (sept::Data &&content, SEPTTreeView *source_view = nullptr);

    void insert_value (InsertActionKind k);

    void create_actions ();
    void create_status_bar ();
    void read_settings ();
    void write_settings ();
    void update_all_view_viewports ();
    void update_stuff ();
    void resize_columns_to_contents ();

    QMdiArea *m_mdi_area;

    QAction *m_insert_value_before_action;
    QAction *m_insert_value_after_action;
    QAction *m_delete_selected_values_action;

    QAction *m_extract_elements_action;
    QAction *m_integrate_data_action;

    std::unique_ptr<SEPTItemDelegate> m_item_delegate;
};
