#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include<hash_index.h>
#include <iostream>
#include "query_type_input.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Q_INIT_RESOURCE(res);
    ui->setupUi(this);
    ui->query->setText("");
    ui->newQuery->setEnabled(false);
    proxy_.setSourceModel(&model_);
    proxy_.setFilterKeyColumn(2);
    ui->tableView->setModel(&proxy_);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_hashFile_clicked()
{
    QString ratings_csv = ":/static/ratings.csv";
    hash_index = HashIndex(ratings_csv);
    index_selected = Hash;
    ui->newQuery->setEnabled(true);
}

void MainWindow::on_randomFile_clicked()
{
    index_selected = Random;
}


void MainWindow::on_newQuery_clicked()
{
    QueryTypeInput queryType(this);
    connect(&queryType, &QueryTypeInput::queryCreated, this, &MainWindow::executeQuery);
    queryType.exec();
}

void MainWindow::executeQuery(QPair<int, Rating> pair)
{
    int query = pair.first;
    Rating rating;
    model_.clear();
    switch (query) {
        case Update:
        {
            if(index_selected == Hash)
            {
                hash_index.update(pair.second);
            }
            else{
                // update de random file
            }
            break;
        }
        case Find:
        {
            if(index_selected == Hash)
            {
                rating = hash_index.find(pair.second.userId, pair.second.movieId);
                model_.append(rating);
            }
            else
            {
                // find de random file
            }
            break;
        }
        case Insert:
        {
            if(index_selected == Hash)
            {
                hash_index.insert(pair.second);
            }
            else
            {
                // insert de random file
            }
            break;
        }
        default:
            break;
    }
}
