#include "io.h"
#include "util.h"

#include <png.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

char* data_directory = 0;

void set_data_directory(const char* path)
{
    if(data_directory)
        free(data_directory);
    data_directory = strdup(path);
}

const char* get_data_directory()
{
    return data_directory;
}

FILE* ex_fopen(const char* filepath, const char* mode)
{
    char* final_path = calloc(strlen(data_directory) + strlen(filepath) + 2, sizeof(char));
    strcpy(final_path, data_directory);
    strcat(final_path, "/");
    strcat(final_path, filepath);
    FILE* file = fopen(final_path, mode);
    free(final_path);
    if(!file) {
        fprintf(stderr, "%s not found\n", final_path);
        error("Failed to load file: File not found.");
        return NULL;
    }

    return file;
}

unsigned char* load_file(const char* filepath)
{
    FILE* file = ex_fopen(filepath, "rb");
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char* filedata = calloc(filesize, sizeof(unsigned char));
    fread(filedata, 1, filesize, file);
    filedata[filesize - 1] = '\0';

    fclose(file);
    return filedata;
}

png_byte* load_image_raw(const char* path, uint16_t* w, uint16_t* h)
{
	FILE* infile = fopen(path, "rb");
	if(!infile) {
		warn("Could not open file.");
		return 0;
	}
	
	uint8_t header[8];
	png_structp pstruct;
	png_infop info_struct;
	uint16_t width, height;
	png_byte* image_data;
	png_bytep* row_ptrs;
	
	fread(header, sizeof(uint8_t), 8, infile);
	if(png_sig_cmp(header, 0, 8)) {
        warn("File has an invalid header.");
		return 0;
	}
	pstruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pstruct) {
		warn("Could not read structure of file.");
		return 0;
	}
	info_struct = png_create_info_struct(pstruct);
	if(!info_struct) {
		png_destroy_read_struct(&pstruct, NULL, NULL);
		warn("Could not create info_struct for file.");
		return 0;
	}
	if(setjmp(png_jmpbuf(pstruct))) {
		return 0;
	}
	
	png_init_io(pstruct, infile);
	png_set_sig_bytes(pstruct, 8);
	png_read_info(pstruct, info_struct);
	
	width = png_get_image_width(pstruct, info_struct);
	height = png_get_image_height(pstruct, info_struct);
    png_byte color_type = png_get_color_type(pstruct, info_struct);
    png_byte bit_depth = png_get_bit_depth(pstruct, info_struct);
    int number_of_passes = png_set_interlace_handling(pstruct);
    
	if(color_type == PNG_COLOR_TYPE_RGB) {
		png_set_filler(pstruct, 0xff, PNG_FILLER_AFTER);
	}
	
	png_read_update_info(pstruct, info_struct);
    
    if(setjmp(png_jmpbuf(pstruct))){
		return 0;
	}
	
	int rowbytes = png_get_rowbytes(pstruct, info_struct);
	//rowbytes += 3 - ((rowbytes-1) % 4);
	
	image_data = (png_byte*)malloc(rowbytes * height /** sizeof(png_byte)+15*/);
	row_ptrs = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int i = 0; i < height; i++){
		row_ptrs[height - 1 - i] = image_data + i * rowbytes;
	}
	
	png_read_image(pstruct, row_ptrs);
	
	/*if(png_get_color_type(pstruct, info_struct) != PNG_COLOR_TYPE_RGBA) {
		png_set_add_alpha(pstruct, 0xff, PNG_FILLER_AFTER);
		std::cerr << "Added Alpha channel\n";
	}*/
	
	png_destroy_read_struct(&pstruct, &info_struct, NULL);
	free(row_ptrs);
	fclose(infile);

    *w = width;
    *h = height;
    return image_data;
}

GLuint load_image(const char* path, uint16_t* w, uint16_t* h)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
    png_byte* image_data = load_image_raw(path, w, h);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *w, *h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

	free(image_data);

    return texture;
}

font* load_font(const char* path)
{
    font* f = malloc(sizeof(font));

    f->texture = load_image(path, &f->w, &f->h);
    if(!f->texture) {
        free(f);
        return 0;
    }
    f->w /= 16;
    f->h /= 16;

    return f;
}

sprite make_sprite(const char* path)
{
    sprite spr;
    spr.texture = load_image(path, &spr.width, &spr.height);
    spr.origin_x = 0;
    spr.origin_y = 0;
    return spr;
}

