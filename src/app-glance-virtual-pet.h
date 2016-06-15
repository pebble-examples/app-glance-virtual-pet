// Persist Keys
#define PERSIST_KEY_PET_BORN      0
#define PERSIST_KEY_PET_LAST_FED  1

// Game defines
#define NUM_STATES 4

// State Information
const uint32_t icons[NUM_STATES] = {
  RESOURCE_ID_ICON_FROG_HAPPY,
  RESOURCE_ID_ICON_FROG_HUNGRY,
  RESOURCE_ID_ICON_FROG_VERY_HUNGRY,
  RESOURCE_ID_ICON_FROG_MISSING
};

const char *messages[NUM_STATES] = {
  "Mmm, that was delicious!!",
  "I'm getting hungry..",
  "I'm so hungry!! Please feed me soon..",
  "Your pet run away :("
};
