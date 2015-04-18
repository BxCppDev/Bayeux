// Standard library:
#include <iostream>

// This project:
#include <datatools/datatools.h>
#include <datatools/datatools_config.h>
#include <bayeux/bayeux.h>

#if DATATOOLS_WITH_QT_GUI == 1
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <datatools/qt/led.h>
#endif // DATATOOLS_WITH_QT_GUI == 1

void test_qt_led(bool gui_ = false);

int main(int argc_, char * argv_[])
{
#if DATATOOLS_STANDALONE == 1
  DATATOOLS_INIT_MAIN(argc_, argv_);
#else
  BAYEUX_INIT_MAIN(argc_, argv_);
#endif // DATATOOLS_STANDALONE == 1

  bool gui = false;
  int iarg =  1;
  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if ((arg == "-g") || (arg == "--gui")) gui = true;
    iarg++;
  }

  test_qt_led(gui);

#if DATATOOLS_STANDALONE == 1
  DATATOOLS_FINI();
#else
  BAYEUX_FINI();
#endif // DATATOOLS_STANDALONE == 1
  return 0;
}

void test_qt_led(bool gui_)
{
  if (gui_) {
    int argc = 1;
    // char qtitle[100];
    // qtitle = { "test" };
    char * argv[] = { "test" };
#if DATATOOLS_WITH_QT_GUI == 1
    QApplication app(argc, argv);
    QWidget window;
    QVBoxLayout * layout = new QVBoxLayout;
    QPushButton * my_button = new QPushButton("Toggle");
    datatools::qt::led  * my_led = new datatools::qt::led(datatools::qt::led::Triangle,
                                                          datatools::qt::led::Green,
                                                          datatools::qt::led::Red);
    my_led->set_value(true);
    datatools::qt::led * my_led2 = new datatools::qt::led;
    my_led2->set_value(false);
    my_led2->set_on_color(datatools::qt::led::Orange);
    QObject::connect(my_button, SIGNAL(clicked()),my_led,SLOT(toggle_value()));
    QObject::connect(my_button, SIGNAL(clicked()),my_led2,SLOT(toggle_value()));
    layout->addWidget(my_led);
    layout->addWidget(my_led2);
    layout->addWidget(my_button);
    window.setLayout(layout);
    window.show();
    int ret = app.exec();
#else
    std::cerr << "test_qt_led::No support for Qt based GUI\n";
#endif // DATATOOLS_WITH_QT_GUI == 1
  }
  return;
}