uint8_t load_tileset(const char* path, tileset** tiles, uint8_t count, GLuint texture_handle)
{
    // TODO: Use tileset count constant
    if(count == 32)
        return 0;

    // TODO: load image
    uint16_t w, h;
    png_byte* image_data = load_image_raw(path, &w, &h);
    if(!image_data || w != 1024 || h != 1024) {
        if(image_data)
            free(image_data);
        return 0;
    }
    if(count > 0)
        *tiles = realloc(tiles, (count + 1) * sizeof(tileset));
    else
        *tiles = malloc(sizeof(tileset));
    (*tiles)[count].name = path;

    uint8_t i;
    for(i = 0; i < 32; ++i) {
        int8_t found = 1;
        for(uint8_t j = 0; j < count; ++j) {
            if((*tiles)[j].layer == i) {
                found = 0;
                break;
            }
        }
        if(found)
            break;
    }
    (*tiles)[count].layer = i;

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 1024, 1024, 1, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    checkGLError();

    free(image_data);
    // TODO: Set solids

    return 1;
}

uint8_t load_map(const char* path, tilemap* map)
{
    unsigned char* data = load_file(path);
    if(!data) {
        warn("No map data found.");
        return 0;
    }

    int i;
    for(i = 0; i < TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS; ++i) {
        map->tile_id_data[i] = data[i] - 32;
        map->tile_set_data[i] = 0;
    }
    map->loaded = 1;
    free(data);
    return 1;
}

void save_map(const char* path, tilemap* map)
{
    FILE* file = ex_fopen(path, "wb");
    if(!file)
        return NULL;
    // TODO: This is slow. Try something quicker.
    int i;
    for(i = 0; i < TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS; ++i) {
        fputc(map->tile_id_data[i] + 32, file);
    }
    fputc(0, file);
    fclose(file);
}

typedef struct box
{
    uint16_t pos_x;
    uint16_t pos_y;
    uint16_t size_x;
    uint16_t size_y;
} box;

uint8_t box_contains(box holder, box child)
{
    if(holder.size_x >= child.size_x && holder.size_y >= child.size_y)
        return 1;
    return 0;
}

