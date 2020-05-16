#ifndef RATING_H
#define RATING_H

struct Rating
{
    uint64_t userId;
    uint64_t movieId;
    double rating;
    uint64_t timestamp;
    Rating() = default;

    bool
    operator==(const Rating& rhs)
    {
        return userId == rhs.userId && movieId == rhs.movieId;
    }
};

#endif /* RATING_H */