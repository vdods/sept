// 2020.03.16 - Victor Dods

#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QGraphicsScene;
class QGraphicsView;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
QT_END_NAMESPACE

class WheelFilter : public QObject {
    Q_OBJECT

protected:

    bool eventFilter (QObject *obj, QEvent *event);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow ();

protected:

    void closeEvent (QCloseEvent *event) override;
//     void keyPressEvent (QKeyEvent *event) override;
    void wheelEvent (QWheelEvent *event) override;

private slots:

    void about ();

#ifndef QT_NO_SESSIONMANAGER
    void commit_data (QSessionManager &);
#endif

private:

    void create_actions ();
    void create_status_bar ();
    void read_settings ();
    void write_settings ();

    // This is the "model" corresponding to a QGraphicsView.
    QGraphicsScene *m_scene;
    // This is the "view" corresponding to a QGraphicsScene.
    QGraphicsView *m_view;
};
