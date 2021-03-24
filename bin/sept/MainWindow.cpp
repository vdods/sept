// 2020.03.28 - Victor Dods

#include "MainWindow.hpp"

#include "SEPTItem.hpp"
#include "SEPTModel.hpp"
#include "SEPTTreeView.hpp"
#include <functional>
#include <iostream>
#include <QtWidgets>
#include "RowIndexSet.hpp"
#include "sept/ArrayTerm.hpp"

MainWindow::MainWindow ()
    :   m_mdi_area(nullptr)
    ,   m_insert_value_before_action(nullptr)
    ,   m_insert_value_after_action(nullptr)
    ,   m_delete_selected_values_action(nullptr)
{
    // Create the MDI area, configure it, connect it, and set it as the central widget in this MainWindow.
    m_mdi_area = new QMdiArea();
    m_mdi_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdi_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdi_area->setViewMode(QMdiArea::TabbedView);
    setCentralWidget(m_mdi_area);
    connect(m_mdi_area, &QMdiArea::subWindowActivated, this, &MainWindow::subwindow_activated);

    create_actions();
    create_status_bar();
    read_settings();

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest, this, &MainWindow::commit_data);
#endif

    setUnifiedTitleAndToolBarOnMac(true);

    // TODO: Figure out if the existing item delegate has to be deleted (docs say
    // QAbstractItemView doesn't handle ownership).
    m_item_delegate = std::make_unique<SEPTItemDelegate>();

    update_stuff();
    resize_columns_to_contents();
}

void MainWindow::closeEvent (QCloseEvent *event) {
    // Do stuff, then call event->accept() or event->ignore().  There are probably other
    // ways you could respond to the event (see QCloseEvent).
    event->accept();
}

void MainWindow::set_double_precision (int double_precision) {
    m_item_delegate->set_double_precision(double_precision);
    update_all_view_viewports();
    // Update the status bar with the change.
    statusBar()->showMessage(QString::fromStdString(LVD_FMT("Double precision is now set to " << m_item_delegate->double_precision())));
}

void MainWindow::set_double_precision_via_dialog () {

}

void MainWindow::set_signed_integer_radix (lvd::Radix radix) {
    m_item_delegate->set_signed_integer_radix(radix);
    update_all_view_viewports();
    // Update the status bar with the change.
    statusBar()->showMessage(QString::fromStdString(LVD_FMT("Signed int radix is now set to " << m_item_delegate->signed_integer_radix())));
}

void MainWindow::cycle_signed_integer_radix () {
    set_signed_integer_radix(lvd::cycled_radix(m_item_delegate->signed_integer_radix()));
}

void MainWindow::set_signed_integer_radix_to_bin () {
    set_signed_integer_radix(lvd::Radix::BIN);
}

void MainWindow::set_signed_integer_radix_to_oct () {
    set_signed_integer_radix(lvd::Radix::OCT);
}

void MainWindow::set_signed_integer_radix_to_dec () {
    set_signed_integer_radix(lvd::Radix::DEC);
}

void MainWindow::set_signed_integer_radix_to_hex () {
    set_signed_integer_radix(lvd::Radix::HEX);
}

void MainWindow::create_new_document () {
    create_new_document_with_content(
        sept::make_array(
            1,
            2,
            3,
            true,
            false,
            4.567000333,
            89.01123555,
            1.0020304e10,
            80.75,
            sept::make_array(10, 100, -20, -30),
            sept::make_array(true, true, false, false, true, false),
            sept::make_array(-3, -2, -1, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536)
//             sept::make_array(int8_t(120), int16_t(4000), int32_t(1000000000), int64_t(999000000000))
        )
    );
}

void MainWindow::subwindow_activated () {
    update_stuff();
}

