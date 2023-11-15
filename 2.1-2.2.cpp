#include <map>
#include <set>
#include <iostream>
#include <string>

/**
 * Добавление макросов уменьшит дублирующий код:
 * #define t_kv template <typename K, typename V>
 * #define t_si template <typename K = std::string, typename V = int>
 * 
 * Но formatter и подсветка синтаксиса сильно ломаются при их использовании. vscode =(
*/


/**
 * Не нашел другого способа использовать typedef с дженериками. - https://stackoverflow.com/questions/3591024/typedef-alias-of-an-generic-class
 * До С++ 0.3:
	
	template <typename T>
	struct MyOwnMap
	{
		typedef std::map<std::string, T> Type;
	};

	MyOwnMap<int>::Type map;

  * После данного стандарта, можно использовать template + using.
*/

template <typename K, typename V>
using Map = std::map<K, V>;

template <typename V>
using Set = std::set<V>;

template <typename K, typename V>
using MultiMap = std::multimap<K, V>;

/**
 * Можно обойтись обычным typedef:
 * typedef key = std::string;
 * typedef value = int;
 * typedef map = std::map<key, value>;
 *
 * В данном случае поддерживаем только определенный тип key и value.
 */

namespace Print
{
	template <typename K, typename V>
	void print(MultiMap<K, V> &mp)
	{
		std::cout << "Start function print, type std::multimap." << std::endl;

		for (const auto &[key, value] : mp)
		{
			printf("Key: %s, Value: %d", key.c_str(), value);
			std::cout << std::endl;
		}

		std::cout << "End function print, type std::multimap" << std::endl;
	}

	template <typename K, typename V>
	void print(Map<K, V> &mp)
	{
		std::cout << "Start function print, type std::map." << std::endl;

		for (const auto &[key, value] : mp)
		{
			printf("Key: %s, Value: %d", key.c_str(), value);
			std::cout << std::endl;
		}

		std::cout << "End function print, type std::map" << std::endl;
	}

	template <typename V>
	void print(Set<V> &set)
	{
		std::cout << "Start function print, type std::set." << std::endl;

		for (const auto &v : set)
		{
			std::cout << v << " ";
		}

		std::cout << std::endl;
		std::cout << "End function print, type std::set" << std::endl;
	}
}

/**
 * Можно объединить MapMethods и MultiMapMethods, реализация не сильно отличается -> меньше дублирования.
 * Но кажется данные интерфейсы не имеет смысла объединять и все же они должны быть независимы.
 * 
 * Для объединения как вариант использовать интерфейс с одинаковыми методами:
 * 
 * template <typename Collection, typename K, typename V>
 * class AbstractCollectionMethods {
 * 		public:
 * 			virtual ~AbstractCollectionMethods() {}	
        	virtual void push(Collection<K, V> mp, K key, V value) = 0;
        	virtual Set<V> to_set(Collection<K, V> mp) = 0;
			template <typename Predicator = bool (&)(const V)>
        	virtual Collection<K, V> filter(Collection<K, V> mp, Predicator p) = 0;
 * }
 *
 * class Map ...
 * class MultiMap ...
 * 
*/
namespace MapMethods
{
	template <typename K, typename V>
	void push(Map<K, V> &mp, K key, V value)
	{
		if (mp.contains(key))
		{
			throw std::invalid_argument("This key already exist.");
		}

		mp[key] = value;
	}

	// template <typename Set = std::set<value>>
	template <typename K, typename V>
	Set<V> to_set(Map<K, V> &mp)
	{
		Set<V> set;

		for (const auto &[_, value] : mp)
		{
			set.insert(value);
		}

		return set;
	}

	template <typename K, typename V, typename Predicate = bool (&)(const V)>
	Map<K, V> filter(Map<K, V> &mp, Predicate p)
	{
		Map<K, V> filtered_map;

		for (const auto &[key, value] : mp)
		{
			if (p(value))
			{
				filtered_map[key] = value;
			}
		}

		return filtered_map;
	}
}

namespace TestMap
{
	template <typename K = std::string, typename V = int>
	void fill_map(Map<K, V> &mp)
	{
		MapMethods::push(mp, std::string("1"), 1);
		MapMethods::push(mp, std::string("2"), 2);
		MapMethods::push(mp, std::string("3"), 3);
		MapMethods::push(mp, std::string("31"), 31);
	}

	template <typename K = std::string, typename V = int>
	void test_push()
	{
		Map<K, V> mp;
		fill_map(mp);
		Print::print(mp);
	}

	template <typename K = std::string, typename V = int>
	void test_to_set()
	{
		Map<K, V> mp;
		fill_map(mp);
		Set<V> set = MapMethods::to_set(mp);
		Print::print(set);
	}

	template <typename K = std::string, typename V = int>
	void test_filter()
	{
		Map<K, V> mp;
		fill_map(mp);

		auto predicator = [&](const V v)
		{
			int threshold = 30;
			return v <= threshold;
		};

		Map<K, V> filtered_map = MapMethods::filter(mp, predicator);
		Print::print(filtered_map);
	}

	void run()
	{
		test_push();
		test_to_set();
		test_filter();
	}
}

namespace MultiMapMethods
{
	template <typename K, typename V>
	void push(MultiMap<K, V> &mp, K key, V value)
	{
		mp.insert({key, value});
	}

	template <typename K, typename V, typename Set = std::set<V>>
	Set to_set(MultiMap<K, V> &mp)
	{
		Set set;

		for (auto it = mp.begin(); it != mp.end(); it++)
		{
			set.insert(it->second);
		}

		return set;
	}

	template <typename K, typename V, typename Predicate = bool (&)(const V)>
	MultiMap<K, V> filter(MultiMap<K, V> &mp, Predicate p)
	{
		MultiMap<K, V> filtered_map;
		for (const auto &[key, value] : mp)
		{
			if (p(value))
			{
				filtered_map.insert({key, value});
			}
		}

		return filtered_map;
	}
}

namespace TestMultiMap
{
	template <typename K = std::string, typename V = int>
	void fill_map(MultiMap<K, V> &mp)
	{
		MultiMapMethods::push(mp, std::string("1"), 1);
		MultiMapMethods::push(mp, std::string("1"), 2);
		MultiMapMethods::push(mp, std::string("2"), 2);
		MultiMapMethods::push(mp, std::string("3"), 3);
		MultiMapMethods::push(mp, std::string("31"), 31);
	}

	template <typename K = std::string, typename V = int>
	void test_push()
	{
		MultiMap<K, V> mp;
		fill_map(mp);
		Print::print(mp);
	}

	template <typename K = std::string, typename V = int>
	void test_to_set()
	{
		MultiMap<K, V> mp;
		fill_map(mp);
		Set<V> set = MultiMapMethods::to_set(mp);
		Print::print(set);
	}

	template <typename K = std::string, typename V = int>
	void test_filter()
	{
		MultiMap<K, V> mp;
		fill_map(mp);

		auto predicator = [&](const V v)
		{
			int threshold = 30;
			return v <= threshold;
		};

		MultiMap<K, V> filtered_map = MultiMapMethods::filter(mp, predicator);
		Print::print(filtered_map);
	}

	void run()
	{
		test_push();
		test_to_set();
		test_filter();
	}
}

int main()
{
	TestMap::run();
	TestMultiMap::run();
}
