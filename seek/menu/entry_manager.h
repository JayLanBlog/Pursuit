#pragma once
#include "entry_view.h"
#include"../tool/helper/singletonh.h"

using namespace Seek::Menu;
using namespace Seek::Helper;

class EntryManager : public Singleton<EntryManager> {
public:

	void PushDebugEntry(EntryDebugger* entry) {
		entryList.push_back(entry);
	}

	int EntryListCount() {
		return entryList.size();
	}

	EntryDebugger* GetEntryByIndex(int index) {
		if (index > entryList.size())
			return new EntryDebugger();
		return entryList[index];
	}

private:
	std::vector<EntryDebugger*> entryList;
};