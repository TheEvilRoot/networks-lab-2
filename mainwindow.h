#pragma once

#include <QMainWindow>
#include <QList>

#include <memory>

#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct ByteStuffing {
    QString initialString;
    QString resultString;

    QString initialBytes;
    QStringList resultBytes;

    QList<int> appendedIndeces;

    ByteStuffing(const QString &message, unsigned char flag, unsigned char esc, unsigned char code) {
        initialString = message;

        for (int  i = 0; i < message.size(); i++) {
            if (message[i] == flag) {
                appendResult(esc, true);
                appendResult(code, true);
            } else if (message[i] == esc) {
                appendResult(esc, true);
                appendResult(esc);
            } else {
                appendResult(message[i]);
            }
            initialBytes.append(QString::number(message[i].cell(), 16) + " ");
        }
    }

private:
    void appendResult(unsigned char c, bool isExternal = false) {
        resultString.append(c);
        resultBytes.append(QString::number(c, 16));

        if (isExternal)
            appendedIndeces.append(resultString.size() - 1);
    }

    void appendResult(QChar c, bool isExternal = false) {
        resultString.append(c);
        resultBytes.append(QString::number(c.cell(), 16));

        if (isExternal)
            appendedIndeces.append(resultString.size() - 1);
    }
};

struct DeByteStuffing {
    QString resultString;
    QString resultBytes;

    DeByteStuffing(const QString &message, unsigned char flag, unsigned char esc, unsigned char code) {
        for (int i = 0; i < message.size(); i++) {
            if (message[i] == esc && i < message.size() - 1) {
                i++;
                if (message[i] == code) {
                    appendResult(flag);
                } else if (message[i] == esc) {
                    appendResult(esc);
                }
            } else {
                appendResult(message[i]);
            }
        }
    }

private:
    void appendResult(unsigned char c) {
        resultString.append(c);
        resultBytes.append(QString::number(c, 16) + " ");
    }

    void appendResult(QChar c) {
        resultString.append(c);
        resultBytes.append(QString::number(c.cell(), 16) + " ");
    }
};

class MainWindow : public QMainWindow {
Q_OBJECT
private:
    std::unique_ptr<Ui::MainWindow> ui;

    const unsigned char flag{102};
    const unsigned char code{103};
    const unsigned char esc {'^'};

public:
    MainWindow(QWidget* = nullptr): ui{new Ui::MainWindow} {
        ui->setupUi(this);
        populateStatic();
        initUi();
    }

    void initUi() {
        connect(ui->send, &QPushButton::clicked, [&]() {
            auto input = ui->input->text();
            if (input.isEmpty()) return;

            ByteStuffing stuffing(input, flag, esc,code);
            DeByteStuffing deStuffing(stuffing.resultString, flag, esc, code);
            populateResult(stuffing, deStuffing);

            ui->input->clear();
        });
    }

    void populateResult(const ByteStuffing &stuffing, const DeByteStuffing &deStuffing) {
        ui->befure->setText(stuffing.initialString);
        ui->befureBytes->setText(stuffing.initialBytes);
        ui->afterDe->setText(deStuffing.resultString);
        ui->afterDeBytes->setText(deStuffing.resultBytes);
        ui->output->setText(deStuffing.resultString);

        QString resultString;
        QString resultBytes;

        for (int i = 0; i < stuffing.resultString.size() && i < stuffing.resultBytes.size(); i++) {
            if (stuffing.appendedIndeces.contains(i)) {
                resultString.append("<font color=red>" + escapeHtml(stuffing.resultString[i]) + "</font>");
                resultBytes.append("<font color=red>" + escapeHtml(stuffing.resultBytes[i]) + " </font> ");
            } else {
                resultString.append(escapeHtml(stuffing.resultString[i]));
                resultBytes.append(escapeHtml(stuffing.resultBytes[i]) + " ");
            }
        }

        ui->after->setText(resultString);
        ui->afterBytes->setText(resultBytes);
    }

    void populateStatic() {
        ui->flag->setText(charHex(flag));
        ui->code->setText(charHex(code));
        ui->esc->setText(charHex(esc));
    }

    static QString escapeHtml(QChar c) {
        if (c == '>') return "&gt;";
        if (c == '<') return "&lt;";
        return QString(c);
    }

    static QString escapeHtml(QString str) {
        return str.replace(">", "&gt;").replace("<", "&lt;");
    }

    static QString charHex(unsigned char cc) {
        return QString(cc) + " (0x" + QString::number(cc, 16) + ")";
    }
};