void MainWindow::delete_selected_values () {
    SEPTTreeView *active_view = active_mdi_child();
    assert(active_view != nullptr && "this should be impossible due to disabled actions");
    if (active_view == nullptr)
        return;

    QModelIndexList indices = active_view->selectionModel()->selectedIndexes();
    QAbstractItemModel *model = active_view->model();

    DescendingRowIndexSet row_index_set;
    for (int i = 0; i < indices.size(); ++i) {
        QModelIndex &index = indices[i];
        row_index_set.emplace(index);
    }

    // TODO: Make this more efficient and remove everything at once, or at least contiguous segments all at once.
    for (auto const &row_index : row_index_set) {
        auto const &index = row_index.m_index;
        model->removeRow(index.row(), index.parent());
    }

    update_stuff();
    resize_columns_to_contents();
}

void MainWindow::extract_elements () {
    SEPTTreeView *active_view = active_mdi_child();
    assert(active_view != nullptr && "this should be impossible due to disabled actions");
    if (active_view == nullptr)
        return;

    // TEMP HACK: For now, only support extracting elements at top level.  In order to have
    // the more general version, a hierarchy addressing scheme is needed.
    create_new_document_with_content(active_view->extract_elements(), active_view);
}

void MainWindow::integrate_data () {
    SEPTTreeView *active_view = active_mdi_child();
    assert(active_view != nullptr && "this should be impossible due to disabled actions");
    if (active_view == nullptr || active_view->source_view() == nullptr)
        return;

    active_view->source_view()->integrate_data(active_view->model()->root_data());
    // TODO: Probably close this active_view

    update_stuff();
    resize_columns_to_contents();
}

void MainWindow::integrate_data_and_close () {
    SEPTTreeView *active_view = active_mdi_child();
    assert(active_view != nullptr && "this should be impossible due to disabled actions");
    if (active_view == nullptr || active_view->source_view() == nullptr)
        return;

    active_view->source_view()->integrate_data(active_view->model()->root_data());
    m_mdi_area->closeActiveSubWindow();

    // TODO: Figure out if stuff has to be updated.
//     update_stuff();
//     resize_columns_to_contents();
}

void MainWindow::model_data_changed (QModelIndex const &top_left, QModelIndex const &bottom_right, QVector<int> const &roles) {
    resize_columns_to_contents();
}

void MainWindow::view_collapsed (QModelIndex const &index) {
    resize_columns_to_contents();
}

void MainWindow::view_expanded (QModelIndex const &index) {
    resize_columns_to_contents();
}

void MainWindow::about () {
    QMessageBox::about(
        this,
        tr("About SEPT Viewer"),
        tr("Created 2020.03.16 by Victor Dods")
    );
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commit_data(QSessionManager &manager) {
    if (manager.allowsInteraction()) {
        // Do stuff, like maybe bring up "are you sure you want to exit?" dialog.  If that returns
        // with "cancel", then call manager.cancel().
    } else {
        // Do involuntary backup of state.  Could be to an "emergency backup state".
    }
}
#endif

SEPTTreeView *MainWindow::active_mdi_child () const
{
    QMdiSubWindow *active_subwindow = m_mdi_area->activeSubWindow();
    return active_subwindow != nullptr ? qobject_cast<SEPTTreeView *>(active_subwindow->widget()) : nullptr;
}

SEPTTreeView *MainWindow::create_mdi_child ()
{
    auto *view = new SEPTTreeView();
    m_mdi_area->addSubWindow(view);
    return view;
}

void MainWindow::create_new_document_with_content (sept::Data &&content, SEPTTreeView *source_view) {
    SEPTTreeView *view = create_mdi_child();
    view->set_source_view(source_view);

    // Create the model -- arbitrary for now.  Later should load from a file.
    SEPTModel *model = new SEPTModel(std::move(content));
    // Create the view, attach the model, and set the view as the central widget.
    view->setModel(model);

    view->setItemDelegate(m_item_delegate.get());

    // TODO: Not sure if this is right, and what's missing (e.g. document switched)
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::update_stuff);
    connect(view->model(), &QAbstractItemModel::dataChanged, this, &MainWindow::model_data_changed);
    connect(view, &QTreeView::collapsed, this, &MainWindow::view_collapsed);
    connect(view, &QTreeView::expanded, this, &MainWindow::view_expanded);

    // TODO: Probably other stuff needs to happen.

    view->show();
    view->resize_columns_to_contents();
}

