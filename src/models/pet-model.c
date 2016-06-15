#include <pebble.h>
#include "pet-model.h"

// Frequency pet needs to be fed or else it will run away (12hrs)
#define  PET_FEEDING_FREQUENCY 12*3600

struct PetModel {
  time_t born;
  time_t last_fed;
};

PetModel *pet_model_create_new() {
  return pet_model_create_with_data(time(NULL),0);
}

PetModel *pet_model_create_with_data(time_t time_born, time_t time_last_fed) {
  PetModel *this = malloc(sizeof(PetModel));
  this->born = time_born;
  this->last_fed = time_last_fed;

  return this;
}

void pet_model_destroy(PetModel * this) {
  free(this);
}

enum PetState pet_model_get_state(PetModel *this) {
  if (this->last_fed == 0) return PetState_Unborn;

  time_t now = time(NULL);
  if ((now - this->last_fed) >  PET_FEEDING_FREQUENCY) return PetState_RanAway;

  return PetState_Normal;
}

uint32_t pet_model_get_time_born(PetModel *this) {
  return this->born;
}

uint32_t pet_model_get_time_alive(PetModel *this) {
  return time(NULL) - this->born;
}

void pet_model_feed_pet(PetModel *this) {
  this->last_fed = time(NULL);
}

uint32_t pet_model_get_max_time_between_feeding() {
  return  PET_FEEDING_FREQUENCY;
}