// TODO: Use this algorithm for packing: http://www.blackpawn.com/texts/lightmaps/default.html
spriteset* load_spriteset(const char* path)
{
    xmlDocPtr doc = xmlReadFile(path, NULL, 0);
    if(!doc)
        return 0;
    xmlNodePtr root = xmlDocGetRootElement(doc);
    for(; root; root = root->next)
        if(root->type == XML_ELEMENT_NODE && !xmlStrcmp(root->name, (const xmlChar*)"spriteset"))
            break;
    if(!root)
        return 0;
    spriteset* set = malloc(sizeof(spriteset));
    set->animations = malloc(sizeof(struct animation));
    set->animation_count = 0;
    png_byte** buffers = malloc(sizeof(png_byte*));
    box* boxes = malloc(sizeof(box));
    for(xmlNodePtr node = root->children; node; node = node->next) {
        if(node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar*)"animation")) {
            set->animation_count++;
            if(set->animation_count > 1) {
                set->animations = realloc(set->animations, set->animation_count * sizeof(struct animation));
                buffers = realloc(buffers, set->animation_count * sizeof(png_byte*));
                boxes = realloc(boxes, set->animation_count * sizeof(box));
            }
            set->animations[set->animation_count - 1].handle = "idle";
            set->animations[set->animation_count - 1].orients = 0;
            set->animations[set->animation_count - 1].length = 1;
            set->animations[set->animation_count - 1].delay = 1;
            set->animations[set->animation_count - 1].loop = 0;
            set->animations[set->animation_count - 1].play = 1;
            set->animations[set->animation_count - 1].origin_x = 0;
            set->animations[set->animation_count - 1].origin_y = 0;
            xmlChar* a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"name"))) {
                set->animations[set->animation_count - 1].handle = strdup((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"length"))) {
                set->animations[set->animation_count - 1].length = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"speed"))) {
                set->animations[set->animation_count - 1].delay = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"orients"))) {
                set->animations[set->animation_count - 1].orients = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"loop"))) {
                set->animations[set->animation_count - 1].loop = strcmp((char*)a, "false") ? 1 : 0;
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"play"))) {
                set->animations[set->animation_count - 1].play = strcmp((char*)a, "false") ? 1 : 0;
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"origin_x"))) {
                set->animations[set->animation_count - 1].origin_x = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"origin_y"))) {
                set->animations[set->animation_count - 1].origin_y = atoi((char*)a);
                free(a);
            }
            a = 0;
            uint8_t keep = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"file"))) {
                // TODO: Load the file here to an array
                uint16_t w, h;
                buffers[set->animation_count - 1] = load_image_raw((char*)a, &w, &h);
                boxes[set->animation_count - 1].size_x = w;
                boxes[set->animation_count - 1].size_y = h;
                boxes[set->animation_count - 1].pos_x = 0;
                boxes[set->animation_count - 1].pos_y = 0;
                if(buffers[set->animation_count - 1])
                    keep = 1;
                free(a);
            }
            if(!keep) {
                set->animation_count--;
                if(set->animation_count > 0) {
                    set->animations = realloc(set->animations, set->animation_count * sizeof(struct animation));
                    buffers = realloc(buffers, set->animation_count * sizeof(png_byte*));
                    boxes = realloc(boxes, set->animation_count * sizeof(box));
                }
            }
        }
    }

    if(set->animation_count == 0) {
        fprintf(stderr, "Error loading spriteset: No animations found.\n");
        free(set->animations);
        free(set);
        free(boxes);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return 0;
    }
    // TODO: Sort by width
    // Set up the initial box for the texture
    box* available = malloc(sizeof(box));
    uint16_t avail_length = 1;
    available[0].pos_x = 0;
    available[0].pos_y = 0;
    available[0].size_x = 1024;
    available[0].size_y = 1024;
    uint16_t width = 1024;
    uint16_t height = 1024;
    uint16_t f_width = 0;
    uint16_t f_height = 0;

    for(int i = 0; i < set->animation_count; ++i) {
        int16_t selected = -1;
        for(int j = 0; j < avail_length; ++j) {
            if(box_contains(available[j], boxes[i]) && (selected < 0 || available[j].size_x * available[j].size_y > available[selected].size_x * available[selected].size_y)) {
                selected = j;
            }
        }
        if(selected == -1) {
            width += 128;
            height += 128;
            avail_length += 2;
            available = realloc(available, avail_length * sizeof(box));
            available[avail_length - 2].pos_x = width - 128;
            available[avail_length - 2].pos_y = 0;
            available[avail_length - 2].size_x = 128;
            available[avail_length - 2].size_y = height;
            available[avail_length - 1].pos_x = 0;
            available[avail_length - 1].pos_y = height - 128;
            available[avail_length - 2].size_x = width - 128;
            available[avail_length - 2].size_y = 128;
        }
        boxes[i].pos_x = available[selected].pos_x;
        boxes[i].pos_y = available[selected].pos_y;
        if(boxes[i].pos_x + boxes[i].size_x > f_width)
            f_width = boxes[i].pos_x + boxes[i].size_x;
        if(boxes[i].pos_y + boxes[i].size_y > f_height)
            f_height = boxes[i].pos_y + boxes[i].size_y;
        avail_length--;
        if(boxes[i].size_x < available[selected].size_x) {
            avail_length++;
            available[selected].pos_x += boxes[i].size_x;
            available[selected].size_x -= boxes[i].size_x;
            if(boxes[i].size_y < available[selected].size_y) {
                avail_length++;
                available = realloc(available, avail_length * sizeof(box));
                available[avail_length - 1].pos_x = boxes[i].pos_x;
                available[avail_length - 1].pos_y = boxes[i].pos_y + boxes[i].size_y;
                available[avail_length - 1].size_x = boxes[i].size_x;
                available[avail_length - 1].size_y = available[selected].size_y - boxes[i].size_y;
            }
        } else if(boxes[i].size_y < available[selected].size_y) {
            avail_length++;
            available[selected].pos_y += boxes[i].size_y;
            available[selected].size_y -= boxes[i].size_y;
        } else {
            for(int k = selected + 1; k <= avail_length; ++k) {
                available[k - 1] = available[k];
            }
            available = realloc(available, avail_length * sizeof(box));
        }
    }

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_width, f_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    for(int i = 0; i < set->animation_count; ++i) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, boxes[i].pos_x, boxes[i].pos_y, boxes[i].size_x, boxes[i].size_y, GL_RGBA, GL_UNSIGNED_BYTE, buffers[i]);
        set->animations[i].offset_x = (float)boxes[i].pos_x / (float)f_width;
        set->animations[i].offset_y = (float)boxes[i].pos_y / (float)f_width;
        set->animations[i].dimensions_x = boxes[i].size_x / set->animations[i].length;
        set->animations[i].dimensions_y = boxes[i].size_y;
        set->animations[i].size_x = (float)boxes[i].size_x / (float)f_width / set->animations[i].length;
        set->animations[i].size_y = (float)boxes[i].size_y / (float)f_height;
    }

    free(boxes);
    free(available);
    for(uint16_t i = 0; i < set->animation_count; ++i)
        free(buffers[i]);
    free(buffers);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    set->atlas = texture;

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return set;
}

