#include <pebble.h>
#include "app-glance-virtual-pet.h"
#include "pebble-ui-dialog-window/pebble-ui-dialog-window.h"
#include "models/pet-model.h"

// Our pet frog (NULL means it's an egg)
static PetModel *s_pet;

// Our UI and UI Elements
static UIDialogWindow *s_window;
static GBitmap *s_frog_image;
static GBitmap *s_egg_image;
static char s_message[32];

static void prv_update_ui() {
  // Set the message and icon based on state of pet
  if(s_pet) {
    // Frog
    ui_dialog_window_set_icon(s_window, s_frog_image);
    snprintf(s_message, 32, "Mmm delicious!\nScore: %lu", pet_model_get_time_alive(s_pet));
  } else {
    // Egg
    ui_dialog_window_set_icon(s_window, s_egg_image);
    snprintf(s_message, 32, "Press Select to hatch egg");
  }
}

static void prv_on_select(ClickRecognizerRef recognizer, void *context) {
  // If they pressed select in egg mode, spawn a new frog and update UI
  if (!s_pet) {
    s_pet = pet_model_create_new();
    persist_write_int( PERSIST_KEY_PET_BORN, (int) pet_model_get_time_born(s_pet));
    prv_update_ui();
  } else {
    // If we have a frog, quit
    window_stack_pop(true);
  }
}

static void prv_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_on_select);
}

static void prv_update_app_glance_egg(AppGlanceReloadSession *session, size_t limit, void *context) {
  // Update app glance when user exits app with an egg

  // Ensure we have sufficient slices
  if (limit < 1) return;

  // Create the slice
  const AppGlanceSlice slice = (AppGlanceSlice) {
    .layout = {
      .icon_resource_id = RESOURCE_ID_ICON_EGG,
      .template_string = "Open app to hatch egg"
    },
    // TODO: Update to APP_GLANCE_SLICE_NO_EXPIRATION in sdk4-dp2
    .expiration_time = time(NULL) + 31557600
  };

  // add the slice, and check the result
  AppGlanceResult result = app_glance_add_slice(session, slice);
  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error adding AppGlanceSlice: %d", result);
  }

}

static void prv_update_app_glance_pet(AppGlanceReloadSession *session, size_t limit, void *context) {
  // Update app glance when user exits app with a pet

  // Ensure we have sufficient slices
  if (limit < NUM_STATES) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Error: app needs %d slices (%zu available)", NUM_STATES, limit);
  }

  time_t expiration_time = time(NULL);

  // Build and add NUM_STATES slices
  for (int i = 0; i < NUM_STATES; i++) {
    expiration_time += pet_model_get_max_time_between_feeding() / NUM_STATES;

    const AppGlanceSlice slice = {
      .layout = {
        .icon_resource_id = icons[i],
        .template_string = messages[i]
      },
      // TODO: Update `time(NULL) + 31557600` to APP_GLANCE_SLICE_NO_EXPIRATION in sdk4-dp2
      .expiration_time = i == (NUM_STATES - 1) ? time(NULL) + 31557600 : expiration_time
    };

    // add the slice, and check the result
    AppGlanceResult result = app_glance_add_slice(session, slice);
    if (result != APP_GLANCE_RESULT_SUCCESS) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error adding AppGlanceSlice: %d", result);
    }
  }
}

static void prv_init_pet() {
  // Load data
  time_t born = persist_read_int( PERSIST_KEY_PET_BORN);
  time_t last_fed = persist_read_int( PERSIST_KEY_PET_LAST_FED);

  // If the pet hasn't been born yet, don't initialize anything
  if (!born) return;

  // Otherwise, create the pet with the expected data
  s_pet = pet_model_create_with_data(born, last_fed);

  // If the pet has run away, free the memory
  // s_pet == NULL indicates egg mode
  if (pet_model_get_state(s_pet) == PetState_RanAway) {
    pet_model_destroy(s_pet); s_pet = NULL;
  }
}

static void prv_init_ui() {
  // Create the bitmap objects
  s_frog_image = gbitmap_create_with_resource(RESOURCE_ID_FROG);
  s_egg_image = gbitmap_create_with_resource(RESOURCE_ID_EGG);

  // Create a dialog window using the pebble-ui-dialog-window package
  s_window = ui_dialog_window_create(s_message, s_egg_image);
  ui_dialog_window_set_icon_alignment(s_window, GAlignCenter);
  ui_dialog_window_set_label_alignment(s_window, GTextAlignmentCenter);
  ui_dialog_window_set_label_font(s_window, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  // Update UI to set message and image properly
  prv_update_ui();

  Window *window = ui_dialog_window_get_window(s_window);
  window_set_click_config_provider(window, prv_config_provider);

  // Push window
  window_stack_push(window, true);
}

static void prv_deinit_ui() {
  // Destroy the window
  ui_dialog_window_destroy(s_window);

  // Destroy the images
  gbitmap_destroy(s_frog_image);
  gbitmap_destroy(s_egg_image);
}

static void prv_deinit_pet() {
  if (s_pet) {
    // Reload the app glance to display frog mode
    app_glance_reload(prv_update_app_glance_pet, NULL);

    // "feed" the pet, then free the memory
    persist_write_int( PERSIST_KEY_PET_LAST_FED, (int) time(NULL));
    pet_model_destroy(s_pet);
  } else {
    // Reload the app glance to display egg mode
    app_glance_reload(prv_update_app_glance_egg, NULL);
  }
}

static void prv_init() {
  prv_init_pet();
  prv_init_ui();
}

static void prv_deinit() {
  prv_deinit_ui();
  prv_deinit_pet();
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