void MainWindow::insert_value (InsertActionKind k) {
    SEPTTreeView *active_view = active_mdi_child();
    assert(active_view != nullptr && "this should be impossible due to disabled actions");
    if (active_view == nullptr)
        return;

    QModelIndex index = active_view->selectionModel()->currentIndex();
    bool has_current_selection = index.isValid();

    if (!has_current_selection)
        return;

    int offset = k == InsertActionKind::BEFORE ? 0 : 1;
    if (!active_view->model()->insertRow(index.row()+offset, index.parent()))
        return;

    update_stuff();
    resize_columns_to_contents();
}


void MainWindow::create_actions() {
    assert(m_mdi_area != nullptr && "m_mdi_area must be initialized first");

    //
    // File menu
    //

    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
//     QToolBar *file_tool_bar = addToolBar(tr("File"));

    QAction *new_action = file_menu->addAction(tr("&New"), this, &MainWindow::create_new_document, QKeySequence::New);
    new_action->setStatusTip(tr("Create a new document"));

    QAction *close_action = file_menu->addAction(tr("&Close"), m_mdi_area, &QMdiArea::closeActiveSubWindow);
    // TODO: Figure out why Ctrl+W works without specifying it explicitly, and why specifying it explicitly
    // causes there to be an "ambiguous keyboard shortcut" dialog popup.
//     QAction *close_action = file_menu->addAction(tr("&Close"), m_mdi_area, &QMdiArea::closeActiveSubWindow, QKeySequence::Close);
    close_action->setStatusTip(tr("Close the current document"));

    file_menu->addSeparator();

    QAction *exit_action = file_menu->addAction(tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);
    exit_action->setStatusTip(tr("Exit the application"));

    //
    // Edit menu
    //

    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
//     QToolBar *edit_tool_bar = addToolBar(tr("Edit"));

    m_insert_value_before_action = edit_menu->addAction(tr("Insert Value &Before"), this, &MainWindow::insert_value_before, Qt::Key_Insert);
    m_insert_value_before_action->setStatusTip(tr("Insert a value before the value at the current position"));

    m_insert_value_after_action = edit_menu->addAction(tr("Insert Value &After"), this, &MainWindow::insert_value_after, Qt::SHIFT + Qt::Key_Insert);
    m_insert_value_after_action->setStatusTip(tr("Insert a value after the value at the current position"));

    m_delete_selected_values_action = edit_menu->addAction(tr("&Delete Selected Values"), this, &MainWindow::delete_selected_values, Qt::SHIFT + Qt::Key_Delete);

    //
    // Project menu
    //

    QMenu *project_menu = menuBar()->addMenu(tr("&Project"));
//     QToolBar *project_tool_bar = addToolBar(tr("Project"));

    m_extract_elements_action = project_menu->addAction(tr("&Extract Element(s)"), this, &MainWindow::extract_elements, Qt::CTRL + Qt::Key_E);

    //
    // Integrate menu
    //

    QMenu *integrate_menu = menuBar()->addMenu(tr("&Integrate"));
//     QToolBar *integrate_tool_bar = addToolBar(tr("Integrate"));

    m_integrate_data_action = integrate_menu->addAction(tr("&Integrate Data"), this, &MainWindow::integrate_data, Qt::CTRL + Qt::Key_I);
    m_integrate_data_action = integrate_menu->addAction(tr("Integrate Data and &Close"), this, &MainWindow::integrate_data_and_close, Qt::CTRL + Qt::SHIFT + Qt::Key_I);

    //
    // View menu
    //

    QMenu *view_menu = menuBar()->addMenu(tr("&View"));
//     QToolBar *view_tool_bar = addToolBar(tr("View"));

//     view_menu->addSeparator();

    QAction *cycle_signed_integer_radix_action = view_menu->addAction(tr("&Cycle Signed Int Radix"), this, &MainWindow::cycle_signed_integer_radix, QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C));
    cycle_signed_integer_radix_action->setStatusTip(tr("Cycle Signed Int Radix"));

    QAction *set_signed_integer_radix_to_bin_action = view_menu->addAction(tr("Set Signed Int Radix To &Binary"), this, &MainWindow::set_signed_integer_radix_to_bin, QKeySequence(Qt::CTRL + Qt::Key_B));
    set_signed_integer_radix_to_bin_action->setStatusTip(tr("Set Signed Int Radix To Binary"));

    QAction *set_signed_integer_radix_to_oct_action = view_menu->addAction(tr("Set Signed Int Radix To &Octal"), this, &MainWindow::set_signed_integer_radix_to_oct, QKeySequence(Qt::CTRL + Qt::Key_O));
    set_signed_integer_radix_to_oct_action->setStatusTip(tr("Set Signed Int Radix To Octal"));

    QAction *set_signed_integer_radix_to_dec_action = view_menu->addAction(tr("Set Signed Int Radix To &Decimal"), this, &MainWindow::set_signed_integer_radix_to_dec, QKeySequence(Qt::CTRL + Qt::Key_D));
    set_signed_integer_radix_to_dec_action->setStatusTip(tr("Set Signed Int Radix To Decimal"));

    QAction *set_signed_integer_radix_to_hex_action = view_menu->addAction(tr("Set Signed Int Radix To &Hexadecimal"), this, &MainWindow::set_signed_integer_radix_to_hex, QKeySequence(Qt::CTRL + Qt::Key_H));
    set_signed_integer_radix_to_hex_action->setStatusTip(tr("Set Signed Int Radix To Hexadecimal"));

    //
    // Help menu
    //

    QMenu *help_menu = menuBar()->addMenu(tr("&Help"));
    QAction *about_action = help_menu->addAction(tr("&About"), this, &MainWindow::about);
    about_action->setStatusTip(tr("Show the application's About box"));

    QAction *about_qt_action = help_menu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    about_qt_action->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::create_status_bar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::read_settings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width(), availableGeometry.height());
        move(0, 0);
