/* lazy-cartesian-product.hpp
 * (c) Tyler Burdsall - 2018
 * 
 * Licensed under the MIT license
 */


#ifndef _LAZY_CARTESIAN_PRODUCT
#define _LAZY_CARTESIAN_PRODUCT

#define LCP_MAJOR_VERSION 1
#define LCP_MINOR_VERSION 1

#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <stdexcept>


using std::fstream;
using std::ios;
using std::mt19937_64;
using std::random_device;
using std::sort;
using std::string;
using std::uniform_int_distribution;
using std::unordered_set;
using std::vector;
using std::runtime_error;

namespace lazycp
{
namespace errors
{
	struct index_error: public runtime_error
	{
		index_error::index_error(): runtime_error("The given index cannot be out of range") {}
	};
	struct empty_list_error: public runtime_error
	{
		empty_list_error::empty_list_error(): runtime_error("The given list of combinations cannot be empty") {}
	};
	struct empty_answers_error: public runtime_error
	{
		empty_answers_error::empty_answers_error(): runtime_error("The given list of answers cannot be empty") {}
	};
	struct invalid_sample_size_error: public runtime_error
	{
		invalid_sample_size_error::invalid_sample_size_error(): runtime_error("The given sample size cannot be out of range") {}
	};
}
}

namespace lazycp
{
	
	struct precomputed_stats
	{
		vector<long> divs;
		vector<long> mods;
		long max_size;
	};

	class lazy_cartesian_product
	{
	public:
		static const vector<string> entry_at(const vector<vector<string>> &combinations, const long &index)
		{
			const precomputed_stats pc = precompute(combinations);
			if (index  < 0 || index > pc.max_size)
			{
				throw errors::index_error();
			}

			const vector<string> combination = entry_at(combinations, index, pc);
			return combination;
		}

		static const vector<vector<string>> generate_samples(const vector<vector<string>> combinations, const long &sample_size)
		{
			if (combinations.size() == 0)
			{
				throw errors::empty_list_error();
			}
			precomputed_stats ps = precompute(combinations);

			vector<vector<string>> subset;
			if (sample_size != ps.max_size)
			{
				vector<long> sampled_indicies = generate_random_indices(sample_size, ps.max_size);
				for (const long &i : sampled_indicies)
				{
					subset.push_back(entry_at(combinations, i, ps));
				}
			}
			else
			{
				for (long i = 0; i < sample_size; ++i)
				{
					subset.push_back(entry_at(combinations, i, ps));
				}
			}

			return subset;
		}

		static const long compute_max_size(const vector<vector<string>> &combinations)
		{
			long size = 1;
			for (vector<vector<string>>::const_iterator it = combinations.begin(); it != combinations.end(); ++it)
			{
				size *= (*it).size();
			}

			return size;
		}

	private:
		static const precomputed_stats precompute(const vector<vector<string>> &combinations)
		{
			precomputed_stats ps;
			if (combinations.size() == 0)
			{
				throw errors::empty_answers_error();
			}

			long size = combinations.size();
			ps.divs.resize(size);
			ps.mods.resize(size);
			long factor = 1;

			for (int i = size - 1; i >= 0; --i)
			{
				long items = combinations[i].size();
				ps.divs[i] = factor;
				ps.mods[i] = items;
				factor *= items;
			}

			ps.max_size = compute_max_size(combinations);
			return ps;
		}
		static const vector<long> generate_random_indices(const long &sample_size, const long &max_size)
		{
			if (sample_size > max_size)
			{
				throw errors::invalid_sample_size_error();
			}

			vector<long> random_indices;
			random_device rd;
			mt19937_64 generator{rd()};
			uniform_int_distribution<> dist{0, (int)(max_size - 1)};
			unordered_set<long> candidates;
			while (candidates.size() < (unsigned)sample_size)
			{
				candidates.insert(dist(generator));
			}

			random_indices.insert(random_indices.end(), candidates.begin(), candidates.end());

			sort(random_indices.begin(), random_indices.end());

			return random_indices;
		}
		static const bool sample_size_valid(const long &sample, const long &max_size)
		{
			return sample <= max_size;
		}
		
		static const vector<string> entry_at(const vector<vector<string>> &combinations, const long &n, const precomputed_stats &ps)
		{
			long length = combinations.size();
			vector<string> combination(length);

			for (long i = 0; i < length; ++i)
			{
				combination[i] = combinations[i][(n / ps.divs[i]) % ps.mods[i]];
			}

			return combination;
		}

		lazy_cartesian_product() {}
	};
}

#endif
