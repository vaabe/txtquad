#include <stdlib.h>
#include <math.h>
#include "txtquad.h"
#include "inp.h"
#include "watch.h"

#include <pthread.h>

void inp_ev_text(unsigned int _) { }
void inp_ev_mouse(float _a, float _b, struct Extent _e) { }

#define MAX_BUFFER 5000

// threads for watching
int changeflag = 0;  
pthread_t watch; 
int thread; 

char hilbertPath[50] = "./examples/docus/hilbert.md"; 

struct Doc doc; 
int *charCount; 

struct TextBlock {
	char *text; 
	int type; 
}; 

struct Doc {
	char *filePath; 
	int numChars; 
	int numTextBlocks; 
	struct TextBlock *block; 
}; 

void *watch_loop(void *arg)
{
	int fd; 
	int wd; 
	int i = 0; 
	char buffer[EVENT_BUF_LEN]; 
	int length; 

	// int check = *((int*)kon); 

	// printf("%d\n", fd); 

	fd = inotify_init(); 
	if (fd < 0) {
		perror("inotify_init"); 
	}

	// printf("%d\n", fd); 

	wd = inotify_add_watch(fd, "./examples/docus", IN_MODIFY); 
	length = read(fd, buffer, EVENT_BUF_LEN); 
	// printf("%d\n", check); 
	changeflag += 1; 
	// printf("%d %d\n", fd, length); 
	// return fd; 
	pthread_exit(NULL); 

}


struct TextBlock *new_block(int numTextBlocks, int charCount[], int type)
{
	struct TextBlock *block = malloc(numTextBlocks * sizeof(struct TextBlock)); 

	for (int i = 0; i < numTextBlocks; i++)
	{
		block[i].text = malloc((charCount[i] + 5) * sizeof(char)); 
		block[i].type = type; 
	}

	return block; 
}

void read_doc(struct Doc *doc, int charCount[])
{
	FILE *docFile = fopen(doc->filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	int ty = 2; 

	doc->block = new_block(doc->numTextBlocks, charCount, ty); 

	char buffer[doc->numTextBlocks][MAX_BUFFER]; 

	for (int i = 0; i < doc->numTextBlocks; i++) {
		fgets(buffer[i], sizeof(buffer[i]), docFile); 
		doc->block[i].text = buffer[i]; 

		if (buffer[i][0] == '#') {
			if (buffer[i][1] == '#') {
				doc->block[i].type = 2;
				doc->block[i].text += 3; 
			}
			else {
				doc->block[i].type = 3; 
				doc->block[i].text += 2; 
			}	
		}
		else if (buffer[i][0] == ' ') {
			doc->block[i].type = 0; 
		}
		else {
			doc->block[i].type = 1; 
		}
	}
}

int get_num_blocks(struct Doc doc)
{
	FILE *docFile = fopen(doc.filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	char buffer[MAX_BUFFER]; 
	int blockNum = 0; 

	while (fgets(buffer, sizeof(buffer), docFile) != NULL) {
		blockNum++; 
	}

	fclose(docFile); 
	return blockNum; 
}

int get_num_chars(struct Doc doc)
{
	FILE *docFile = fopen(doc.filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	int n = 0; 

	while(fgetc(docFile) != EOF) {
		n++; 
	}

	return n; 
}

int *get_block_char_count(struct Doc doc)
{
	FILE *docFile = fopen(doc.filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	char buffer[MAX_BUFFER]; 
	int blockNum = 0; 

	int *charCount = calloc(doc.numTextBlocks, sizeof(int)); 
	int n = 0; 

	while (fgets(buffer, sizeof(buffer), docFile) != NULL) {

		int m = 0; 

		while ((int)buffer[m] != 0) {
			m++; 
		}

		charCount[blockNum] = m; 
		
		blockNum++; 
	}

	fclose(docFile); 
	return charCount; 
}

struct Share txtquad_update(struct Frame data, struct Text *text)
{
	v3 cam_pos = v3_zero();
	v4 cam_rot = qt_id();

	if (changeflag != 0) {
		doc.filePath = hilbertPath; 
		doc.numChars = get_num_chars(doc); 
		doc.numTextBlocks = get_num_blocks(doc); 
		charCount = get_block_char_count(doc); 
		read_doc(&doc, charCount); 
		// printf("changed!\n"); 

		changeflag = 0; 
		thread = pthread_create(&watch, NULL, watch_loop, NULL); 
	}

	text->char_count = doc.numChars; 

		float x_i = -4.0f; 
		float y_i = 2.0f; 
		float z_i = 4; 

		int k = 0; 

		// float headerDamping; 

		float textScale = 0.1f; 

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

				if (x_i > 4.0f) {
					x_i = -4.0f; 
					y_i -= 0.15f; 
				}

				k++; 
			}

			x_i = -4.0f; 
			y_i -= 0.2f; 
		}

//	char a = 'A' + (1 - .5f * (1 + cos(data.t * .5f))) * 26 + 0;
//	char z = 'Z' - (1 - .5f * (1 + cos(data.t * .5f))) * 26 + 1;
//
//	text->char_count = 4;
//	text->chars[0] = (struct Char) {
//		.pos = { -.5f - .125f, .5f - .125f, 2 },
//		.rot = qt_id(),
//		.scale = .25f,
//		.v = a,
//		.col = v4_one(),
//	};
//	text->chars[1] = (struct Char) {
//		.pos = { .5f - .125f, .5f - .125f, 2 },
//		.rot = qt_id(),
//		.scale = .25f,
//		.v = z,
//		.col = v4_one(),
//	};
//	text->chars[2] = (struct Char) {
//		.pos = { -.5f - .125f, -.5f - .125f, 2 },
//		.rot = qt_id(),
//		.scale = .25f,
//		.v = z,
//		.col = v4_one(),
//	};
//	text->chars[3] = (struct Char) {
//		.pos = { .5f - .125f, -.5f - .125f, 2 },
//		.rot = qt_id(),
//		.scale = .25f,
//		.v = a,
//		.col = v4_one(),
//	};

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
		.win_size = { 800, 600 }, // Ignored
		.mode = MODE_BORDERLESS,
	};

	doc.filePath = hilbertPath; 
	doc.numChars = get_num_chars(doc); 
	doc.numTextBlocks = get_num_blocks(doc); 
	charCount = get_block_char_count(doc); 
	read_doc(&doc, charCount); 

	changeflag = 0; 
	int k = 0; 
	thread = pthread_create(&watch, NULL, watch_loop, NULL); 

	txtquad_init(settings);
	txtquad_start();
	exit(0);
}
