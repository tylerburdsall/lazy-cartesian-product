/* lazy-cartesian-product.hpp
 * (c) Tyler Burdsall - 2018
 * 
 * Licensed under the MIT license
 */


#ifndef _LAZY_CARTESIAN_PRODUCT
#define _LAZY_CARTESIAN_PRODUCT

#define LCP_MAJOR_VERSION 1
#define LCP_MINOR_VERSION 6

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0
#endif

#include <string>
#include <fstream>
#include <stdexcept>
#include <cmath>
#ifdef USE_BOOST
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/container/vector.hpp>
using namespace boost::random;
using namespace boost::multiprecision;
using boost::container::vector;
using boost::random::random_device;
#else
#include <random>
#include <vector>
using std::mt19937_64;
using std::uniform_int_distribution;
using std::random_device;
using std::vector;
#endif

using std::fstream;
using std::ios;
using std::string;
using std::out_of_range;
using std::runtime_error;
using std::floor;

namespace lazycp
{
    namespace errors
    {
        struct index_error: public runtime_error
        {
            index_error(): runtime_error("The given index cannot be out of range") {}
        };
        struct empty_list_error: public runtime_error
        {
            empty_list_error(): runtime_error("The given list of combinations cannot be empty") {}
        };
        struct empty_answers_error: public runtime_error
        {
            empty_answers_error(): runtime_error("The given list of answers cannot be empty") {}
        };
        struct invalid_sample_size_error: public runtime_error
        {
            invalid_sample_size_error(): runtime_error("The given sample size cannot be out of range") {}
        };
    }
}

namespace lazycp
{
#ifdef USE_BOOST	
    struct precomputed_stats
    {
        vector<uint1024_t> divs;
        vector<uint1024_t> mods;
        uint1024_t max_size;
    };
#else
    struct precomputed_stats
    {
        vector<unsigned long long> divs;
        vector<unsigned long long> mods;
        unsigned long long max_size;
    };
#endif
    class RandomIterator
    {
        public:
#ifdef USE_BOOST
            RandomIterator(const string &amount, const string &max): gen((random_device())())
            {
                num_left.assign(amount);
                last_k.assign(0);
                n.assign(max);
            }

            RandomIterator(const uint1024_t &amount, const uint1024_t &max): gen((random_device())())
            {
                num_left.assign(amount);
                last_k.assign(0);
                n.assign(max);
            }
            const uint1024_t next(void)
            {
                if (num_left > 0)
                {
                    uint1024_t range_size((n - last_k) / num_left);
                    uniform_int_distribution<uint1024_t> rnd(0, range_size);
                    uint1024_t r(rnd(gen) + last_k + 1);
                    last_k.assign(r);
                    num_left--;
                    return r;
                }
                else
                {
                    throw out_of_range("Exceeded amount of random numbers to generate.");
                }
            }

#else
            RandomIterator(const string &amount, const string &max): gen((random_device())())
            {
                num_left = stoull(amount, 0, 10);
                last_k = 0;
                n = stoull(max, 0, 0);
            }

            RandomIterator(const unsigned long long &amount, const unsigned long long &max): gen((random_device())())
            {
                num_left = amount;
                last_k = 0;
                n = max;
            }
            const unsigned long long next(void)
            {
                if (num_left > 0)
                {
                    unsigned long long range_size = (n - last_k) / num_left;
                    uniform_int_distribution<> rnd(0, range_size);
                    unsigned long long r = rnd(gen) + last_k + 1;
                    last_k = r;
                    num_left--;
                    return r;
                }
                else
                {
                    throw out_of_range("Exceeded amount of random numbers to generate.");
                }
            }
#endif
            const bool has_next(void)
            {
                return num_left > 0;
            }

        private:
#ifdef USE_BOOST
            uint1024_t         n;
            uint1024_t         last_k;
            uint1024_t         num_left;
#else
            unsigned long long n;
            unsigned long long last_k;
            unsigned long long num_left;
#endif
            mt19937_64         gen;
    };

