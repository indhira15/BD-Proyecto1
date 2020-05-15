#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <boost/functional/hash.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include "Rating.h"
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

    HashIndex(std::string file_name)
    {
        std::size_t n_r = 0;
        {
            std::ifstream ifs(file_name);
            ifs.unsetf(ifs.skipws);
            n_r = std::count(
                std::istream_iterator<char>(ifs),
                std::istream_iterator<char>(), 
                '\n'
            );
        }
        B = (--n_r / f_r) * ( 1 + d );
        init();
        {
            std::ifstream ifs(file_name);
            ifs.setf(ifs.skipws);
            std::fstream fs("data_hash.dat", fs.binary|fs.in|fs.out);
            std::string line;
            std::vector<std::string> v;
            std::getline(ifs, line);
            int cnt = 1;
            ofstream outFile;
            outFile.open("hash_timeResults.csv");
            while(std::getline(ifs, line))
            {
                line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
                boost::tokenizer<boost::escaped_list_separator<char> > tokens(line);
                v.assign(tokens.begin(), tokens.end());
                if(v.size() < 4)
                    continue;
                Rating r = {boost::lexical_cast<uint64_t>(v[0]),
                            boost::lexical_cast<uint64_t>(v[1]),
                            (int)(std::stod(v[2])*10)/10.0,
                            boost::lexical_cast<uint64_t>(v[3]) };
                if(cnt == 10 || cnt == 100 || cnt == 1000 || cnt == 10000 || cnt == 100000 || cnt == 1000000){
                  auto begin = chrono::high_resolution_clock::now();
                  insert(r,fs);
                  auto end = chrono::high_resolution_clock::now();

                  outFile << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << ",";

                  begin = chrono::high_resolution_clock::now();    
                  find(r.userId, r.movieId);
                  end = chrono::high_resolution_clock::now();

                  outFile << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << endl;
                }else{insert(r, fs);}
                cnt++;
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

            while(b.next != 0 && b.filled == F_R_)
            {
                fs.seekg(b.next);
                b_pos = fs.tellg();
                fs.read((char *)&b, sizeof(b));
            }
            if(b.filled < F_R_)
            {
                std::size_t s = sizeof(b);
                b.ratings[b.filled++] = r;
                fs.seekp(b_pos);
                fs.write((char *)&b, sizeof(b));
                fs.flush();
            }
            else
            {
                {
                    fs.seekp(0, std::ios_base::end);
                    Bucket new_b{};
                    new_b.ratings[new_b.filled++] = r;
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
                std::cout << b.ratings[b.filled - 1].timestamp << std::endl;
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
        Rating r{userId, movieId};
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