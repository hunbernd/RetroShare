!include("retroshare.pri"): error("Could not include file retroshare.pri")

TEMPLATE = subdirs
#CONFIG += tests

SUBDIRS += openpgpsdk
openpgpsdk.file = openpgpsdk/src/openpgpsdk.pro

SUBDIRS += libbitdht
libbitdht.file = libbitdht/src/libbitdht.pro

SUBDIRS += libretroshare
libretroshare.file = libretroshare/src/libretroshare.pro
libretroshare.depends = openpgpsdk libbitdht

SUBDIRS += libresapi
libresapi.file = libresapi/src/libresapi.pro
libresapi.depends = libretroshare

retroshare_gui {
    SUBDIRS += retroshare_gui
    retroshare_gui.file = retroshare-gui/src/retroshare-gui.pro
    retroshare_gui.depends = libretroshare libresapi
    retroshare_gui.target = retroshare_gui
}

retroshare_nogui {
    SUBDIRS += retroshare_nogui
    retroshare_nogui.file = retroshare-nogui/src/retroshare-nogui.pro
    retroshare_nogui.depends = libretroshare libresapi
    retroshare_nogui.target = retroshare_nogui
}

retroshare_plugins {
    SUBDIRS += plugins
    plugins.file = plugins/plugins.pro
    plugins.depends = retroshare_gui
    plugins.target = plugins
}

wikipoos {
    SUBDIRS += pegmarkdown
    pegmarkdown.file = supportlibs/pegmarkdown/pegmarkdown.pro
    retroshare_gui.depends += pegmarkdown
}

tests {
    SUBDIRS += librssimulator
    librssimulator.file = tests/librssimulator/librssimulator.pro

    SUBDIRS += unittests
    unittests.file = tests/unittests/unittests.pro
    unittests.depends = libretroshare librssimulator
    unittests.target = unittests
}
