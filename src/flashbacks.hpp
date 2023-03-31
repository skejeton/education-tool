///
/// BRIEF: Everything related to flashbacks
///

#ifndef H_FLASHBACKS_CATEDU
#define H_FLASHBACKS_CATEDU

#define FLASHBACKS_DIALOGS_MAX 128

#include <vector>

typedef size_t FlashbacksDialogId;

enum class FlashbacksDialogChoice {
  UNDEFINED,
  CORRECT,
  WRONG,
};

struct FlashbacksDialog {
  bool taken;
  const char *text, *answer;
  FlashbacksDialogChoice choice;
  FlashbacksDialogId next, prev;
};


/// NOTE: Must be extremely short lived 
struct FlashbacksAllocatedDialog {
  FlashbacksDialogId id;
  FlashbacksDialog *pointer;
};

struct Flashbacks {
  FlashbacksDialog dialogs[FLASHBACKS_DIALOGS_MAX];
  std::vector<FlashbacksDialogId> backlog;
  
  FlashbacksAllocatedDialog alloc_dialog();
  FlashbacksDialog *get_from_id(FlashbacksDialogId id);
  void touch(FlashbacksDialogId id, FlashbacksDialogChoice choice);
};

struct FlashbacksDialogPrototype {
  const char *text, *answer;
};

struct FlashbacksDialogMaker {
  Flashbacks *flashbacks;
  FlashbacksDialogId previous_id;
  FlashbacksDialogId starter_id;

  static FlashbacksDialogMaker from(Flashbacks *flashbacks);
  void append_dialog(FlashbacksDialogPrototype proto);
};


struct FlashbacksGui {
  Flashbacks *flashbacks;
  enum class Mode {
    INACTIVE,
    SEQUENCE,
    BACKLOG
  };

  Mode mode, prev_mode;

  enum class AnswerMode {
    UNKNOWN,
    SEEN_ANSWER,
    CHOSE_ANSWER,
  };
  
  AnswerMode answer_mode;

  static FlashbacksGui create(Flashbacks *flashbacks);
  void begin_sequence(FlashbacksDialogId start);
  
  FlashbacksDialogId sequence_current;


  void show();
};


#endif // H_FLASHBACKS_CATEDU