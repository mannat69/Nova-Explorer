#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class PropertiesDialog;
}
QT_END_NAMESPACE

class PropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PropertiesDialog(const QString &path,
                              QWidget *parent = nullptr);

    ~PropertiesDialog();

private:
    Ui::PropertiesDialog *ui;
};

#endif