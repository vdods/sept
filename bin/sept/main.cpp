// 2020.03.16 - Victor Dods

#include <QApplication>

#include <lvd/OstreamDelegate.hpp>
#include "MainWindow.hpp"
#include "sept/Data.hpp"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("vdods");
    QCoreApplication::setApplicationName("Structured Expression Project Toolkit");
    QCoreApplication::setApplicationVersion("0.0.0");

    MainWindow main_window;
    main_window.show();
    return app.exec();
}
