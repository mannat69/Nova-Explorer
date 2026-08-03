#include "propertiesdialog.h"
#include "ui_PropertiesDialog.h"
#include "utility.h"

#include <QFileInfo>
#include <QFileIconProvider>
#include <QDateTime>

PropertiesDialog::PropertiesDialog(const QString &path,
                                   QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertiesDialog)
{
    ui->setupUi(this);

    const QFileInfo info(path);

    if (!info.exists())
    {
        ui->nameLabel->setText(tr("This item no longer exists."));
        ui->typeLabel->clear();
        ui->locationLabel->clear();
        ui->sizeLabel->clear();
        ui->createdLabel->clear();
        ui->modifiedLabel->clear();
        ui->permissionsLabel->clear();
        return;
    }

    QFileIconProvider provider;
    ui->iconLabel->setPixmap(provider.icon(info).pixmap(64, 64));
    ui->iconLabel->setAlignment(Qt::AlignHCenter);

    const QString displayName = info.fileName().isEmpty() ? info.absoluteFilePath() : info.fileName();

    // NOTE: setText() replaces the whole label, so the "Name:"/"Type:"/etc.
    // captions defined in the .ui file have to be included here rather than
    // just the value -- otherwise the labels end up blank captions.
    ui->nameLabel->setText(tr("Name: %1").arg(displayName));
    ui->typeLabel->setText(tr("Type: %1").arg(info.isDir() ? tr("Folder") : tr("File")));
    ui->locationLabel->setText(tr("Location: %1").arg(info.absolutePath()));

    ui->sizeLabel->setText(info.isDir()
        ? tr("Size: --")
        : tr("Size: %1").arg(Utility::formatFileSize(info.size())));

    ui->createdLabel->setText(tr("Created: %1")
        .arg(info.birthTime().isValid() ? info.birthTime().toString() : tr("Unknown")));

    ui->modifiedLabel->setText(tr("Modified: %1")
        .arg(info.lastModified().isValid() ? info.lastModified().toString() : tr("Unknown")));

    ui->permissionsLabel->setText(tr("Permissions: %1")
        .arg(Utility::formatPermissions(info.permissions())));

    setWindowTitle(tr("Properties — %1").arg(displayName));
}

PropertiesDialog::~PropertiesDialog()
{
    delete ui;
}
