#pragma once

#include <pebble.h>

typedef struct PetModel PetModel;

enum PetState {
  PetState_Unborn,
  PetState_Normal,
  PetState_RanAway
};

PetModel * pet_model_create_new();
PetModel * pet_model_create_with_data(time_t time_born, time_t time_last_fed);
void  pet_model_destroy(PetModel *this);

void  pet_model_feed_pet(PetModel *this);

uint32_t  pet_model_get_time_born(PetModel *this);
uint32_t  pet_model_get_time_alive(PetModel *this);
enum PetState  pet_model_get_state(PetModel *this);

uint32_t  pet_model_get_max_time_between_feeding();
