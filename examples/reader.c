#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "txtquad.h"
#include "inp.h"
#include "parse.h"
#include "watch.h"

// window resizing 1hr
// scrolling 1hr
// starts new line at end of word 1.5hrs
// executable 20-30mins

void inp_ev_text(unsigned int _) { }
void inp_ev_mouse(float _a, float _b, struct Extent _e) { }

char docpath[50] = "./examples/prose/hilbert.md";

struct Doc doc; 
int *charCount; 

struct Share txtquad_update(struct Frame data, struct Text *text)
{
	v3 cam_pos = v3_zero();
	v4 cam_rot = qt_id();

	if (modded != 0) {
		doc.filePath = docpath; 
		doc.numChars = get_num_chars(doc); 
		doc.numTextBlocks = get_num_blocks(doc); 
		charCount = get_block_char_count(doc); 
		read_doc(&doc, charCount); 

		modded = 0; 
		watchthread = pthread_create(&watch, NULL, watch_file, NULL); 
	}

	text->char_count = doc.numChars; 

		float x_i = -2.0f; 
		float y_i = 2.0f; 
		float z_i = 4; 

		int k = 0; 

		// float headerDamping; 

		float textScale = 0.07f; 

		int numVisibleBlocks = 9; // doc.numTextBlocks; 

		for (int blockNum = 0; blockNum < numVisibleBlocks; blockNum++) {
			for (int charNum = 0; charNum < charCount[blockNum]; charNum++) {
				text->chars[k] = (struct Char) {
					.pos = { x_i, y_i, z_i}, 
					.rot = qt_id(), 
					.scale = textScale * doc.block[blockNum].type, 
					.v = doc.block[blockNum].text[charNum], 
					.col = v4_one(), 
				}; 

				x_i += textScale * doc.block[blockNum].type; 

				if (x_i > 2.0f) {
					x_i = -2.0f; 
					y_i -= 0.15f; 
				}

				k++; 
			}

			x_i = -2.0f; 
			y_i -= 0.2f; 
		}

	float asp = (float)data.win_size.w / data.win_size.h;
	m4 view = m4_view(cam_pos, cam_rot);
	m4 proj = m4_persp(60, asp, .001f, 1024);
	m4 vp = m4_mul(proj, view);

	return (struct Share) {
		vp,
		(v2) { data.win_size.w, data.win_size.h },
		data.t,
	};
}

int main()
{
	inp_init(NULL, 0, NULL, 0);

	struct Settings settings = {
		.app_name = "txtquad-reader",
		.asset_path = "./assets/",
		.win_size = { 800, 800 }, 
		.mode = MODE_WINDOWED,
	};

	doc.filePath = docpath; 
	doc.numChars = get_num_chars(doc); 
	doc.numTextBlocks = get_num_blocks(doc); 
	charCount = get_block_char_count(doc); 
	read_doc(&doc, charCount); 

	modded = 0; 
	watchthread = pthread_create(&watch, NULL, watch_file, NULL); 

	txtquad_init(settings);
	txtquad_start();
	exit(0);
}
