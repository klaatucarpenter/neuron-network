#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "modelparser.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->runButton->setEnabled(false);
    ui->fileNameField->setReadOnly(true);
    ui->modelInfo->setReadOnly(true);
    ui->outputView->setReadOnly(true);
    ui->tabWidget->setCurrentIndex(0);
    ui->outputView->setLineWrapMode(QPlainTextEdit::NoWrap);
    ui->outputView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->outputView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static QString actToStr(Activation a){
    switch(a){
    case Activation::Relu:    return "relu";
    case Activation::Sigmoid: return "sigmoid";
    case Activation::Tanh:    return "tanh";
    case Activation::Softmax: return "softmax";
    default:                  return "linear";
    }
}

static QString makeModelSummary(const ParsedFile& pf)
{
    const auto& net = pf.network;

    int outputSize = net.layers.empty() ? 0 : net.layers.back().outDim;
    std::size_t totalWeights = 0, totalBiases = 0;
    double wMin =  std::numeric_limits<double>::infinity();
    double wMax = -std::numeric_limits<double>::infinity();
    double bMin =  std::numeric_limits<double>::infinity();
    double bMax = -std::numeric_limits<double>::infinity();

    for (const auto& L : net.layers) {
        totalWeights += L.weights.size();
        totalBiases  += L.biases.size();
        if (!L.weights.empty()) {
            for (double v : L.weights) { if (v < wMin) wMin = v; if (v > wMax) wMax = v; }
        }
        if (!L.biases.empty()) {
            for (double v : L.biases)  { if (v < bMin) bMin = v; if (v > bMax) bMax = v; }
        }
    }
    const std::size_t totalParams = totalWeights + totalBiases;

    QString out;
    out += QString("MODEL SUMMARY\n");
    out += QString("Input size: %1\n").arg(net.inputSize);
    out += QString("Layers: %1\n").arg(net.layers.size());
    out += QString("Output size: %1\n").arg(outputSize);
    out += "\n";

    // Tabelka warstw
    out += "Layers (index: type in->out, act, weights, biases)\n";
    for (int i = 0; i < (int)net.layers.size(); ++i) {
        const auto& L = net.layers[i];
        out += QString("  %1: Dense %2->%3, act=%4, weights=%5, biases=%6\n")
                   .arg(i)
                   .arg(L.inDim).arg(L.outDim)
                   .arg(actToStr(L.activation))
                   .arg(L.weights.size())
                   .arg(L.biases.size());
    }
    out += "\n";

    if (totalWeights > 0) {
        out += QString("Weights range: [%1, %2]\n").arg(wMin, 0, 'g', 12).arg(wMax, 0, 'g', 12);
    } else {
        out += "Weights range: (no weights)\n";
    }
    if (totalBiases > 0) {
        out += QString("Biases range:  [%1, %2]\n").arg(bMin, 0, 'g', 12).arg(bMax, 0, 'g', 12);
    } else {
        out += "Biases range:  (no biases)\n";
    }
    out += "\n";

    out += QString("DATA rows: %1\n").arg(pf.inputs.size());
    if (!pf.inputs.empty()) {
        const auto& v0 = pf.inputs.front();
        out += QString("Preview DATA[0] (%1): ").arg(v0.size());
        QStringList parts;
        for (double v : v0) parts << QString::number(v, 'g', 10);
        out += parts.join(' ') + "\n";
    }
    return out;
}

void MainWindow::on_loadFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Wybierz plik"), "",
                                                    tr("Wszystkie pliki (*.*)"));

    if (!fileName.isEmpty()) {
        ui->runButton->setEnabled(false);
        ui->fileNameField->setText(fileName);
        parsed_ = parseInputFile(fileName.toStdString());

        ui->modelInfo->clear();
        ui->modelInfo->appendPlainText(makeModelSummary(parsed_));
        ui->runButton->setEnabled(true);
        statusBar()->showMessage(tr("Model loaded"), 5000);
    }
}

void MainWindow::on_runButton_clicked()
{
    ui->tabWidget->setCurrentIndex(2);
    ui->outputView->clear();

    const int nRows = static_cast<int>(parsed_.inputs.size());
    const int outDim = parsed_.network.layers.empty()
                           ? 0
                           : parsed_.network.layers.back().outDim;
    QElapsedTimer timer;
    timer.start();
    const int PREC = 6;
    const int WIDTH = 12;

    ui->outputView->appendPlainText("=== RUN ===");
    for (int r = 0; r < nRows; ++r) {
        const auto& x = parsed_.inputs[r];
        auto y = parsed_.network.inference(x);

        QString line = QString("%1 ").arg(r, 3);
        for (int j = 0; j < outDim; ++j) {
            line += " | ";
            line += QString("%1").arg(QString::number(y[j], 'f', PREC), WIDTH);
        }

        ui->outputView->appendPlainText(line);
    }

    ui->outputView->appendPlainText(
        QString("=== DONE (%1 rows in %2 ms) ===")
            .arg(nRows)
            .arg(timer.elapsed()));
}

