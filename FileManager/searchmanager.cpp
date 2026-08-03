#include "searchmanager.h"

SearchManager::SearchManager(QObject *parent)
    : QObject(parent)
{
    m_proxy = new QSortFilterProxyModel(this);

    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterKeyColumn(0);
}

void SearchManager::setSourceModel(QAbstractItemModel *model)
{
    m_proxy->setSourceModel(model);
}

QSortFilterProxyModel *SearchManager::proxyModel() const
{
    return m_proxy;
}

void SearchManager::setFilterText(const QString &text)
{
    m_proxy->setFilterFixedString(text);
}