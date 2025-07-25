QT -= gui
QT += network
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
 #       fiforeader.cpp \
        main.cpp \
   # TcpServer/tcpserver.cpp \
    #TcpServer/send.cpp \
   # utils/utils.cpp
    device/candevice.cpp \
    utils/utils.cpp \
    device/cantask.cpp \
    device/tcpserver.cpp \
    dataprocess/dataprocess.cpp \
    utils/effectvaluecalculator.cpp

HEADERS += \
    fiforeader.h \
   # TcpServer/tcpserver.h \
   # TcpServer/send.h \
   # utils/utils.h \
  #  maintask.h
    device/candevice.h \
    utils/utils.h \
    global_def.h \
    device/cantask.h \
    device/tcpserver.h \
    dataprocess/dataprocess.h \
    utils/effectvaluecalculator.h

INCLUDEPATH += include

DEFINES += USE_SIM

if(contains(DEFINES,USE_SIM)){
LIBS += $$PWD/libx86/libfftw3.a
}else{
LIBS += $$PWD/libarm/libfftw3.a -lpthread $$PWD/libarm/libFifoReader.a
}
#LIBS += -L$$PWD/libx86/ -lfftw3