spriteset* get_spriteset(const char* path)
{
    spriteset* set = load_spriteset(path);
    return set;
}

void destroy_spriteset(spriteset* set)
{
    glDeleteTextures(1, &set->atlas);
    for(uint8_t i = 0; i < set->animation_count; ++i)
        free(set->animations[i].handle);
    free(set->animations);
    free(set);
}

int16_t load_boxes(const char* path, ui_box** boxes)
{
    xmlDocPtr doc = xmlReadFile(path, NULL, 0);
    if(!doc)
        return 0;
    xmlNodePtr root = xmlDocGetRootElement(doc);
    int16_t count = 0;
    *boxes = malloc(sizeof(ui_box));
    for(; root; root = root->next)
        if(root->type == XML_ELEMENT_NODE && !xmlStrcmp(root->name, (const xmlChar*)"box")) {
            ++count;
            if(count != 1)
                boxes = realloc(boxes, count * sizeof(ui_box));
            (*boxes[count - 1]).texture = 0;
            xmlChar* a = 0;
            if((a = xmlGetProp(root, (const xmlChar*)"src"))) {
                (*boxes[count - 1]).texture = load_image((char*)a, &(*boxes[count - 1]).w, &(*boxes[count - 1]).h);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(root, (const xmlChar*)"w"))) {
                (*boxes[count - 1]).border_w = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(root, (const xmlChar*)"h"))) {
                (*boxes[count - 1]).border_h = atoi((char*)a);
                free(a);
            }

            float rx = (float)(*boxes[count - 1]).border_w / (float)(*boxes[count - 1]).w;
            float ry = (float)(*boxes[count - 1]).border_h / (float)(*boxes[count - 1]).h;

            float uvs[] = {
                rx, 0,
                0, 0,
                0, ry,
                rx, 0,
                0, ry,
                rx, ry,

                1, 0,
                1 - rx, 0,
                1 - rx, ry,
                1, 0,
                1 - rx, ry,
                1, ry,

                rx, 1 - ry,
                0,  1 - ry,
                0,  1,
                rx, 1 - ry,
                0,  1,
                rx, 1,

                1, 1 - ry,
                1 - rx, 1 - ry,
                1 - rx, 1,
                1, 1 - ry,
                1 - rx, 1,
                1, 1,

                rx, ry,
                0, ry,
                0, 1 - ry,
                rx, ry,
                0, 1 - ry,
                rx, 1 - ry,

                1, ry,
                1 - rx, ry,
                1 - rx, 1 - ry,
                1, ry,
                1 - rx, 1 - ry,
                1, 1 - ry,

                1 - rx, 0,
                rx, 0,
                rx, ry,
                1 - rx, 0,
                rx, ry,
                1 - rx, ry,

                1 - rx, 1 - ry,
                rx, 1 - ry,
                rx, 1,
                1 - rx, 1 - ry,
                rx, 1,
                1 - rx, 1,

                1 - rx, ry,
                rx, ry,
                rx, 1 - ry,
                1 - rx, ry,
                rx, 1 - ry,
                1 - rx, 1 - ry,
            };
            glGenBuffers(1, &(*boxes[count - 1]).uvs);
            glBindBuffer(GL_ARRAY_BUFFER, (*boxes[count - 1]).uvs);
            glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), uvs, GL_STATIC_DRAW);

            if((*boxes[count - 1]).texture == 0)
                --count;
        }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    if(count == 0)
        return -1;
    return count;
}