    class lazy_cartesian_product
    {
        public:
#ifdef USE_BOOST
            static const vector<string> boost_entry_at(const vector<vector<string>> &combinations, const string &index)
            {
                const precomputed_stats pc = boost_precompute(combinations);
                uint1024_t parsed_index(index);
                if (parsed_index  < 0 || parsed_index >= pc.max_size)
                {
                    throw errors::index_error();
                }

                const vector<string> combination = boost_entry_at(combinations, parsed_index, pc);
                return combination;
            }
            static const vector<vector<string>> boost_generate_samples(const vector<vector<string>> combinations, const string &sample_size)
            {
                const uint1024_t parsed_sample_size(sample_size);
                if (combinations.size() == 0)
                {
                    throw errors::empty_list_error();
                }
                precomputed_stats ps = boost_precompute(combinations);

                vector<vector<string>> subset;
                if (parsed_sample_size != ps.max_size)
                {
                    RandomIterator iter(parsed_sample_size, ps.max_size);
                    while(iter.has_next())
                    {
                        subset.push_back(boost_entry_at(combinations, iter.next(), ps));
                    }
                }
                else
                {
                    for (uint1024_t i = 0; i < parsed_sample_size; ++i)
                    {
                        subset.push_back(boost_entry_at(combinations, i, ps));
                    }
                }

                return subset;
            }
            static const uint1024_t boost_compute_max_size(const vector<vector<string>> &combinations)
            {
                uint1024_t size(1);
                for (vector<vector<string>>::const_iterator it = combinations.begin(); it != combinations.end(); ++it)
                {
                    size *= (*it).size();
                }

                return size;
            }
            
#else
            static const vector<string> entry_at(const vector<vector<string>> &combinations, const unsigned long long &index)
            {
                const precomputed_stats pc = precompute(combinations);
                if (index  < 0 || index >= pc.max_size)
                {
                    throw errors::index_error();
                }

                const vector<string> combination = entry_at(combinations, index, pc);
                return combination;
            }
            static const vector<vector<string>> generate_samples(const vector<vector<string>> combinations, const unsigned long long &sample_size)
            {
                if (combinations.size() == 0)
                {
                    throw errors::empty_list_error();
                }
                precomputed_stats ps = precompute(combinations);

                vector<vector<string>> subset;
                if (sample_size != ps.max_size)
                {
		    subset.reserve(sample_size);
                    RandomIterator iter(sample_size, ps.max_size);
                    while(iter.has_next())
                    {
                        subset.push_back(entry_at(combinations, iter.next(), ps));
                    }
                }
                else
                {
		    subset.reserve(sample_size);
                    for (unsigned long long i = 0; i < sample_size; ++i)
                    {
                        subset.push_back(entry_at(combinations, i, ps));
                    }
                }

                return subset;
            }
            static const unsigned long long compute_max_size(const vector<vector<string>> &combinations)
            {
                unsigned long long size = 1;
                for (vector<vector<string>>::const_iterator it = combinations.begin(); it != combinations.end(); ++it)
                {
                    size *= (*it).size();
                }

                return size;
            }
#endif
        private:
#ifdef USE_BOOST
            static const precomputed_stats boost_precompute(const vector<vector<string>> &combinations)
            {
                precomputed_stats ps;
                if (combinations.size() == 0)
                {
                    throw errors::empty_answers_error();
                }

                long long size = combinations.size();
                ps.divs.resize(size);
                ps.mods.resize(size);
                uint1024_t factor = 1;

                for (long long i = size - 1; i >= 0; --i)
                {
                    uint1024_t items(combinations[i].size());
                    ps.divs[i] = factor;
                    ps.mods[i] = items;
                    factor *= items;
                }

                ps.max_size = boost_compute_max_size(combinations);
                return ps;
            }
            static const bool boost_sample_size_valid(const uint1024_t &sample, const uint1024_t &max_size)
            {
                return sample <= max_size;
            }

            static const vector<string> boost_entry_at(const vector<vector<string>> &combinations, const uint1024_t &n, const precomputed_stats &ps)
            {
                unsigned long long length(combinations.size());
                vector<string> combination(length);

                for (unsigned long long i = 0; i < length; ++i)
                {
                    combination[i] = combinations[i][(unsigned long long)((uint1024_t)(n / ps.divs[i]) % ps.mods[i])];
                }

                return combination;
            }
#else
            static const precomputed_stats precompute(const vector<vector<string>> &combinations)
            {
                precomputed_stats ps;
                if (combinations.size() == 0)
                {
                    throw errors::empty_answers_error();
                }

                unsigned long long size = combinations.size();
                ps.divs.resize(size);
                ps.mods.resize(size);
                unsigned long factor = 1;

                for (long long i = size - 1; i >= 0; --i)
                {
                    unsigned long long items = combinations[i].size();
                    ps.divs[i] = factor;
                    ps.mods[i] = items;
                    factor *= items;
                }

                ps.max_size = compute_max_size(combinations);
                return ps;
            }
            static const bool sample_size_valid(const unsigned long long &sample, const unsigned long &max_size)
            {
                return sample <= max_size;
            }
            static const vector<string> entry_at(const vector<vector<string>> &combinations, const unsigned long long &n, const precomputed_stats &ps)
            {
                unsigned long long length = combinations.size();
                vector<string> combination(length);

                for (unsigned long long i = 0; i < length; ++i)
                {
                    combination[i] = combinations[i][(unsigned long long)(floor(n / ps.divs[i])) % ps.mods[i]];
                }

                return combination;
            }
#endif
            lazy_cartesian_product() {}
    };
}
#endif
