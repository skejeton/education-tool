///
/// BRIEF: Editor for entities and flashbacks.
///

#ifndef H_CATEDU_ENTITY_EDITOR
#define H_CATEDU_ENTITY_EDITOR

#include "flashbacks.hpp"
#include "vector"

struct EntityEditor {
  std::vector<FlashbacksDialogPrototype> prototypes;

  void show();
	static EntityEditor derive_from(Flashbacks *flashbacks, FlashbacksDialogId start);
  void emplace(Flashbacks *flashbacks, FlashbacksDialogId *start);
};

#endif