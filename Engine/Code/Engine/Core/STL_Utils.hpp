#pragma once

#include <vector>
#include <map>

template <typename T>
void QuickErase(std::vector<T*>& targetVector, T* objectToRemove)
{
	for (int i = 0; i < targetVector.size(); ++i)
	{
		T* target = targetVector[i];
		if (target == objectToRemove)
		{
			delete target;

			size_t size = targetVector.size();
			targetVector[i] = targetVector[size - 1];
			targetVector.pop_back();
			i--;
		}
	}
}

template <typename K, class V>
V FindInMap(std::map<K, V>& targetMap, K keyToFind)
{
	auto search = targetMap.find(keyToFind);
	if (search != targetMap.end())
	{
		return search->second;
	}

	return nullptr;
}
