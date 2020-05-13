#include "query_type_input.h"
#include <QVariant>
#include "input_query.h"
#include <iostream>
#include "rating.h"
#include "find_query.h"

QueryTypeInput::QueryTypeInput (QWidget *parent)
{
    this->setWindowTitle("Query Type");
    int height = 150;
    int width = 360;
    this->setGeometry(parent->x() + parent->width()/2 - width/2,
                    parent->y() + parent->height()/2 - height/2,
                    width, height);
//    this->setFixedSize(QSize(width, height));
    layout = new QHBoxLayout(this);

    findButton = new QPushButton("find");
    updateButton = new QPushButton("update");
    insertButton = new QPushButton("insert");

    findButton->setProperty("ActionRole", QVariant(Find));
    updateButton->setProperty("ActionRole", QVariant(Update));
    insertButton->setProperty("ActionRole", QVariant(Insert));

    findButton->setAutoDefault(false);
    updateButton->setAutoDefault(false);
    insertButton->setAutoDefault(false);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(updateButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(insertButton, QDialogButtonBox::ActionRole);

    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton *)));
    layout->addWidget(buttonBox);
    this->setLayout(layout);
}

void QueryTypeInput::buttonClicked(QAbstractButton *button)
{
    int actionRole = button->property("ActionRole").toInt();
    InputQueryDialog inputQuery;
    Rating rating;
    QPair<int, Rating> pair;
    switch(actionRole)
    {
        case Find:
        {
            FindQueryDialog inputQuery;
            inputQuery.setModal(true);
            inputQuery.setWindowFlags(Qt::FramelessWindowHint);
            rating = FindQueryDialog::getFields(this);
            pair.first = Find;
            pair.second = rating;
            emit queryCreated(pair);
            accept();
            break;
        }
        case Update:
        {
            InputQueryDialog inputQuery;
            inputQuery.setModal(true);
            inputQuery.setWindowFlags(Qt::FramelessWindowHint);
            rating = InputQueryDialog::getFields(this);
            pair.first = Update;
            pair.second = rating;
            emit queryCreated(pair);
            accept();
            break;
        }
        case Insert:
        {
            inputQuery.setModal(true);
            inputQuery.setWindowFlags(Qt::FramelessWindowHint);
            rating = InputQueryDialog::getFields(this);
            pair.first = Insert;
            pair.second = rating;
            emit queryCreated(pair);
            accept();
            break;
        }
        default:
            std::cout << "Default???" << std::endl;
            break;
    }
}
