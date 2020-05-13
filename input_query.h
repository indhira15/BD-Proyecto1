#ifndef INPUT_QUERY_H
#define INPUT_QUERY_H

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "rating.h"


class InputQueryDialog : public QDialog
{
    Q_OBJECT
private:
    QList<QLineEdit*> fields;
public:
    explicit InputQueryDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        QStringList labels = {"userId", "movieId", "rating", "timestamp"};
        QFormLayout *layout = new QFormLayout(this);
        for(int i = 0; i < 4; ++i)
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
        connect(buttonBox, &QDialogButtonBox::accepted, this, &InputQueryDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &InputQueryDialog::reject);
        setLayout(layout);
    }

    static Rating getFields(QWidget *parent)
    {
        InputQueryDialog *dialog = new InputQueryDialog(parent);
        Rating rating;
        const int ret = dialog->exec();
        if(ret)
        {
            rating = Rating{
                    dialog->fields[0]->text().toULong(),
                    dialog->fields[1]->text().toULong(),
                    dialog->fields[2]->text().toDouble(),
                    dialog->fields[3]->text().toULong()
            };
        }

        dialog->deleteLater();

        return rating;
    }
};

#endif // INPUT_QUERY_H
