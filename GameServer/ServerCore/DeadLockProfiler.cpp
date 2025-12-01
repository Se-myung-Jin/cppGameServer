#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	int32 lockId = 0;

	auto findItem = _nameToId.find(name);
	if (findItem == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else
	{
		lockId = findItem->second;
	}

	if (_lockStack.empty() == false)
	{
		const int32 prevId = _lockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	// 디버깅 용 차원에서 검증용 코드
	if (_lockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId)
		CRASH("INVALID_UNLOCK");

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		DFS(lockId);

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::DFS(int32 index)
{
	if (_discoveredOrder[index] != -1)
		return;

	_discoveredOrder[index] = _discoveredCount++;

	auto findItem = _lockHistory.find(index);
	if (findItem == _lockHistory.end())
	{
		_finished[index] = true;
		return;
	}

	set<int32>& nextSet = findItem->second;
	for (int32 next : nextSet)
	{
		if (_discoveredOrder[next] == -1)
		{
			_parent[next] = index;
			DFS(next);
			continue;
		}
		
		if (_discoveredOrder[index] < _discoveredOrder[next])
			continue;

		if (_finished[next] == false)
		{
			printf("%s -> %s\n", _idToName[index], _idToName[next]);
			
			int32 now = index;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == next)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	_finished[index] = true;
}
