#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QKeySequence>
#include <QSettings>
#include <QStyle>
#include <QFrame>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    fileModel = new QFileSystemModel(this);
    fileModel->setRootPath(QDir::rootPath());

    searchManager = new SearchManager(this);
    searchManager->setSourceModel(fileModel);

    ui->folderTreeView->setModel(fileModel);
    ui->folderTreeView->setRootIndex(fileModel->index(QDir::rootPath()));

    ui->fileTableView->setModel(searchManager->proxyModel());
    ui->fileTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->fileTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->fileTableView->setAlternatingRowColors(true);
    ui->fileTableView->verticalHeader()->setVisible(false);
    QModelIndex sourceRoot = fileModel->index(QDir::rootPath());

    QModelIndex proxyRoot =
        searchManager->proxyModel()->mapFromSource(sourceRoot);

    ui->fileTableView->setRootIndex(proxyRoot);
    ui->fileTableView->horizontalHeader()->setStretchLastSection(true);
    ui->fileTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->folderTreeView->hideColumn(1);
    ui->folderTreeView->hideColumn(2);
    ui->folderTreeView->hideColumn(3);
    ui->folderTreeView->setAnimated(true);
    ui->folderTreeView->setIndentation(20);
    ui->folderTreeView->setSortingEnabled(true);

    navigationManager = new NavigationManager(this);

    connect(ui->folderTreeView, &QTreeView::clicked,
            this, &MainWindow::onFolderClicked);
    connect(ui->fileTableView,
            &QTableView::doubleClicked,
            this,
            &MainWindow::onFileDoubleClicked);
    connect(ui->addressBaar, &QLineEdit::returnPressed,
            this, &MainWindow::onAddressBarReturnPressed);

    connect(ui->actionBack, &QAction::triggered,
            navigationManager, &NavigationManager::goBack);
    connect(ui->actionForward, &QAction::triggered,
            navigationManager, &NavigationManager::goForward);
    connect(ui->actionUp, &QAction::triggered,
            navigationManager, &NavigationManager::goUp);
    connect(ui->actionRefresh, &QAction::triggered,
            navigationManager, &NavigationManager::refresh);

    connect(navigationManager, &NavigationManager::currentPathChanged,
            this, &MainWindow::onNavigationPathChanged);
    connect(navigationManager, &NavigationManager::canGoBackChanged,
            ui->actionBack, &QAction::setEnabled);
    connect(navigationManager, &NavigationManager::canGoForwardChanged,
            ui->actionForward, &QAction::setEnabled);
    connect(navigationManager, &NavigationManager::navigationFailed,
            this, &MainWindow::onNavigationFailed);

    // Remember the current folder on every move so we can resume there next launch.
    connect(navigationManager, &NavigationManager::currentPathChanged, this, [](const QString &path) {
        QSettings settings;
        settings.setValue("lastFolder", path);
    });

    connect(ui->searchBar,
            &QLineEdit::textChanged,
            searchManager,
            &SearchManager::setFilterText);

    ui->actionBack->setEnabled(false);
    ui->actionForward->setEnabled(false);

    applyToolbarIcons();
    applyModernStyle();

    itemCountLabel = new QLabel(this);
    ui->statusbar->addPermanentWidget(itemCountLabel);
    connect(searchManager->proxyModel(), &QAbstractItemModel::modelReset,
            this, &MainWindow::updateItemCountLabel);
    connect(searchManager->proxyModel(), &QAbstractItemModel::layoutChanged,
            this, &MainWindow::updateItemCountLabel);
    connect(searchManager->proxyModel(), &QAbstractItemModel::rowsInserted,
            this, &MainWindow::updateItemCountLabel);
    connect(searchManager->proxyModel(), &QAbstractItemModel::rowsRemoved,
            this, &MainWindow::updateItemCountLabel);

    restoreLastFolder();

    // --- File operations: actions (also usable as shortcuts), context menu, status feedback ---
    fileOperations = new FileOperations(this);

    actionNewFolder = new QAction(tr("New Folder"), this);
    actionNewFolder->setShortcut(QKeySequence("Ctrl+Shift+N"));

    actionRename = new QAction(tr("Rename"), this);
    actionRename->setShortcut(Qt::Key_F2);
    actionRename->setEnabled(false);

    actionDelete = new QAction(tr("Delete"), this);
    actionDelete->setShortcut(QKeySequence::Delete);
    actionDelete->setEnabled(false);

    actionCopy = new QAction(tr("Copy"), this);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionCopy->setEnabled(false);

    actionCut = new QAction(tr("Cut"), this);
    actionCut->setShortcut(QKeySequence::Cut);
    actionCut->setEnabled(false);

    actionPaste = new QAction(tr("Paste"), this);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionPaste->setEnabled(false);

    actionProperties =
        new QAction(tr("Properties"),this);

    // Registering the actions on the table view (with a shortcut context that
    // includes children) makes the shortcuts fire while the view has focus.
    ui->fileTableView->addAction(actionNewFolder);
    ui->fileTableView->addAction(actionRename);
    ui->fileTableView->addAction(actionDelete);
    ui->fileTableView->addAction(actionCopy);
    ui->fileTableView->addAction(actionCut);
    ui->fileTableView->addAction(actionPaste);
    ui->fileTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->fileTableView->addAction(actionProperties);

    connect(ui->fileTableView, &QTableView::customContextMenuRequested,
            this, &MainWindow::onFileTableContextMenu);

    connect(actionNewFolder, &QAction::triggered, this, &MainWindow::onNewFolder);
    connect(actionRename, &QAction::triggered, this, &MainWindow::onRename);
    connect(actionDelete, &QAction::triggered, this, &MainWindow::onDeleteSelected);
    connect(actionCopy, &QAction::triggered, this, &MainWindow::onCopySelected);
    connect(actionCut, &QAction::triggered, this, &MainWindow::onCutSelected);
    connect(actionPaste, &QAction::triggered, this, &MainWindow::onPasteHere);
    connect(actionProperties,
            &QAction::triggered,
            this,
            &MainWindow::onProperties);

    connect(fileOperations, &FileOperations::clipboardChanged,
            actionPaste, &QAction::setEnabled);
    connect(fileOperations, &FileOperations::operationSucceeded,
            this, [this](const QString &msg) { ui->statusbar->showMessage(msg, 3000); });
    connect(fileOperations, &FileOperations::operationFailed,
            this, [this](const QString &msg) { ui->statusbar->showMessage(msg, 5000); });

    // Enable/disable selection-dependent actions as the current row changes.
    connect(ui->fileTableView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [=]()
            {
                const int count = ui->fileTableView->selectionModel()->selectedRows().size();

                actionRename->setEnabled(count == 1);
                actionDelete->setEnabled(count >= 1);
                actionCopy->setEnabled(count >= 1);
                actionCut->setEnabled(count >= 1);

                updateItemCountLabel();
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applyToolbarIcons()
{
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    ui->actionBack->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    ui->actionForward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    ui->actionUp->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    ui->actionRefresh->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

    ui->actionBack->setToolTip(tr("Go back"));
    ui->actionForward->setToolTip(tr("Go forward"));
    ui->actionUp->setToolTip(tr("Go up one level"));
    ui->actionRefresh->setToolTip(tr("Refresh"));
}

void MainWindow::applyModernStyle()
{
    // A little breathing room and leading icons on the two line edits.
    ui->addressBaar->addAction(style()->standardIcon(QStyle::SP_DirIcon), QLineEdit::LeadingPosition);
    ui->searchBar->addAction(style()->standardIcon(QStyle::SP_FileDialogContentsView), QLineEdit::LeadingPosition);
    ui->addressBaar->setMinimumHeight(34);
    ui->searchBar->setMinimumHeight(34);

    ui->folderTreeView->setUniformRowHeights(true);
    ui->folderTreeView->verticalScrollBar()->setSingleStep(20);

    ui->fileTableView->verticalHeader()->setDefaultSectionSize(30);
    ui->fileTableView->setShowGrid(false);
    ui->fileTableView->setFrameShape(QFrame::NoFrame);
    ui->folderTreeView->setFrameShape(QFrame::NoFrame);

    setStyleSheet(R"(
        QMainWindow, QWidget#centralwidget {
            background-color: #f4f5f7;
        }
        QToolBar {
            background-color: #ffffff;
            border: none;
            border-bottom: 1px solid #e5e7eb;
            padding: 6px 8px;
            spacing: 6px;
        }
        QToolBar QToolButton {
            padding: 6px 10px;
            border-radius: 6px;
            color: #333333;
        }
        QToolBar QToolButton:hover {
            background-color: #e8edf7;
        }
        QToolBar QToolButton:pressed {
            background-color: #d3e0fb;
        }
        QLineEdit {
            padding: 6px 10px;
            border: 1px solid #dcdfe4;
            border-radius: 8px;
            background-color: #ffffff;
            selection-background-color: #2854c9;
            font-size: 13px;
        }
        QLineEdit:focus {
            border: 1px solid #2854c9;
        }
        QLabel#foldersLabel {
            color: #8a8f98;
            font-weight: 600;
            font-size: 11px;
            padding: 4px 2px;
        }
        QTreeView, QTableView {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            alternate-background-color: #fafbfc;
            outline: none;
        }
        QTreeView::item, QTableView::item {
            padding: 4px 6px;
            border: none;
        }
        QTreeView::item:selected, QTableView::item:selected {
            background-color: #2854c9;
            color: white;
        }
        QTreeView::item:hover:!selected, QTableView::item:hover:!selected {
            background-color: #eef1f8;
        }
        QTreeView::branch {
            background: transparent;
        }
        QHeaderView::section {
            background-color: #fafbfc;
            border: none;
            border-bottom: 1px solid #e5e7eb;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 12px;
            color: #6b7280;
        }
        QStatusBar {
            background-color: #ffffff;
            border-top: 1px solid #e5e7eb;
            color: #6b7280;
        }
        QStatusBar QLabel {
            color: #6b7280;
            font-size: 12px;
            padding: 0 6px;
        }
        QSplitter::handle {
            background-color: #f4f5f7;
            width: 8px;
        }
        QMenu {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            padding: 4px;
        }
        QMenu::item {
            padding: 6px 24px 6px 12px;
            border-radius: 6px;
            color: #333333;
        }
        QMenu::item:selected {
            background-color: #2854c9;
            color: white;
        }
        QMenu::separator {
            height: 1px;
            background: #e5e7eb;
            margin: 4px 8px;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 10px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #d1d5db;
            border-radius: 5px;
            min-height: 24px;
        }
        QScrollBar::handle:vertical:hover {
            background: #9ca3af;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
    )");
}


void MainWindow::restoreLastFolder()
{
    QSettings settings;
    const QString lastFolder = settings.value("lastFolder", QDir::rootPath()).toString();
    const QFileInfo info(lastFolder);

    navigationManager->navigateTo(info.exists() && info.isDir() ? lastFolder : QDir::rootPath());
}

void MainWindow::updateItemCountLabel()
{
    const int totalCount = searchManager->proxyModel()->rowCount(ui->fileTableView->rootIndex());
    const int selectedCount = ui->fileTableView->selectionModel()
                                  ? ui->fileTableView->selectionModel()->selectedRows().size()
                                  : 0;

    QString text = tr("%1 item%2").arg(totalCount).arg(totalCount == 1 ? "" : "s");
    if (selectedCount > 0)
        text += tr(" · %1 selected").arg(selectedCount);

    itemCountLabel->setText(text);
}

void MainWindow::onNavigationFailed(const QString &path)
{
    ui->statusbar->showMessage(tr("\"%1\" is not a valid folder.").arg(path), 4000);
    ui->addressBaar->setText(navigationManager->currentPath());
}

void MainWindow::onFolderClicked(const QModelIndex &index)
{
    navigationManager->navigateTo(fileModel->filePath(index));
}

void MainWindow::onFileDoubleClicked(const QModelIndex &proxyIndex)
{
    QModelIndex sourceIndex =
        searchManager->proxyModel()->mapToSource(proxyIndex);

    QString path = fileModel->filePath(sourceIndex);

    QFileInfo info(path);

    if (info.isDir())
    {
        navigationManager->navigateTo(path);
    }
    else if (info.isFile())
    {
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
            ui->statusbar->showMessage(tr("Could not open \"%1\".").arg(info.fileName()), 4000);
    }
}

void MainWindow::onAddressBarReturnPressed()
{
    navigationManager->navigateTo(ui->addressBaar->text());
}

void MainWindow::onNavigationPathChanged(const QString &path)
{
    QModelIndex sourceIndex = fileModel->index(path);

    QModelIndex proxyIndex =
        searchManager->proxyModel()->mapFromSource(sourceIndex);

    ui->fileTableView->setRootIndex(proxyIndex);

    ui->folderTreeView->setCurrentIndex(sourceIndex);
    ui->folderTreeView->scrollTo(sourceIndex);

    ui->addressBaar->setText(path);
    updateItemCountLabel();
}

void MainWindow::onFileTableContextMenu(const QPoint &pos)
{
    const QModelIndex index = ui->fileTableView->indexAt(pos);

    if(index.isValid())
        ui->fileTableView->setCurrentIndex(index);

    QMenu menu(this);

    menu.addAction(actionNewFolder);

    menu.addSeparator();

    menu.addAction(actionCopy);
    menu.addAction(actionCut);
    menu.addAction(actionPaste);

    menu.addSeparator();

    menu.addAction(actionRename);
    menu.addAction(actionDelete);

    menu.addSeparator();
    menu.addAction(actionProperties);

    menu.exec(ui->fileTableView->viewport()->mapToGlobal(pos));
}

void MainWindow::onNewFolder()
{
    const QString newFolderPath = fileOperations->createNewFolder(navigationManager->currentPath());

    if (!newFolderPath.isEmpty())
    {
        QModelIndex sourceIndex = fileModel->index(newFolderPath);

        QModelIndex proxyIndex =
            searchManager->proxyModel()->mapFromSource(sourceIndex);

        ui->fileTableView->setCurrentIndex(proxyIndex);
        ui->fileTableView->edit(proxyIndex);
    }
}

void MainWindow::onRename()
{
    QModelIndex proxyIndex = ui->fileTableView->currentIndex();
    if(selectedPaths().size() != 1)
    {
        QMessageBox::information(
            this,
            "Rename",
            "Please select exactly one item.");
        return;
    }

    if (!proxyIndex.isValid())
        return;

    QModelIndex sourceIndex =
        searchManager->proxyModel()->mapToSource(proxyIndex);

    QString oldPath = fileModel->filePath(sourceIndex);
    QString oldName = fileModel->fileName(sourceIndex);

    bool ok = false;
    const QString newName = QInputDialog::getText(this, tr("Rename"), tr("New name:"),
                                                  QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty() && newName != oldName)
        fileOperations->renamePath(oldPath, newName);
}

void MainWindow::onDeleteSelected()
{
    const QStringList paths = selectedPaths();
    if (paths.isEmpty())
        return;

    const QString message = paths.size() == 1
                                ? tr("Are you sure you want to delete \"%1\"? This cannot be undone.")
                                      .arg(QFileInfo(paths.first()).fileName())
                                : tr("Are you sure you want to delete these %1 items? This cannot be undone.")
                                      .arg(paths.size());

    const auto reply = QMessageBox::question(this, tr("Delete"), message,
                                             QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
        fileOperations->deletePaths(paths);
}

void MainWindow::onCopySelected()
{
    QStringList paths = selectedPaths();

    if (paths.isEmpty())
        return;

    fileOperations->copyToClipboard(paths);

    ui->statusbar->showMessage(
        tr("%1 item(s) copied.").arg(paths.size()),
        2000);
}

void MainWindow::onCutSelected()
{
    QStringList paths = selectedPaths();

    if (paths.isEmpty())
        return;

    fileOperations->cutToClipboard(paths);

    ui->statusbar->showMessage(
        tr("%1 item(s) cut.").arg(paths.size()),
        2000);
}

void MainWindow::onPasteHere()
{
    fileOperations->pasteTo(navigationManager->currentPath());
}

QStringList MainWindow::selectedPaths() const
{
    QStringList paths;

    QModelIndexList proxyRows =
        ui->fileTableView->selectionModel()->selectedRows();

    for (const QModelIndex &proxyIndex : proxyRows)
    {
        QModelIndex sourceIndex =
            searchManager->proxyModel()->mapToSource(proxyIndex);

        paths << fileModel->filePath(sourceIndex);
    }

    return paths;
}

void MainWindow::onProperties()
{
    const QStringList paths = selectedPaths();

    if (paths.size() != 1)
    {
        ui->statusbar->showMessage(tr("Select exactly one item to view its properties."), 3000);
        return;
    }

    PropertiesDialog dialog(paths.first(), this);
    dialog.exec();
}