import qbs.FileInfo

QtApplication {
    Depends { name: "Qt.widgets" }
    cpp.defines: []
    files: [
        "main.cc",
        "mainwindow.h",
        "mainwindow.ui",
    ]
    install: true
    installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}
