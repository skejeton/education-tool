///
/// BRIEF: Quests
/// 

#ifndef H_CATEDU_QUEST
#define H_CATEDU_QUEST

#include <vector>
#include <string>
#include "table.hpp"

typedef TableId ObjectiveId;

struct Objective {
  bool completed;
  const char *id;
  const char *name;
  ObjectiveId parent;
  ObjectiveId sibling;
  ObjectiveId child;
};

struct ObjectiveSystem {
  Table<Objective> objectives;
  ObjectiveId first_element;

  ObjectiveId push_objective(ObjectiveId parent_id, const char *id, const char *name);
};

struct ObjectivesUi {
  ObjectiveSystem *system;

  void show();
};

#endif