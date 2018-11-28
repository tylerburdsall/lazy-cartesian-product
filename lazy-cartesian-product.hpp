/* lazy-cartesian-product.hpp
 * (c) Tyler Burdsall - 2018
 * 
 * Licensed under the MIT license
 */


#ifndef _LAZY_CARTESIAN_PRODUCT
#define _LAZY_CARTESIAN_PRODUCT

#define LCP_MAJOR_VERSION 1
#define LCP_MINOR_VERSION 3

#include <string>
#include <fstream>
#include <stdexcept>
#include <cmath>
#ifdef USE_BOOST
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/set.hpp>
using namespace boost::random;
using namespace boost::multiprecision;
using boost::container::vector;
using boost::container::set;
using boost::random::random_device;
#else
#include <random>
#include <vector>
#include <set>
using std::mt19937_64;
using std::uniform_int_distribution;
using std::random_device;
using std::vector;
using std::set;
#endif

using std::fstream;
using std::ios;
using std::string;
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
		static const vector<vector<string>> boost_generate_samples(const vector<vector<string>> combinations, const uint1024_t &sample_size)
		{
			if (combinations.size() == 0)
			{
				throw errors::empty_list_error();
			}
			precomputed_stats ps = boost_precompute(combinations);

			vector<vector<string>> subset;
			if (sample_size != ps.max_size)
			{
				vector<uint1024_t> sampled_indicies = boost_generate_random_indices(sample_size, ps.max_size);
				for (const uint1024_t &i : sampled_indicies)
				{
					subset.push_back(boost_entry_at(combinations, i, ps));
				}
			}
			else
			{
				for (uint1024_t i = 0; i < sample_size; ++i)
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
                static const vector<uint1024_t> boost_generate_random_indices(const uint1024_t &sample_size, const uint1024_t &max_size)
		{
			if (sample_size > max_size)
			{
				throw errors::invalid_sample_size_error();
			}
			vector<uint1024_t> random_indices;
			auto seed = random_device{}();
			mt19937 gen{seed};
			uniform_int_distribution<uint1024_t> dist{0,(max_size - 1)};
			set<uint1024_t> candidates;

			while (candidates.size() < sample_size)
			{
				candidates.insert(dist(gen));
			}

			random_indices.insert(random_indices.end(), candidates.begin(), candidates.end());

			return random_indices;
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
				vector<unsigned long long> sampled_indicies = generate_random_indices(sample_size, ps.max_size);
				for (const unsigned long long &i : sampled_indicies)
				{
					subset.push_back(entry_at(combinations, i, ps));
				}
			}
			else
			{
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
                static const vector<unsigned long long> generate_random_indices(const unsigned long long &sample_size, const unsigned long long &max_size)
		{
			if (sample_size > max_size)
			{
				throw errors::invalid_sample_size_error();
			}

			vector<unsigned long long> random_indices;
			random_device rd;
			mt19937_64 generator{rd()};
			uniform_int_distribution<unsigned long long> dist{0,(max_size - 1)};
			set<unsigned long long> candidates;
			while (candidates.size() < sample_size)
			{
				candidates.insert(dist(generator));
			}

			random_indices.insert(random_indices.end(), candidates.begin(), candidates.end());

			return random_indices;
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
