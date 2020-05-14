#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <boost/functional/hash.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "rating.h"
#include "bucket.h"
#include "sizes.h"

namespace std
{
    template<>
    struct hash<Rating>
    {
        std::size_t
        operator()(Rating const& r)
        const noexcept
        {
            std::size_t h(0), h1, h2;
            h1 = std::hash<uint64_t>{}(r.userId);
            h2 = std::hash<uint64_t>{}(r.movieId);
            boost::hash_combine(h, h1);
            boost::hash_combine(h, h2);
            return h;
        }
    };
}

class HashIndex
{
private:
    std::size_t B;
    static std::size_t constexpr f_r = F_R_;
    static constexpr float d = D_;

    void
    init()
    {
        std::ofstream ofs("data_hash.dat", ofs.binary|ofs.out|ofs.trunc);
        for(std::size_t i = 0; i <= B; ++i)
        {
            Bucket b{};
            ofs.write((char *)&b, sizeof(b));
            ofs.flush();
        }
    }

public:
    HashIndex():
    B(10)
    {
        init();
    }

    HashIndex(std::size_t n_r):
    B((n_r / f_r) * ( 1 + d ))
    {
        init();
    }

    HashIndex(QString file_name)
    {
        std::size_t n_r = 0;
        {
            // usar qfile y qtextstream para abrir el csv
            QFile ifile(file_name);
            ifile.open(QIODevice::ReadOnly);
            QTextStream ifs(&ifile);
            while(!ifs.atEnd())
            {
                ifs.readLine();
                n_r++;
            }
        }
        B = (--n_r / f_r) * ( 1 + d );
        init();
        {
            QFile ifile(file_name);
            ifile.open(QIODevice::ReadOnly);
            QTextStream ifs(&ifile);
            std::fstream fs("data_hash.dat", fs.binary|fs.in|fs.out);
            QString line;
            ifs.readLine();
            while(!ifs.atEnd())
            {
                line = ifs.readLine();
                line.remove('\r');
                QStringList v = line.split(QRegExp("[,\t\r ]"), QString::SkipEmptyParts);
                if(v.size() < 4)
                    continue;
                Rating r = {v[0].toULong(),
                            v[1].toULong(),
                            (int)(v[2].toDouble()*10)/10.0,
                            v[3].toULong() };
                insert(r, fs);
            }
        }
    }

    std::size_t
    h(Rating const& r)
    {
        return std::hash<Rating>{}(r) % B;
    }

    void
    check_init()
    {
        std::ifstream ifs("data_hash.dat", ifs.binary);
        Bucket b{};
        for(std::size_t i = 0; i <= B; ++i)
        {
            ifs.read((char *)&b, sizeof(b));
            if(b.filled)
            {
                std::cout << b.filled << " " << ifs.tellg() << std::endl;
                return;
            }
        }
    }

    void
    insert(Rating r)
    {
        std::fstream fs("data_hash.dat", fs.binary|fs.in|fs.out);
        return insert(r, fs);
    }

    void
    insert(Rating r, std::fstream& fs)
    {
        Bucket b{};
        long long b_pos = 0;
        long long offset = h(r) * sizeof(b);
        {
            if(!fs.is_open())
                fs.open("data_hash.dat", fs.binary|fs.in|fs.out);
            {
                fs.read((char *)&b, sizeof(b));
                offset += fs.tellg();
            }
            fs.seekg(offset);
            {
                b_pos = offset;
                fs.read((char *)&b, sizeof(b));
            }
            // saltear los buckets llenos
            while(b.next != 0 && b.filled == F_R_)
            {
                fs.seekg(b.next);
                b_pos = fs.tellg();
                fs.read((char *)&b, sizeof(b));
            }
            if(b.filled < F_R_)
            {
                // caso en el que hay un bucket no lleno
                std::size_t s = sizeof(b);
                b.ratings[b.filled++] = r;
                fs.seekp(b_pos);
                fs.write((char *)&b, sizeof(b));
                fs.flush();
            }
            else
            {
                // caso en el que todos los buckets del hash están llenos
                {
                    // voy al final del archivo
                    fs.seekp(0, std::ios_base::end);
                    Bucket new_b{};
                    // agrega el rating en la pos 0
                    // e incrementa el numero filled
                    new_b.ratings[new_b.filled++] = r;
                    // el next del bucket b apunta al final del archivo
                    // porque ahí voy a crear el nuevo bucket
                    b.next = fs.tellp();
                    fs.write((char *)&new_b, sizeof(new_b));
                    fs.flush();
                }
                {
                    fs.seekp(b_pos);
                    fs.write((char *)&b, sizeof(b));
                    fs.flush();
                }
            }
        }
    }

    void
    check_insert(Rating r)
    {
        Bucket b{};
        long long offset = h(r) * sizeof(b);
        {
            std::ifstream ifs("data_hash.dat", ifs.binary);
            ifs.read((char *)&b, sizeof(b));
            offset += ifs.tellg();
            ifs.seekg(offset);
            ifs.read((char *)&b, sizeof(b));
            for(int i = 0; i < b.filled; ++i)
            {
                std::cout << b.ratings[b.filled - 1].userId << std::endl;
                std::cout << b.ratings[b.filled - 1].movieId << std::endl;
                std::cout << b.ratings[b.filled - 1].rating << std::endl;
            }
        }
    }

    void
    update(Rating r)
    {
        Bucket b{};
        long long b_pos = 0;
        long long offset = h(r) * sizeof(b);
        {
            std::fstream fs("data_hash.dat", fs.binary|fs.in|fs.out);
            fs.read((char *)&b, sizeof(b));
            offset += fs.tellg();
            fs.seekg(offset);
            {
                b_pos = offset;
                fs.read((char *)&b, sizeof(b));
            }
            while(true)
            {
                for(std::size_t i = 0; i < F_R_; ++i)
                {
                    if(b.ratings[i] == r)
                    {
                        b.ratings[i] = r;
                        fs.seekp(b_pos);
                        fs.write((char *)&b, sizeof(b));
                        fs.flush();
                        return;
                    }
                }
                if(b.next == 0)
                    return;
                fs.seekg(b.next);
                b_pos = fs.tellg();
                fs.read((char *)&b, sizeof(b));
            }
        }
    }

    Rating
    find(uint64_t userId, uint64_t movieId)
    {
        Bucket b{};
        Rating r{userId, movieId, 0, 0};
        long long offset = (h(r) + 1) * sizeof(b);
        long long b_pos = 0;
        {
            std::ifstream ifs("data_hash.dat", ifs.binary);
            b_pos = offset;
            ifs.seekg(b_pos);
            ifs.read((char *)&b, sizeof(b));
            while(true)
            {
                for(std::size_t i = 0; i < F_R_; ++i)
                {
                    if(r == b.ratings[i])
                        return b.ratings[i];
                }
                if(b.next != 0)
                {
                    ifs.seekg(b.next);
                    ifs.read((char *)&b, sizeof(b));
                }
                else
                {
                    return Rating{};
                }

            }
        }
    }
};

#endif /* HASH_INDEX_H */
