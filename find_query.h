#ifndef FIND_QUERY_H
#define FIND_QUERY_H

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "rating.h"


class FindQueryDialog : public QDialog
{
    Q_OBJECT
private:
    QList<QLineEdit*> fields;
public:
    explicit FindQueryDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        QStringList labels = {"userId", "movieId"};
        QFormLayout *layout = new QFormLayout(this);
        for(int i = 0; i < 2; ++i)
        {
            QLabel *label = new QLabel(labels[i], this);
            QLineEdit *lineEdit = new QLineEdit(this);
            layout->addRow(label, lineEdit);
            fields << lineEdit;
        }
        QDialogButtonBox *buttonBox = new QDialogButtonBox(
                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                    Qt::Horizontal, this);
        layout->addRow(buttonBox);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &FindQueryDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &FindQueryDialog::reject);
        setLayout(layout);
    }

    static Rating getFields(QWidget *parent)
    {
        FindQueryDialog *dialog = new FindQueryDialog(parent);
        Rating rating;
        const int ret = dialog->exec();
        if(ret)
        {
            rating = Rating{
                    dialog->fields[0]->text().toULong(),
                    dialog->fields[1]->text().toULong(),
                    0,
                    0
            };
        }

        dialog->deleteLater();

        return rating;
    }
};

#endif // FIND_QUERY_H
