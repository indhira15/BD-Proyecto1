#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <rating.h>
#include "hash_index.h"
#include "randomFile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    RatingModel model_;
    QSortFilterProxyModel proxy_;
    HashIndex hash_index;
    RandomFile random_file;
    enum Query {
        Find = 1,
        Update = 2,
        Insert = 3
    };
    enum Index {
        NoIndex = 0,
        Hash = 2,
        Random = 3
    };
    int index_selected = NoIndex;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newQuery_clicked();

    void on_hashFile_clicked();

    void on_randomFile_clicked();

    void executeQuery(QPair<int, Rating>);

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
