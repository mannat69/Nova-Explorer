#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include <QObject>
#include <QStringList>

class NavigationManager : public QObject
{
    Q_OBJECT

public:
    explicit NavigationManager(QObject *parent = nullptr);

    QString currentPath() const;
    bool canGoBack() const;
    bool canGoForward() const;

public slots:
    // Navigates to a new directory. Pushes the previous location onto the
    // back stack and clears the forward stack (standard browser-style nav).
    void navigateTo(const QString &path);
    void goBack();
    void goForward();
    void goUp();
    void refresh();

signals:
    void currentPathChanged(const QString &path);
    void canGoBackChanged(bool canGoBack);
    void canGoForwardChanged(bool canGoForward);
    void navigationFailed(const QString &path);

private:
    void setCurrentPath(const QString &path);
    void emitNavigationStateChanged();

    QStringList m_backStack;
    QStringList m_forwardStack;
    QString m_currentPath;
};

#endif // NAVIGATIONMANAGER_H