//         resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
//         move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::write_settings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::update_all_view_viewports () {
    // Now that the item delegate has changed, we must ensure that the view reflects that change.
    // Doing update() on the view itself didn't work, you have to update the viewport directly.
    for (auto *mdi_subwindow : m_mdi_area->subWindowList()) {
        auto *view = qobject_cast<SEPTTreeView *>(mdi_subwindow->widget());
        view->viewport()->update();
    }
}

void MainWindow::update_stuff () {
    SEPTTreeView *active_view = active_mdi_child();
    if (active_view == nullptr) {
        m_insert_value_before_action->setEnabled(false);
        m_insert_value_after_action->setEnabled(false);
        m_delete_selected_values_action->setEnabled(false);
        return;
    }

    bool has_current_selection = active_view->selectionModel()->currentIndex().isValid();
    m_insert_value_before_action->setEnabled(has_current_selection);
    m_insert_value_after_action->setEnabled(has_current_selection);
    m_delete_selected_values_action->setEnabled(has_current_selection);

    if (has_current_selection) {
        active_view->closePersistentEditor(active_view->selectionModel()->currentIndex());

        int row = active_view->selectionModel()->currentIndex().row();
        int column = active_view->selectionModel()->currentIndex().column();
        if (active_view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}

void MainWindow::resize_columns_to_contents () {
    SEPTTreeView *view = active_mdi_child();
    if (view == nullptr)
        return;

    view->resize_columns_to_contents();
}
