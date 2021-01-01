#include "hud.h"

xpm_object *hud_wave, *hud_charges, *hud_score;
static uint16_t score = 0;
static uint8_t charges = 0,  wave = 0;
static bool first = true; 

void draw_current_hud(){
    set_hud_values();
    if(first){
        first = false;
        hud_wave = create_sprite(hud_overlay_wave, "hud_wave",35,20);
        hud_charges = create_sprite(hud_overlay_charges, "hud_charges", 880, 690);
        hud_score = create_sprite(hud_overlay_score, "hud_score", 30, 690);
    }
    draw_score();
    draw_charges();
    draw_waves();
    print_xpm(hud_wave);
    print_xpm(hud_charges);
    print_xpm(hud_score);
}
  
void set_hud_values(){
    score = get_score();
    charges = (uint8_t) get_available_blasts();
    wave = get_wave();
}

void draw_score(){
    uint8_t third_digit = (score/100)%10, second_digit = (score/10)%10, first_digit = score%10;
    int x = 65, y = 740, spacing_x = 30;
    xpm_object* xpm = create_sprite(digits[third_digit], "third_digit_score", x, y);
    draw_digit(xpm);
    x += spacing_x;
    xpm = create_sprite(digits[second_digit], "second_digit_score", x, y);
    draw_digit(xpm);
    x += spacing_x;
    xpm = create_sprite(digits[first_digit], "first_digit_score", x, y);
    draw_digit(xpm);
}

void draw_charges(){
    uint8_t second_digit = (charges/10)%10, first_digit = charges%10;
    int x = 970, y = 740, spacing_x = 30;
    xpm_object* xpm = create_sprite(digits[second_digit], "second_digit_charges", x, y);
    draw_digit(xpm);
    x += spacing_x;
    xpm = create_sprite(digits[first_digit], "first_digit_charges", x, y);
    draw_digit(xpm);
}

void draw_waves(){
    uint8_t second_digit = (wave/10)%10, first_digit = wave%10;
    int x = 78, y = 70, spacing_x = 30;
    xpm_object* xpm = create_sprite(digits[second_digit], "second_digit_waves", x, y);
    draw_digit(xpm);
    x += spacing_x;
    xpm = create_sprite(digits[first_digit], "first_digit_waves", x, y);
    draw_digit(xpm);
}

void draw_digit(xpm_object* xpm){
    for (size_t i = 0; i < (xpm->img).height; i++) {
        for (size_t j = 0; j < (xpm->img).width; j++) {
        uint32_t color = 0;
        for (size_t byte = 0; byte < get_bytes_per_pixel(); byte++) {
            color |= (*(xpm->map + (j + i * (xpm->img).width) * get_bytes_per_pixel() + byte)) << (byte * 8);
        }
        if (color == xpm_transparency_color((xpm->img).type))
            continue;
        draw_static_buffer_pixels(xpm->x + j, xpm->y + i, color, get_buffer_to_draw());
        }
    }
    free_xpm_mem(xpm);
}

void draw_time(){
    char * time = print_time();
    uint8_t length = strlen(time);
    int x = 470, y=750;
    for (uint8_t i = 0; i < length ; i++){
        char elem = time[i];
        if(elem >= 0x30 && elem <= 0x39){
            uint8_t index = elem % 0x30;
            xpm_object* xpm = create_sprite(digits[index], "number", x, y);
            x += (xpm->img).width + 5;
            draw_digit(xpm);
        }else if(elem == 0x3A){
            xpm_object* xpm = create_sprite(colon, "colon", x, y);
            x += (xpm->img).width + 5;
            draw_digit(xpm);
        }
    }
}

void draw_date(){
    char* date = print_date();
    uint8_t length = strlen(date);
    uint8_t num_letters = get_h_resolution()/30;
    uint8_t rest = ((num_letters-length)%2)/2;
    int x = (get_h_resolution()%30) + ((num_letters-length)/2)*30 + rest, y=650;
    for (uint8_t i = 0; i < length ; i++){
        char elem = date[i];
        if(elem == 0x2C){ //comma
            xpm_object* xpm = create_sprite(comma, "comma", x, y);
            x += (xpm->img).width + 2;
            draw_digit(xpm);
        }else if(elem >= 0x41 && elem <= 0x5A){
            uint8_t index = elem % 0x41;
            xpm_object* xpm = create_sprite(alphabet[index], "letter", x, y);
            x += (xpm->img).width + 2;
            draw_digit(xpm);
        }else if(elem >= 0x30 && elem <= 0x39){
            uint8_t index = elem % 0x30;
            xpm_object* xpm = create_sprite(digits[index], "number", x, y);
            x += (xpm->img).width + 2;
            draw_digit(xpm);
        }else x += 30;
    }
}


void free_xpm_mem(xpm_object* xpm){
    free(xpm->map);
    free(xpm->ID);
    free(xpm);
}

void reset_hud(){
    score = 0;
    charges = 0;
    wave = 0;
}
