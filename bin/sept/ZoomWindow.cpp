// 2020.03.16 - Victor Dods

#include "MainWindow.hpp"

#include <iostream>
#include <QtWidgets>

bool WheelFilter::eventFilter (QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        auto wheel_event = static_cast<QWheelEvent*>(event);
        if (wheel_event->modifiers() == Qt::ControlModifier || wheel_event->modifiers() == Qt::ShiftModifier) {
            // Returning true filters the event.  The idea is to filter it on the QGraphicsView's
            // viewport and then let the main window handle the event.
            return true;
        }
    }
    return false; // Returning false does not filter the event.
}

MainWindow::MainWindow () {
    create_actions();
    create_status_bar();
    read_settings();

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest, this, &MainWindow::commit_data);
#endif

    setUnifiedTitleAndToolBarOnMac(true);

    // Create and populate the scene.
    m_scene = new QGraphicsScene(this);
    {
        auto grid_layout = new QGridLayout();
        {
            auto label = new QLabel("HIPPO");
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            grid_layout->addWidget(label, 0, 0);
        }
        grid_layout->addWidget(new QPushButton("THINGY"), 0, 1);
        grid_layout->addWidget(new QTextEdit("OSTRICH"), 1, 0);

        {
            auto subscene = new QGraphicsScene();
            {
                auto label = new QLabel("TINY HIPPO\nTINY OSTRICH\nTINY DONKEY");
                label->setTextInteractionFlags(Qt::TextSelectableByMouse);
                subscene->addWidget(label);
            }
            auto subview = new QGraphicsView();
            subview->scale(0.5, 0.5);
            subview->setScene(subscene);
            grid_layout->addWidget(subview);
        }

        auto w = new QWidget();
        w->setLayout(grid_layout);

        m_scene->addWidget(w);
    }
//     {
// //         QWidget *w = new QLabel("HIPPO");
// //         QWidget *w = new QPushButton("THINGY");
//         QWidget *w = new QTextEdit("OSTRICH");
//         m_scene->addWidget(w);
//     }

    m_view = new QGraphicsView(this);
    // NOTE: Rendering of QTextEdit and QPushButton happen incorrectly if the default
    // ViewportUpdateMode (which is QGraphicsView::MinimalViewportUpdate) is used.
    // Same with SmartViewportUpdate and BoundingRectViewportUpdate.  The NoViewportUpdate
    // doesn't work because it doesn't update automatically (though perhaps it could work
    // if the widgets were manually triggered to re-render).
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // This works, though there are drop shadows which look weird.
    m_view->setScene(m_scene);

    // Set the drag mode to hand.  Note that the text selection and text entry of QLabel and
    // QTextEdit interfere with this, so it's not necessarily easy to do this.
//     m_view->setDragMode(QGraphicsView::ScrollHandDrag);

    // Install an event filter on the QGraphicsView to override certain behaviors.
    auto wheel_filter = new WheelFilter();
    m_view->viewport()->installEventFilter(wheel_filter);

    this->setCentralWidget(m_view);
}

void MainWindow::closeEvent (QCloseEvent *event) {
    // Do stuff, then call event->accept() or event->ignore().  There are probably other
    // ways you could respond to the event (see QCloseEvent).
    event->accept();
}

// void MainWindow::keyPressEvent (QKeyEvent *event) {
//     switch (event->key()) {
//         case Qt::Key_Minus:
//             m_view->scale(1.0/1.1, 1.0/1.1);
//             break;
//
//         case Qt::Key_Plus:
//             m_view->scale(1.1, 1.1);
//             break;
//
//         case Qt::Key_BracketLeft:
//             m_view->rotate(-15.0);
//             break;
//
//         case Qt::Key_BracketRight:
//             m_view->rotate(15.0);
//             break;
//     }
// }

void MainWindow::wheelEvent (QWheelEvent *event) {
    double constexpr ANGLE_DELTA = 15.0;
    double constexpr SCALE_FACTOR = 1.1;

    // If only Ctrl button is pressed, zoom.
    // If only Shift button is pressed, rotate.

    // NOTE: If the modifier is Qt::AltModifier, then the x and y coordinates of angleDelta
    // are switched, ostensibly to facilitate horizontal scrolling.
    switch (event->modifiers()) {
        case Qt::ControlModifier: {
            bool wheel_went_up = event->angleDelta().y() >= 0;
            if (wheel_went_up)
                m_view->scale(SCALE_FACTOR, SCALE_FACTOR);
            else
                m_view->scale(1.0/SCALE_FACTOR, 1.0/SCALE_FACTOR);
            event->accept();
            break;
        }

        case Qt::ShiftModifier: {
            bool wheel_went_up = event->angleDelta().y() >= 0;
            m_view->rotate((wheel_went_up ? -1.0 : 1.0) * ANGLE_DELTA);
            event->accept();
            break;
        }
    }
}

void MainWindow::about () {
    QMessageBox::about(
        this,
        tr("About SEPT Viewer"),
        tr("Created 2020.03.16 by Victor Dods")
    );
}

void MainWindow::create_actions() {
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
//     QToolBar *file_tool_bar = addToolBar(tr("File"));

    file_menu->addSeparator();

    QAction *exit_action = file_menu->addAction(tr("E&xit"), this, &QWidget::close);
    exit_action->setShortcuts(QKeySequence::Quit);
    exit_action->setStatusTip(tr("Exit the application"));

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
