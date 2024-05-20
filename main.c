#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mathomatic/lib/mathomatic.h"
#include "mathomatic/includes.h"
#include "scene_essentials.h"
#include "external/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

#define VISUAL_STYLES_COUNT 4
// raygui embedded styles
#include "./external/styles/style_bluish.h"      // raygui style: bluish
#include "./external/styles/style_cyber.h"       // raygui style: cyber
#include "./external/styles/style_dark.h"        // raygui style: dark
#include "./external/styles/style_candy.h"       // raygui style: candy
void changeVisualStyle(int *prevStyle, const int crntStyle);

#define DEFAULT_OUT "logs.txt"

float run_function(char* func, float val);
void regula_falsi(float *x_next, float x0, float x1, float fx0, float fx1, size_t *iter);
float nultocka(char *func, float x0, float x1, float eps, size_t max_iter);

void exit_program(int exit_val) { exit(exit_val); }
void usage(FILE *fp) { (void)fp; return; }

size_t g_screen_width = 800;
size_t g_screen_height = 600;

#define SET_FONT_SIZE(size) GuiSetStyle(DEFAULT, TEXT_SIZE, fmax(0, (int)(size)))
#define CURR_FONT_SIZE GuiGetStyle(DEFAULT, TEXT_SIZE)

int best_font_size(char *text, float width, float height) { 
	int curr_size = CURR_FONT_SIZE;
	const float lower_limit = 4.0/10.0;
	const float middle_limit = 9.0/10.0;
	const float upper_limit = 2.0;

	for (size_t __iter = 0; __iter < 20; __iter++) {
		
		Vector2 txt_size = MeasureTextEx(GuiGetFont(), text, curr_size, GuiGetStyle(DEFAULT, TEXT_SPACING));
		if (txt_size.x > upper_limit*width) curr_size -= 4;
		else if (txt_size.x > width) curr_size--;
		else if (txt_size.x < lower_limit*width) curr_size += 4;
		else if (txt_size.x < middle_limit*width) curr_size++;

		if (txt_size.y > upper_limit*height) curr_size -= 4;
		else if (txt_size.y > height) curr_size--;
	}

	return curr_size;
}

bool editing_func = false;
bool editing_x0 = false;
bool editing_x1 = false;
char *func_txt;
int main(int argc, char** argv) {

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(g_screen_width, g_screen_height, "Regula Falsi");

	// Load default style
    	GuiLoadStyleBluish();
    	int activeVisualStyle = 0;
    	int prevActiveVisualStyle = 0;

	const char* styleChooserText 		    = "Bluish;Cyber;Dark;Candy"; // STYLE CHOOSER

	size_t left_pad   = 24;
	size_t top_pad    = 48;
	size_t layer_pad  = 14;

	size_t max_func_text = 256;
	func_txt = calloc(max_func_text, 1);

	int x0 = 0, x1 = 0;
	float aprox_nultocka = NAN;

	bool draw_error = false;
	float error_time = 0;


	while(!WindowShouldClose()) {
		g_screen_height = (size_t) GetScreenHeight();
		g_screen_width  = (size_t) GetScreenWidth();

		changeVisualStyle(&prevActiveVisualStyle, activeVisualStyle);


		if (IsKeyPressed(KEY_TAB)) {
			if (editing_func) {
				editing_func = false;
				editing_x0 = true;
			} else if (editing_x0) {
				editing_x0 = false;
				editing_x1 = true;
			} else if (editing_x1) {
				editing_x1 = false;
				editing_func = true;
			}
		}

		if (IsKeyPressed(KEY_ENTER)) {

			FILE *logs = fopen(DEFAULT_OUT, "w");
			gfp = logs;

			aprox_nultocka = nultocka(func_txt, x0, x1, 1e-3, 100);
			fclose(logs);

			if (isnan(aprox_nultocka)) {
				draw_error = true;
				error_time = 0;
			}
		}

		BeginDrawing();

			ClearBackground(SC_BACKGROUND);


			{
				GuiLabel((Rectangle){ left_pad, 10, (int)(g_screen_width / 10), 24 }, "Style:");
				Rectangle styleChooserRec = { left_pad + 42 ,10, 120, 24 };
				GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
				GuiComboBox(styleChooserRec, styleChooserText, &activeVisualStyle);
				if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), styleChooserRec)) {
					activeVisualStyle = ((activeVisualStyle + VISUAL_STYLES_COUNT) - 1) % VISUAL_STYLES_COUNT;
				}
				GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			}

			Rectangle function_input = {left_pad, top_pad, g_screen_width - 2*left_pad, 1/3.0*g_screen_height};

			size_t func_txt_size = CURR_FONT_SIZE;
			SET_FONT_SIZE(best_font_size(func_txt, function_input.width, function_input.height));

			if (GuiTextBox(function_input, func_txt, max_func_text, editing_func)) {
				editing_func = !editing_func;
			}
			SET_FONT_SIZE(func_txt_size);


			int x_label_font = best_font_size("X0:", left_pad, function_input.height/2.0);	

			Rectangle spinner_x0 = {
				2*left_pad, 
				top_pad + layer_pad + function_input.height, 
				function_input.width/2 - left_pad, 
				function_input.height/2
			};
			Rectangle spinner_x1 = {
				2*left_pad + spinner_x0.width + spinner_x0.x, 
				top_pad + layer_pad + function_input.height, 
				function_input.width/2 - 2*left_pad, 
				function_input.height/2
		        };

			int prev_size = CURR_FONT_SIZE;
			SET_FONT_SIZE(x_label_font);

			if (GuiSpinner(spinner_x0, "X0:", &x0, INT_MIN, INT_MAX, editing_x0)) {
				editing_x0 = !editing_x0;
			}

			if (GuiSpinner(spinner_x1, "X1:", &x1, INT_MIN, INT_MAX, editing_x1)) {
				editing_x1 = !editing_x1;

			}

			SET_FONT_SIZE(prev_size);


			Rectangle ispis_rez = {
				left_pad, 
				spinner_x1.y + spinner_x1.height + layer_pad, 
				function_input.width,
				function_input.height,
			};

			if (!isnan(aprox_nultocka)) {
				char *text = malloc(max_func_text);
				snprintf(text, max_func_text, "NULTOCKA: %f", aprox_nultocka);
				
				prev_size = CURR_FONT_SIZE;

				GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
				SET_FONT_SIZE(best_font_size(text, ispis_rez.width, ispis_rez.height));
				GuiLabel(ispis_rez, text);
				SET_FONT_SIZE(prev_size);
				GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			} else {
				prev_size = CURR_FONT_SIZE;

				char *text = "S ENTER pokrece se metoda regula falsi.";

				SET_FONT_SIZE(best_font_size(text, ispis_rez.width, ispis_rez.height));
				GuiDummyRec(ispis_rez, text);

				SET_FONT_SIZE(prev_size);
			}


			if (draw_error) {
				GuiUnlock();
				error_time += GetFrameTime();
				if (error_time > 4) draw_error = false;
				
				SET_FONT_SIZE(16);		
				GuiMessageBox((Rectangle) {
					g_screen_width/2.0 - 0.25*g_screen_width,
					g_screen_height/2.0 - 0.25*g_screen_height,
					0.5*g_screen_width,
					0.5*g_screen_height,
				}, "Greska pri izvodenju metode!", 
				"Potrebno je definirati pocetne tocke koje \nobuhvacaju nultocku za metodu regula falsi!", "");
				SET_FONT_SIZE(prev_size);
				GuiLock();
			} else {
				GuiUnlock();
			}

			

		EndDrawing();
	}

	CloseWindow();

	exit(EXIT_SUCCESS);
}

