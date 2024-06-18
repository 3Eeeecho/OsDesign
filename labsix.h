#ifndef LABSIX_H
#define LABSIX_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <unordered_map>

struct File {
    QString name;
    QString content;
    int size;

    File(QString name = "") : name(name), size(0) {}
};

struct Directory {
    QString name;
    std::unordered_map<QString, File> files;
    std::unordered_map<QString, Directory> subdirectories;

    Directory(QString name = "") : name(name) {}
};

struct FileDescriptor {
    QString fileName;
    QString directoryPath;
    int offset;

    FileDescriptor(QString fileName = "", QString directoryPath = "", int offset = 0)
        : fileName(fileName), directoryPath(directoryPath), offset(offset) {}
};

class LabSix : public QMainWindow
{
    Q_OBJECT

public:
    LabSix(QWidget *parent = nullptr);
    ~LabSix();

private slots:
    void executeOperation();
    void createFile();
    void deleteFile();
    void openFile();
    void closeFile();
    void readFile();
    void writeFile();
    void createDirectory();
    void changeDirectory();
    void updateDisplay();

private:
    void setupUI();
    Directory* navigateToDirectory(const QString& path);
    QString getFullPath(const QString& path, const QString& name);
    QString displayDirectory(const Directory* dir, int depth)const;

    Directory root;
    std::unordered_map<int, FileDescriptor> openFiles;
    int nextFd;
    QString currentPath;

    QComboBox *operationComboBox;
    QPushButton *executeButton;
    QTextEdit *display;
};

#endif // LABSIX_H
