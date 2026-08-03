#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include <QSortFilterProxyModel>

class SearchManager : public QObject
{
    Q_OBJECT

public:
    explicit SearchManager(QObject *parent = nullptr);

    void setSourceModel(QAbstractItemModel *model);

    QSortFilterProxyModel *proxyModel() const;

public slots:
    void setFilterText(const QString &text);

private:
    QSortFilterProxyModel *m_proxy;
};

#endif