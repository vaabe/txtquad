#include <stdlib.h>
#include <math.h>
#include "txtquad.h"
#include "inp.h"

#include "faffing.h"
#include "parse.h"

int essayLen; 
char *essay; 

char hilbertPath[50] = "./examples/hilbert.md"; 

struct Doc doc; 
int *charCount; 

#ifdef DEMO_2
static char cli[1024];
static size_t cli_len;

void inp_ev_text(unsigned int unicode)
{
	char ascii = unicode > 'Z' && unicode <= 'z' ? unicode - 32 : unicode;
	cli[cli_len++] = ascii;
}
#else
void inp_ev_text(unsigned int _) { }
#endif
void inp_ev_mouse(float _a, float _b, struct Extent _e) { }

struct Share txtquad_update(struct Frame data, struct Text *text)
{
	v3 cam_pos = v3_zero();
	v4 cam_rot = qt_id();

#ifdef DEMO_0
	cam_rot = qt_axis_angle(v3_up(), data.t);
	cam_pos = qt_app(cam_rot, v3_neg(v3_fwd()));

	text->char_count = 1;
	text->chars[0] = (struct Char) {
		.pos = { -.5f + PIX_WIDTH * .5f, -.5f, 0 },
		.rot = qt_id(),
		.scale = 1.f,
		.v = 'A',
		.col = v4_one(),
	};

#elif DEMO_4

//	int essayLen = get_essay_len(); 
//	char *essay = get_essay_chars(essayLen); 

	text->char_count = essayLen; 

	// initial x, y, z positions
	float x_i = -4.0f; 
	float y_i = 2.0f; 
	float z_i = 4; 

	for (int i = 0; i < essayLen; i++) {
		text->chars[i] = (struct Char) {
			.pos = { x_i, y_i, z_i }, 
			.rot = qt_id(), 
			.scale = 0.25f, 
			.v = essay[i],
			.col = v4_one(),
		}; 
		x_i += 0.3f; 

		if (x_i > 4.0f) {
			x_i = -4.0f; 
			y_i -= 0.3f; 
		}
	}

#elif DEMO_5

	text->char_count = doc.numChars; 

	float x_i = -4.0f; 
	float y_i = 2.0f; 
	float z_i = 4; 

	int k = 0; 

	// float headerDamping; 

	float textScale = 0.1f; 

	int numVisibleBlocks = 9; 

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

			if (x_i > 4.0f) {
				x_i = -4.0f; 
				y_i -= 0.15f; 
			}

			k++; 
		}

		x_i = -4.0f; 
		y_i -= 0.2f; 
	}

#elif DEMO_1
	char a = 'A' + (1 - .5f * (1 + cos(data.t * .5f))) * 26 + 0;
	char z = 'Z' - (1 - .5f * (1 + cos(data.t * .5f))) * 26 + 1;

	text->char_count = 4;
	text->chars[0] = (struct Char) {
		.pos = { -.5f - .125f, .5f - .125f, 2 },
		.rot = qt_id(),
		.scale = .25f,
		.v = a,
		.col = v4_one(),
	};
	text->chars[1] = (struct Char) {
		.pos = { .5f - .125f, .5f - .125f, 2 },
		.rot = qt_id(),
		.scale = .25f,
		.v = z,
		.col = v4_one(),
	};
	text->chars[2] = (struct Char) {
		.pos = { -.5f - .125f, -.5f - .125f, 2 },
		.rot = qt_id(),
		.scale = .25f,
		.v = z,
		.col = v4_one(),
	};
	text->chars[3] = (struct Char) {
		.pos = { .5f - .125f, -.5f - .125f, 2 },
		.rot = qt_id(),
		.scale = .25f,
		.v = a,
		.col = v4_one(),
	};

#elif DEMO_2
	if (KEY_DOWN(ENTER)) {
		cli[cli_len++] = '\n';
	}

	if (KEY_DOWN(BACKSPACE)) {
		if (cli_len) --cli_len;
	}

	if (KEY_DOWN(ESCAPE)) {
		cli_len = 0;
	}

	text->char_count = 0;
	text->block_count = 1;
	text->blocks[0] = (struct Block) {
		.str = cli,
		.str_len = cli_len,
		.pos = { -.9f, -.9f, 2 },
		.rot = qt_axis_angle(v3_right(), M_PI * .15f),
		.scale = .25f,
		.piv = { 0.f, 1.f },
		.off = { 0.f, 0.f },
		.just = JUST_LEFT,
		.col = v4_one(),
		.spacing = LINE_HEIGHT,
		.col_lim = 8,
		.cursor = '_',
	};

#elif DEMO_3
	cam_rot = qt_axis_angle(v3_up(), sinf(data.t));
	cam_pos = qt_app(cam_rot, v3_neg(v3_fwd()));

	const float a = cosf(2.f * data.t) * .5f + .5f;

	text->char_count = 1;
	text->chars[0] = (struct Char) {
		.pos = { -.5f + PIX_WIDTH * .5f, -.5f, .5f },
		.rot = qt_id(),
		.scale = 1.f,
		.v = 'A',
		.col = (v4) { 1.f, 1.f, 1.f, a },
	};
#endif

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
#ifndef DEMO_2
	inp_init(NULL, 0, NULL, 0);
#else
	int inp_handles[3] = {
		  KEY(ENTER)
		, KEY(BACKSPACE)
		, KEY(ESCAPE)
	};
	inp_init(inp_handles, 3, NULL, 0);
#endif
	struct Settings settings = {
		.app_name = "txtquad-demo",
		.asset_path = "./assets/",
		.win_size = { 800, 600 }, // Ignored
		.mode = MODE_BORDERLESS,
	};

	essayLen = get_essay_len();  
	essay = get_essay_chars(essayLen); 

	doc.filePath = hilbertPath; 
	doc.numChars = get_num_chars(doc); 
	doc.numTextBlocks = get_num_blocks(doc); 
	charCount = get_block_char_count(doc); 
	read_doc(&doc, charCount); 

	txtquad_init(settings);
	txtquad_start();
	exit(0);
}
