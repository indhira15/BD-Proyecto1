#ifndef RATING_H
#define RATING_H

struct Rating
{
    unsigned long long userId;
    unsigned long long movieId;
    double rating;
    unsigned long long timestamp;
    Rating() = default;

    bool
    operator==(const Rating& rhs)
    {
        return userId == rhs.userId && movieId == rhs.movieId;
    }

};

#endif /* RATING_H */