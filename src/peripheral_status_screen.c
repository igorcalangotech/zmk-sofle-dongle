/*
 * Tela custom para as metades (perifericos) do Eyelash Sofle em modo dongle.
 *
 * Mostra um texto fixo (CONFIG_EYELASH_PERIPHERAL_NAME) e o percentual de
 * bateria desta metade, no lugar da animacao do nice_oled.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <lvgl.h>

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

static lv_obj_t *battery_label;

struct battery_state {
    uint8_t level;
};

static void battery_update_cb(struct battery_state state) {
    if (battery_label != NULL) {
        lv_label_set_text_fmt(battery_label, "BAT %d%%", state.level);
    }
}

static struct battery_state battery_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    return (struct battery_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
    };
}

// Atualiza o label na thread do display (thread-safe), igual ao padrao do ZMK.
ZMK_DISPLAY_WIDGET_LISTENER(eyelash_periph_battery, struct battery_state, battery_update_cb,
                            battery_get_state);
ZMK_SUBSCRIPTION(eyelash_periph_battery, zmk_battery_state_changed);

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    lv_obj_t *name_label = lv_label_create(screen);
    lv_label_set_text(name_label, CONFIG_EYELASH_PERIPHERAL_NAME);
    lv_obj_align(name_label, LV_ALIGN_CENTER, 0, -8);

    battery_label = lv_label_create(screen);
    lv_label_set_text(battery_label, "BAT --%");
    lv_obj_align(battery_label, LV_ALIGN_CENTER, 0, 8);

    // dispara a leitura inicial da bateria
    eyelash_periph_battery_init();

    return screen;
}
