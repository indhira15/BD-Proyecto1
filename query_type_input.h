#ifndef QUERY_TYPE_INPUT_H
#define QUERY_TYPE_INPUT_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>

class QLineEdit;
class QLabel;
class Rating;

class QueryTypeInput : public QDialog
{
    Q_OBJECT
private:
    enum Role {
        Find = 1,
        Update = 2,
        Insert = 3
    };
    QPushButton *findButton, *updateButton, *insertButton;
    QDialogButtonBox *buttonBox;
    QHBoxLayout *layout;

private slots:
    void buttonClicked(QAbstractButton *button);
public:
    QueryTypeInput(QWidget *parent = nullptr);

signals:
    void queryCreated(QPair<int, Rating>);
};


#endif // QUERY_TYPE_INPUT_H
