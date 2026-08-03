#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QObject>
#include <QStringList>

// Performs actual filesystem work (copy, move, delete, rename, mkdir) and
// owns a small internal clipboard used for copy/cut + paste. Emits status
// signals so the UI layer can surface success/failure without needing to
// know the mechanics of any given operation.
class FileOperations : public QObject
{
    Q_OBJECT

public:
    enum class ClipboardMode { None, Copy, Cut };

    explicit FileOperations(QObject *parent = nullptr);

    void copyToClipboard(const QStringList &paths);
    void cutToClipboard(const QStringList &paths);
    bool hasClipboardContent() const;
    ClipboardMode clipboardMode() const;

public slots:
    bool pasteTo(const QString &destDir);
    bool deletePaths(const QStringList &paths);
    bool renamePath(const QString &path, const QString &newName);
    QString createNewFolder(const QString &parentDir,
                             const QString &baseName = QStringLiteral("New Folder"));

signals:
    void operationFailed(const QString &message);
    void operationSucceeded(const QString &message);
    void clipboardChanged(bool hasContent);

private:
    static bool copyRecursively(const QString &sourcePath, const QString &destinationPath);
    static QString uniqueDestinationPath(const QString &destDir, const QString &sourcePath);

    QStringList m_clipboardPaths;
    ClipboardMode m_clipboardMode = ClipboardMode::None;
};

#endif // FILEOPERATIONS_H
