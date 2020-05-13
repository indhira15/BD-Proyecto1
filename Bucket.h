#ifndef BUCKET_H
#define BUCKET_H

#include <vector>
#include <boost/serialization/access.hpp>
#include "rating.h"
#include "sizes.h"

struct Bucket
{
    long long next;
    std::size_t filled;
    Rating ratings[F_R_] = {};
    static constexpr std::size_t size = F_R_;
    Bucket() = default;
};

#endif /* BUCKET_H */
