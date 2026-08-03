#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QAction>
#include <QLabel>

#include "propertiesdialog.h"
#include "navigationmanager.h"
#include "fileoperations.h"
#include "searchmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onFolderClicked(const QModelIndex &index);
    void onFileDoubleClicked(const QModelIndex &index);
    void onAddressBarReturnPressed();
    void onNavigationPathChanged(const QString &path);
    void onNavigationFailed(const QString &path);

    void onFileTableContextMenu(const QPoint &pos);
    void onNewFolder();
    void onRename();
    void onDeleteSelected();
    void onCopySelected();
    void onCutSelected();
    void onPasteHere();
    void onProperties();

private:
    void applyToolbarIcons();
    void applyModernStyle();
    void updateItemCountLabel();
    void restoreLastFolder();

    Ui::MainWindow *ui;
    QStringList selectedPaths() const;
    QFileSystemModel *fileModel;
    NavigationManager *navigationManager;
    FileOperations *fileOperations;
    SearchManager *searchManager;
    QLabel *itemCountLabel;

    QAction *actionNewFolder;
    QAction *actionRename;
    QAction *actionDelete;
    QAction *actionCopy;
    QAction *actionCut;
    QAction *actionPaste;
    QAction *actionProperties;
};
#endif // MAINWINDOW_H
