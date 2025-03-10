QT       += core gui
#QT += opengl
#QT += quick quick3d
INCLUDEPATH += D:\eigen-3.3.9
DEFINES += WIN32_LEAN_AND_MEAN
RC_ICONS =SlopeCraft.ico
# 版本号
VERSION = 3.4.2
# 语言
# 0x0004 表示 简体中文
RC_LANG = 0x0004
# 公司名
#QMAKE_TARGET_COMPANY =  TokiNoBug
# 产品名称
#QMAKE_TARGET_PRODUCT = SlopeCraft.exe
# 详细描述
#QMAKE_TARGET_DESCRIPTION = SlopeCraft is a generator for 3 dimensional pixel painting in minecraft.
# 版权
#QMAKE_TARGET_COPYRIGHT = CopyRight TokiNoBug


#DEFINES += QT_NO_WARNING_OUTPUT\
#                QT_NO_DEBUG_OUTPUT
#仅在发布时取消注释

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Collecter.cpp \
    NBTWriter.cpp \
    Page0.cpp \
    Page1.cpp \
    Page2.cpp \
    Page3.cpp \
    Page4.cpp \
    Page5.cpp \
    Page6.cpp \
    Page7.cpp \
    Page8.cpp \
    Pages.cpp \
    adjt.cpp \
    main.cpp \
    mainwindow.cpp \
    mcmap.cpp \
    optitree.cpp \
    tokicolor.cpp \
    tpstrategywind.cpp \
    zlibs/adler32.c \
    zlibs/compress.c \
    zlibs/crc32.c \
    zlibs/deflate.c \
    zlibs/gzclose.c \
    zlibs/gzlib.c \
    zlibs/gzread.c \
    zlibs/gzwrite.c \
    zlibs/infback.c \
    zlibs/inffast.c \
    zlibs/inflate.c \
    zlibs/inftrees.c \
    zlibs/trees.c \
    zlibs/uncompr.c \
    zlibs/zutil.c

HEADERS += \
    NBTWriter.h \
    mainwindow.h \
    optitree.h \
    tpstrategywind.h \
    zlibs/crc32.h \
    zlibs/deflate.h \
    zlibs/gzguts.h \
    zlibs/inffast.h \
    zlibs/inffixed.h \
    zlibs/inflate.h \
    zlibs/inftrees.h \
    zlibs/trees.h \
    zlibs/zconf.h \
    zlibs/zlib.h \
    zlibs/zutil.h

FORMS += \
    mainwindow.ui \
 \    #previewer.ui
    tpstrategywind.ui

TRANSLATIONS += \
    Slope_zh_CN.ts \
    Slope_en_US.ts

CODECFORTR = utf-8



CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../BlockTextures.qrc \
    ../Languages.qrc \
    Pics.qrc

QT += widgets
