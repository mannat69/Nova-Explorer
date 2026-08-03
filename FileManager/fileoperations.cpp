#include "fileoperations.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

FileOperations::FileOperations(QObject *parent)
    : QObject(parent)
{
}

void FileOperations::copyToClipboard(const QStringList &paths)
{
    m_clipboardPaths = paths;
    m_clipboardMode = ClipboardMode::Copy;
    emit clipboardChanged(hasClipboardContent());
}

void FileOperations::cutToClipboard(const QStringList &paths)
{
    m_clipboardPaths = paths;
    m_clipboardMode = ClipboardMode::Cut;
    emit clipboardChanged(hasClipboardContent());
}

bool FileOperations::hasClipboardContent() const
{
    return !m_clipboardPaths.isEmpty() && m_clipboardMode != ClipboardMode::None;
}

FileOperations::ClipboardMode FileOperations::clipboardMode() const
{
    return m_clipboardMode;
}

bool FileOperations::pasteTo(const QString &destDir)
{
    if (!hasClipboardContent())
    {
        emit operationFailed(tr("Clipboard is empty."));
        return false;
    }

    const QFileInfo destInfo(destDir);
    if (!destInfo.exists() || !destInfo.isDir())
    {
        emit operationFailed(tr("Destination is not a valid folder."));
        return false;
    }

    QStringList failedNames;

    for (const QString &sourcePath : std::as_const(m_clipboardPaths))
    {
        const QFileInfo sourceInfo(sourcePath);
        if (!sourceInfo.exists())
        {
            failedNames << tr("%1 (not found)").arg(sourceInfo.fileName());
            continue;
        }

        // Refuse to paste a folder into itself or one of its own descendants.
        if (sourceInfo.isDir() &&
            (destInfo.absoluteFilePath() == sourceInfo.absoluteFilePath() ||
             destInfo.absoluteFilePath().startsWith(sourceInfo.absoluteFilePath() + "/")))
        {
            failedNames << tr("%1 (can't paste into itself)").arg(sourceInfo.fileName());
            continue;
        }

        const QString targetPath = uniqueDestinationPath(destDir, sourcePath);

        if (m_clipboardMode == ClipboardMode::Copy)
        {
            const bool ok = sourceInfo.isDir()
                ? copyRecursively(sourcePath, targetPath)
                : QFile::copy(sourcePath, targetPath);

            if (!ok)
                failedNames << sourceInfo.fileName();
        }
        else // Cut -> move
        {
            QDir parentDir;
            bool ok = parentDir.rename(sourcePath, targetPath);

            if (!ok)
            {
                // rename() fails across drives/filesystems; fall back to copy+delete.
                ok = sourceInfo.isDir()
                    ? copyRecursively(sourcePath, targetPath)
                    : QFile::copy(sourcePath, targetPath);

                if (ok)
                {
                    ok = sourceInfo.isDir()
                        ? QDir(sourcePath).removeRecursively()
                        : QFile::remove(sourcePath);
                }
            }

            if (!ok)
                failedNames << sourceInfo.fileName();
        }
    }

    const bool allSucceeded = failedNames.isEmpty();

    if (m_clipboardMode == ClipboardMode::Cut && allSucceeded)
    {
        m_clipboardPaths.clear();
        m_clipboardMode = ClipboardMode::None;
        emit clipboardChanged(false);
    }

    if (allSucceeded)
        emit operationSucceeded(tr("Paste completed."));
    else
        emit operationFailed(tr("Could not paste: %1").arg(failedNames.join(tr(", "))));

    return allSucceeded;
}

bool FileOperations::deletePaths(const QStringList &paths)
{
    QStringList failedNames;

    for (const QString &path : paths)
    {
        const QFileInfo info(path);

        if (!info.exists())
        {
            failedNames << tr("%1 (not found)").arg(info.fileName());
            continue;
        }

        const bool ok = info.isDir() ? QDir(path).removeRecursively() : QFile::remove(path);
        if (!ok)
            failedNames << info.fileName();
    }

    if (failedNames.isEmpty())
    {
        emit operationSucceeded(tr("Delete completed."));
        return true;
    }

    emit operationFailed(tr("Could not delete: %1").arg(failedNames.join(tr(", "))));
    return false;
}

bool FileOperations::renamePath(const QString &path, const QString &newName)
{
    const QFileInfo info(path);

    if (newName.isEmpty() || newName.contains('/') || newName.contains('\\'))
    {
        emit operationFailed(tr("Invalid name."));
        return false;
    }

    const QString newPath = info.absoluteDir().filePath(newName);

    if (QFileInfo::exists(newPath))
    {
        emit operationFailed(tr("An item named \"%1\" already exists.").arg(newName));
        return false;
    }

    QDir dir;
    const bool ok = dir.rename(path, newPath);

    if (ok)
        emit operationSucceeded(tr("Renamed to \"%1\".").arg(newName));
    else
        emit operationFailed(tr("Could not rename \"%1\".").arg(info.fileName()));

    return ok;
}

QString FileOperations::createNewFolder(const QString &parentDir, const QString &baseName)
{
    QDir dir(parentDir);

    if (!dir.exists())
    {
        emit operationFailed(tr("Parent folder does not exist."));
        return QString();
    }

    QString name = baseName;
    int suffix = 1;

    while (dir.exists(name))
        name = QString("%1 (%2)").arg(baseName).arg(++suffix);

    if (!dir.mkdir(name))
    {
        emit operationFailed(tr("Could not create folder \"%1\".").arg(name));
        return QString();
    }

    emit operationSucceeded(tr("Created folder \"%1\".").arg(name));
    return dir.filePath(name);
}

bool FileOperations::copyRecursively(const QString &sourcePath, const QString &destinationPath)
{
    const QFileInfo sourceInfo(sourcePath);

    if (sourceInfo.isDir())
    {
        QDir destDir(destinationPath);
        if (!destDir.mkpath("."))
            return false;

        const QDir sourceDir(sourcePath);
        const QFileInfoList entries = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

        for (const QFileInfo &entry : entries)
        {
            const QString newDestPath = destinationPath + "/" + entry.fileName();
            if (!copyRecursively(entry.absoluteFilePath(), newDestPath))
                return false;
        }

        return true;
    }

    return QFile::copy(sourcePath, destinationPath);
}

QString FileOperations::uniqueDestinationPath(const QString &destDir, const QString &sourcePath)
{
    const QFileInfo sourceInfo(sourcePath);
    const QDir dir(destDir);

    const QString baseName = sourceInfo.completeBaseName();
    const QString suffix = sourceInfo.suffix();
    const QString fileName = sourceInfo.fileName();

    QString candidate = fileName;
    int counter = 1;

    while (dir.exists(candidate))
    {
        ++counter;
        candidate = (sourceInfo.isDir() || suffix.isEmpty())
            ? QString("%1 (%2)").arg(fileName).arg(counter)
            : QString("%1 (%2).%3").arg(baseName).arg(counter).arg(suffix);
    }

    return dir.filePath(candidate);
}
