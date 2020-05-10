#include <iostream>
#include <sstream>
#include <boost/tokenizer.hpp>

#include "Bucket.h"
#include "Rating.h"
#include "HashIndex.h"

int
main(int argc, char* argv[])
{

/*
    // Testing HashIndex
    Rating r = { 1,307,3.5,1256677221 };
    HashIndex index = HashIndex(27000000);
    std::cout << index.h(r) << std::endl;
*/
    

    /*
    // Testing serialization
    std::ostringstream oss;
    {
        Rating r1{ 1, 307, 3.5, 1256677221 }, r2{ 2, 308, 3.7, 2256677221 };
        Bucket b{ 0, 2, {r1, r2} };
        boost::archive::text_oarchive oa(oss);
        oa << b;
    }

    Bucket cloned;
    std::istringstream iss(oss.str());
    {
        boost::archive::text_iarchive ia(iss);
        ia >> cloned;
    }

    {
        std::ostringstream oss2;

        boost::archive::text_oarchive oa(oss2);
        oa << cloned;

        std::cout << oss.str()  << std::endl;
        std::cout << oss2.str() << std::endl;
    }
    */

/*
    // Testing HashIndex init
    HashIndex index = HashIndex(270);
    index.check_init();
*/

/*
   // Testing HashIndex insert
   HashIndex index = HashIndex(270);
   Rating r1{ 1, 307, 3.5, 1256677221 };
   index.insert(r1);
    index.check_insert(r1);


    // Testing HashIndex update
    Rating r2{ 1, 307, 5, 2256677221 };
    index.update(r2);
    index.check_insert(r1);
*/

    // Testing init from csv
    HashIndex index = HashIndex("static/ratings.csv"); 

    return 0;
}