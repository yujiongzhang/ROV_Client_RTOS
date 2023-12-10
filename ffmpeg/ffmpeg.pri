HEADERS += $$PWD/ffmpeghead.h
HEADERS += $$PWD/ffmpeg.h
SOURCES += $$PWD/ffmpeg.cpp


INCLUDEPATH += $$PWD/ffmpeg4/include64
LIBS += -L$$PWD/bin -lavcodec-58 -lavdevice-58 -lavfilter-7 -lavformat-58 -lavutil-56 -lpostproc-55 -lswresample-3 -lswscale-5