void changeVisualStyle(int *prevVisualStyle, const int visualStyle) {

	if (visualStyle != *prevVisualStyle)
        {

            GuiLoadStyleDefault();
            switch (visualStyle)
            {	
                case 0: GuiLoadStyleBluish();	 break;
                case 1: GuiLoadStyleCyber();	 break;
                case 2: GuiLoadStyleDark();	 break;
		case 3: GuiLoadStyleCandy();	 break;
                default: break;
            }

            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

            *prevVisualStyle = visualStyle;
        }
}


float run_function(char* func, float val) {

	char *output;
	int rv;

	if (!matho_init()) {
		printf("Not enough memory.\n");
		exit(1);
	}
	matho_parse(func, NULL);
	char buff[50];
	snprintf(buff, sizeof(buff), "replace x with %f", val);
	rv = matho_process(buff, NULL);

	rv = matho_process("calculate", &output);
	float value = NAN;
	if (output) {
		value = strtof(output, NULL);
		fprintf(gfp, "Value: %f\n", value);
		if (rv) {
		    free(output);
		} else {
		    printf("Error return.\n");
		}
	}

	return value;
}

void regula_falsi(float *x_next, float x0, float x1, float fx0, float fx1, size_t *iter) {

	*x_next = x0 - ((x1 - x0) / (fx1 - fx0))*fx0;
	++(*iter);
	printf("Iteracija #%3zu X = %7.5f \n", *iter, *x_next);
}


float nultocka(char *func, float x0, float x1, float eps, size_t max_iter) {

	size_t iter = 0;
	float x_curr,x_next;
	regula_falsi(&x_curr, x0, x1, run_function(func, x0), run_function(func, x1), &iter);

	do {
		if (isnan(x_curr)) return NAN;
		if (run_function(func, x0)*run_function(func, x_curr) < 0) {
			x1 = x_curr;
		} else {
			x0 = x_curr;
		}

		regula_falsi(&x_next, x0, x1, run_function(func, x0), run_function(func, x1), &iter);

		if (fabs(x_next-x_curr) < eps)
		{
			printf("Nakon %zu iteracija, nultočka = %6.4f\n", iter, x_next);
			return x_next;
		}

		x_curr = x_next;
	} while (iter < max_iter);

	printf("Riješenje ne postoji ili je broj iteracija premalen!\n");

	return NAN;
}
