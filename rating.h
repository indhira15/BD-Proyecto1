#ifndef RATING_H
#define RATING_H
#include <cstdint>
#include <QDateTime>
#include <QVariant>
#include <QModelIndex>
#include <QSortFilterProxyModel>

struct Rating {
  unsigned long long userId, movieId;
  double rating;
  QDateTime timestamp;
  Rating() = default;

  Rating(const unsigned long long& userId, const unsigned long long& movieId, const double& rating, const unsigned long long& unixtime):
      userId(userId), movieId(movieId), rating(rating)
  {
      timestamp.setTime_t(unixtime);
  }

  bool
  operator==(const Rating& rhs)
  {
      return userId == rhs.userId && movieId == rhs.movieId;
  }
};

class RatingModel : public QAbstractTableModel
{
    QList<Rating> ratings_;
public:
    RatingModel(QObject *parent = {}) {}
    int rowCount(const QModelIndex &) const override { return ratings_.count(); }
    int columnCount(const QModelIndex &) const override { return 4; }

    QVariant data(const QModelIndex &index, int role) const override {
        if(role != Qt::DisplayRole && role != Qt::EditRole) return {};
        const auto &rating = ratings_[index.row()];
        switch (index.column()) {
            case 0: return rating.userId;
            case 1: return rating.movieId;
            case 2: return rating.rating;
            case 3: return rating.timestamp;
            default: return {};
        }
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if(orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
        switch (section) {
        case 0: return "userId";
        case 1: return "movieId";
        case 2: return "rating";
        case 3: return "timestamp";
        default: return {};
        }
    }

    void append(const Rating &rating) {
        beginInsertRows({}, ratings_.count(), ratings_.count());
        ratings_.append(rating);
        endInsertRows();
    }

    void clear()
    {
        ratings_.clear();
    }
};

#endif /* RATING_H */
