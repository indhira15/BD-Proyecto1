#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include<hash_index.h>
#include <iostream>
#include <randomFile.h>
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
    // abro el csv
    // creo el index
    hash_index = HashIndex(ratings_csv);
    // indico al programa que el index seleccionado es el hash
    index_selected = Hash;
    // activo el botón que dice "New Query"
    ui->newQuery->setEnabled(true);
}

void MainWindow::on_randomFile_clicked()
{
    QString ratings_csv = ":/static/ratings.csv";
    random_file = RandomFile(ratings_csv);
    index_selected = Random;
    ui->newQuery->setEnabled(true);
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
    // borro los resultados de la query anterior
    model_.clear();
    switch (query) {
        case Update:
        {
            if(index_selected == Hash)
            {
                hash_index.update(pair.second);
            }
            else{
                random_file.update(pair.second);
            }
            break;
        }
        case Find:
        {
            if(index_selected == Hash)
            {
                rating = hash_index.find(pair.second.userId, pair.second.movieId);
                // agregar al modelo para que salga en la tabla
                model_.append(rating);
            }
            else
            {
                rating = random_file.search_openIndex(pair.second.userId, pair.second.movieId);
                //rating = random_file.search_fixedIndex(pair.second.userId, pair.second.movieId);
                model_.append(rating);
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
                random_file.insert(pair.second);
            }
            break;
        }
        default:
            break;
    }
}
