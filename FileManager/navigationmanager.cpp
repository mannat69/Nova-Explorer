#include "navigationmanager.h"

#include <QDir>
#include <QFileInfo>

NavigationManager::NavigationManager(QObject *parent)
    : QObject(parent)
{
}

QString NavigationManager::currentPath() const
{
    return m_currentPath;
}

bool NavigationManager::canGoBack() const
{
    return !m_backStack.isEmpty();
}

bool NavigationManager::canGoForward() const
{
    return !m_forwardStack.isEmpty();
}

void NavigationManager::navigateTo(const QString &path)
{
    const QFileInfo info(path);
    if (!info.exists() || !info.isDir())
    {
        emit navigationFailed(path);
        return;
    }

    const QString cleanPath = QDir(path).absolutePath();

    if (cleanPath == m_currentPath)
        return;

    if (!m_currentPath.isEmpty())
        m_backStack.push_back(m_currentPath);

    m_forwardStack.clear();

    setCurrentPath(cleanPath);
}

void NavigationManager::goBack()
{
    if (m_backStack.isEmpty())
        return;

    m_forwardStack.push_back(m_currentPath);
    const QString previousPath = m_backStack.takeLast();
    setCurrentPath(previousPath);
}

void NavigationManager::goForward()
{
    if (m_forwardStack.isEmpty())
        return;

    m_backStack.push_back(m_currentPath);
    const QString nextPath = m_forwardStack.takeLast();
    setCurrentPath(nextPath);
}

void NavigationManager::goUp()
{
    QDir dir(m_currentPath);
    if (dir.cdUp())
        navigateTo(dir.absolutePath());
}

void NavigationManager::refresh()
{
    emit currentPathChanged(m_currentPath);
}

void NavigationManager::setCurrentPath(const QString &path)
{
    m_currentPath = path;
    emit currentPathChanged(m_currentPath);
    emitNavigationStateChanged();
}

void NavigationManager::emitNavigationStateChanged()
{
    emit canGoBackChanged(canGoBack());
    emit canGoForwardChanged(canGoForward());
}
