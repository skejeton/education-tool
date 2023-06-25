///
/// BRIEF: Quests
/// 

#ifndef H_CATEDU_QUEST
#define H_CATEDU_QUEST

#include <vector>

struct Objective {
  bool completed;
  char* id;
  char* name;
};

struct ObjectiveList {
  std::vector<Objective> objectives;
  
  Objective *get_objective_by_id(char* id);
  void push_objective(char *id, char* name);
  void complete_objective(char *id);
};

struct ObjectiveListUi {
  ObjectiveList* list;

  void show();
};

#endif