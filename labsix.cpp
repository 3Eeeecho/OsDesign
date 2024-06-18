#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QString>
#include "labsix.h"
LabSix::LabSix(QWidget *parent)
    : QMainWindow(parent), root("root"), nextFd(0)
{
    setupUI();
    updateDisplay();
}
LabSix::~LabSix()
{
}
void LabSix::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    operationComboBox = new QComboBox(this);
    operationComboBox->setFixedHeight(30);
    operationComboBox->addItem("创建目录");
    operationComboBox->addItem("切换目录");
    operationComboBox->addItem("创建文件");
    operationComboBox->addItem("删除文件");
    operationComboBox->addItem("打开文件");
    operationComboBox->addItem("关闭文件");
    operationComboBox->addItem("读文件");
    operationComboBox->addItem("写文件");

    executeButton = new QPushButton("执行", this);
    display = new QTextEdit(this);
    display->setReadOnly(true);

    mainLayout->addWidget(new QLabel("选择操作:", this));
    mainLayout->addWidget(operationComboBox);
    mainLayout->addWidget(executeButton);
    mainLayout->addWidget(new QLabel("文件系统状态:", this));
    mainLayout->addWidget(display);

    setCentralWidget(centralWidget);

    connect(executeButton, &QPushButton::clicked, this, &LabSix::executeOperation);

    setWindowTitle("实验六");
}

void LabSix::executeOperation()
{
    QString operation = operationComboBox->currentText();
    if (operation == "创建文件") {
        createFile();
    } else if (operation == "删除文件") {
        deleteFile();
    } else if (operation == "打开文件") {
        openFile();
    } else if (operation == "关闭文件") {
        closeFile();
    } else if (operation == "读文件") {
        readFile();
    } else if (operation == "写文件") {
        writeFile();
    } else if (operation == "创建目录") {
        createDirectory();
    } else if (operation == "切换目录") {
        changeDirectory();
    }
}

void LabSix::createFile()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, "创建文件", "输入文件名:", QLineEdit::Normal, "", &ok);
    if (ok && !fileName.isEmpty()) {
        Directory* dir = navigateToDirectory(currentPath);
        if (dir && dir->files.find(fileName) == dir->files.end()) {
            dir->files[fileName] = File(fileName);
            updateDisplay();
        } else {
            QMessageBox::warning(this, "错误", "文件已存在或目录不存在！");
        }
    }
}
void LabSix::deleteFile()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, "删除文件", "输入文件名:", QLineEdit::Normal, "", &ok);
    if (ok && !fileName.isEmpty()) {
        auto it = root.files.find(fileName);
        if (it == root.files.end()) {
            QMessageBox::warning(this, "错误", "文件不存在！");
        } else {
            root.files.erase(it);
            updateDisplay();
        }
    }
}
void LabSix::openFile()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, "打开文件", "输入文件名:", QLineEdit::Normal, "", &ok);
    if (ok && !fileName.isEmpty()) {
        auto it = root.files.find(fileName);
        if (it == root.files.end()) {
            QMessageBox::warning(this, "错误", "文件不存在！");
        } else {
            int fd = nextFd++;
            openFiles[fd] = FileDescriptor(fileName);
            updateDisplay();
        }
    }
}
void LabSix::closeFile()
{
    bool ok;
    int fd = QInputDialog::getInt(this, "关闭文件", "输入文件描述符:", 0, 0, nextFd - 1, 1, &ok);
    if (ok) {
        auto it = openFiles.find(fd);
        if (it == openFiles.end()) {
            QMessageBox::warning(this, "错误", "文件描述符无效！");
        } else {
            openFiles.erase(it);
            updateDisplay();
        }
    }
}
void LabSix::readFile()
{
    bool ok;
    int fd = QInputDialog::getInt(this, "读文件", "输入文件描述符:", 0, 0, nextFd - 1, 1, &ok);
    if (ok) {
        auto it = openFiles.find(fd);
        if (it == openFiles.end()) {
            QMessageBox::warning(this, "错误", "文件描述符无效！");
        } else {
            File &file = root.files[it->second.fileName];
            QMessageBox::information(this, "文件内容", file.content);
        }
    }
}
void LabSix::writeFile()
{
    bool ok;
    int fd = QInputDialog::getInt(this, "写文件", "输入文件描述符:", 0, 0, nextFd - 1, 1, &ok);
    if (ok) {
        auto it = openFiles.find(fd);
        if (it == openFiles.end()) {
            QMessageBox::warning(this, "错误", "文件描述符无效！");
        } else {
            QString content = QInputDialog::getText(this, "写文件", "输入文件内容:", QLineEdit::Normal, "", &ok);
            if (ok) {
                File &file = root.files[it->second.fileName];
                file.content += content;
                file.size = file.content.size();
                updateDisplay();
            }
        }
    }
}

QString LabSix::displayDirectory(const Directory* dir, int depth) const{
    QString status;
    QString indent = QString("  ").repeated(depth);
    for (const auto& pair : dir->files) {
        status += indent + pair.first + " (大小: " + QString::number(pair.second.size) + ")\n";
    }
    for (const auto& pair : dir->subdirectories) {
        status += indent + "[" + pair.first + "]\n";
        status += displayDirectory(&pair.second, depth + 1);
    }
    return status;
}

void LabSix::updateDisplay()
{
    QString status = "当前路径: " + currentPath + "\n\n";
    status += "根目录:\n" + displayDirectory(&root, 0);
    status += "\n打开的文件:\n";
    for (const auto &pair : openFiles) {
        status += "描述符: " + QString::number(pair.first) + ", 文件路径: " + pair.second.directoryPath + "/" + pair.second.fileName + ", 偏移量: " + QString::number(pair.second.offset) + "\n";
    }
    display->setText(status);
}

Directory* LabSix::navigateToDirectory(const QString& path) {
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    Directory* dir = &root;
    for (const QString& part : parts) {
        if (dir->subdirectories.find(part) == dir->subdirectories.end()) {
            return nullptr;
        }
        dir = &dir->subdirectories[part];
    }
    return dir;
}

QString LabSix::getFullPath(const QString& path, const QString& name) {
    return path.isEmpty() ? name : path + "/" + name;
}

void LabSix::createDirectory()
{
    bool ok;
    QString dirName = QInputDialog::getText(this, "创建目录", "输入目录名:", QLineEdit::Normal, "", &ok);
    if (ok && !dirName.isEmpty()) {
        Directory* dir = navigateToDirectory(currentPath);
        if (dir && dir->subdirectories.find(dirName) == dir->subdirectories.end()) {
            dir->subdirectories[dirName] = Directory(dirName);
            updateDisplay();
        } else {
            QMessageBox::warning(this, "错误", "目录已存在或父目录不存在！");
        }
    }
}

void LabSix::changeDirectory()
{
    bool ok;
    QString path = QInputDialog::getText(this, "切换目录", "输入目录路径:", QLineEdit::Normal, "", &ok);
    if (ok && !path.isEmpty()) {
        if (navigateToDirectory(path)) {
            currentPath = path;
            updateDisplay();
        } else {
            QMessageBox::warning(this, "错误", "目录不存在！");
        }
    }
}
