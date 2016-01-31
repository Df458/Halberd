#include "texture_util.h"
#include "io_util.h"
#include "util.h"
#include "xml_util.h"
#include "render_util.h"

#include <png.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// Hidden structures
///////////////////////////////////////////////////////////////////////////////

typedef struct texture_atlas_box
{
    uint16_t pos_x;
    uint16_t pos_y;
    uint16_t size_x;
    uint16_t size_y;
} texture_atlas_box;

///////////////////////////////////////////////////////////////////////////////
// Hidden functions
///////////////////////////////////////////////////////////////////////////////

png_byte* load_png_to_buffer(const char* path, uint16_t* w, uint16_t* h)
{
    FILE* infile = fopen(path, "rb");
    if(!infile) {
        warn("Could not open file.");
        fprintf(stderr, "Error Path: %s\n", path);
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
    
    // TODO: This next line came out weird, please confirm that this is correct
    image_data = (png_byte*)malloc(rowbytes * height * sizeof(png_byte)+15);
    row_ptrs = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int i = 0; i < height; i++){
        row_ptrs[height - 1 - i] = image_data + i * rowbytes;
    }
    
    png_read_image(pstruct, row_ptrs);
    
    if(png_get_color_type(pstruct, info_struct) != PNG_COLOR_TYPE_RGBA) {
        png_set_add_alpha(pstruct, 0xff, PNG_FILLER_AFTER);
    }
    
    png_destroy_read_struct(&pstruct, &info_struct, NULL);
    free(row_ptrs);
    fclose(infile);

    *w = width;
    *h = height;
    return image_data;
}

uint8_t box_contains(texture_atlas_box holder, texture_atlas_box child)
{
    if(holder.size_x >= child.size_x && holder.size_y >= child.size_y)
        return 1;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

texture* create_texture(uint16_t w, uint16_t h)
{
	texture* texture_data = malloc(sizeof(texture));
	glGenTextures(1, &texture_data->texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_data->texture_id);
    texture_data->texture_width = w;
    texture_data->texture_height = h;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_data->texture_width, texture_data->texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    return texture_data;
}

void destroy_texture(texture* tex)
{
    glDeleteTextures(1, &tex->texture_id);
    free(tex);
}

texture* load_resource_to_texture(const char* resource_location, const char* resource_name)
{
	texture* texture_data = malloc(sizeof(texture));
	glGenTextures(1, &texture_data->texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_data->texture_id);
	
    char* path = construct_extended_resource_path(resource_location, resource_name);
    // TODO: Add support for additional types
    png_byte* image_data = load_png_to_buffer(path, &texture_data->texture_width, &texture_data->texture_height);
    free(path);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_data->texture_width, texture_data->texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

	free(image_data);

    return texture_data;
}

bool save_texture_to_resource(texture* tex, const char* resource_location, const char* resource_name)
{
    stub(false);
}

font* create_font(uint16_t w, uint16_t h)
{
	font* font_data = malloc(sizeof(font));
    font_data->glyph_width = w;
    font_data->glyph_width = h;
    font_data->texture_data = create_texture(font_data->glyph_width * 16, font_data->glyph_height * 16);

    return font_data;
}

void destroy_font(font* fnt)
{
    destroy_texture(fnt->texture_data);
    free(fnt);
}

font* load_resource_to_font(const char* resource_location, const char* resource_name)
{
    font* new_font         = malloc(sizeof(font));
    new_font->texture_data = load_resource_to_texture(resource_location, resource_name);
    new_font->glyph_width  = new_font->texture_data->texture_width  / 16;
    new_font->glyph_width  = new_font->texture_data->texture_height / 16;

    return new_font;
}

bool save_font_to_resource(font* fnt, const char* resource_location, const char* resource_name)
{
    stub(false);
}

sprite* create_sprite()
{
	sprite* sprite_data = malloc(sizeof(sprite));
    sprite_data->atlas = 0;
    sprite_data->animations = 0;
    sprite_data->animation_count = 0;

    return sprite_data;
}

void destroy_sprite(sprite* spr)
{
    if(spr->atlas) {
            destroy_texture(spr->atlas);
        for(uint8_t i = 0; i < spr->animation_count; ++i)
            free(spr->animations[i].handle);
        free(spr->animations);
    }
    free(spr);
}

// TODO: Add support for paths in sprite files
sprite* load_resource_to_sprite(const char* resource_location, const char* resource_name)
{
    xmlDocPtr doc = load_resource_to_xml(resource_location, resource_name);
    if(!doc)
        return 0;
    xmlNodePtr root = xmlDocGetRootElement(doc);
    for(; root; root = root->next)
        if(root->type == XML_ELEMENT_NODE && !xmlStrcmp(root->name, (const xmlChar*)"sprite"))
            break;
    if(!root)
        return 0;
    sprite* spr = malloc(sizeof(sprite));
    spr->animations = malloc(sizeof(struct animation));
    spr->animation_count = 0;
    png_byte** buffers = malloc(sizeof(png_byte*));
    texture_atlas_box* boxes = malloc(sizeof(texture_atlas_box));
    for(xmlNodePtr node = root->children; node; node = node->next) {
        if(node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar*)"animation")) {
            spr->animation_count++;
            if(spr->animation_count > 1) {
                spr->animations = realloc(spr->animations, spr->animation_count * sizeof(struct animation));
                buffers = realloc(buffers, spr->animation_count * sizeof(png_byte*));
                boxes = realloc(boxes, spr->animation_count * sizeof(texture_atlas_box));
            }
            spr->animations[spr->animation_count - 1].handle = "idle";
            spr->animations[spr->animation_count - 1].orients = 0;
            spr->animations[spr->animation_count - 1].length = 1;
            spr->animations[spr->animation_count - 1].delay = 1;
            spr->animations[spr->animation_count - 1].loop = 0;
            spr->animations[spr->animation_count - 1].play = 1;
            spr->animations[spr->animation_count - 1].origin_x = 0;
            spr->animations[spr->animation_count - 1].origin_y = 0;
            xmlChar* a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"name"))) {
                spr->animations[spr->animation_count - 1].handle = strdup((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"length"))) {
                spr->animations[spr->animation_count - 1].length = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"speed"))) {
                spr->animations[spr->animation_count - 1].delay = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"orients"))) {
                spr->animations[spr->animation_count - 1].orients = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"loop"))) {
                spr->animations[spr->animation_count - 1].loop = strcmp((char*)a, "false") ? 1 : 0;
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"play"))) {
                spr->animations[spr->animation_count - 1].play = strcmp((char*)a, "false") ? 1 : 0;
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"origin_x"))) {
                spr->animations[spr->animation_count - 1].origin_x = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"origin_y"))) {
                spr->animations[spr->animation_count - 1].origin_y = atoi((char*)a);
                free(a);
            }
            a = 0;
            uint8_t keep = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"file"))) {
                uint16_t w, h;
                char* ex_path = construct_extended_resource_path(resource_location, (char*)a);
                buffers[spr->animation_count - 1] = load_png_to_buffer(ex_path, &w, &h);
                boxes[spr->animation_count - 1].size_x = w;
                boxes[spr->animation_count - 1].size_y = h;
                boxes[spr->animation_count - 1].pos_x = 0;
                boxes[spr->animation_count - 1].pos_y = 0;
                if(buffers[spr->animation_count - 1])
                    keep = 1;
                free(a);
            }
            if(!keep) {
                spr->animation_count--;
                if(spr->animation_count > 0) {
                    spr->animations = realloc(spr->animations, spr->animation_count * sizeof(struct animation));
                    buffers = realloc(buffers, spr->animation_count * sizeof(png_byte*));
                    boxes = realloc(boxes, spr->animation_count * sizeof(texture_atlas_box));
                }
            }
        }
    }

    if(spr->animation_count == 0) {
        fprintf(stderr, "Error loading sprite: No animations found.\n");
        free(spr->animations);
        free(spr);
        free(boxes);
        xmlFreeDoc(doc);
        /*xmlCleanupParser();*/
        return 0;
    }
    // TODO: Sort by width
    // spr up the initial box for the texture
    texture_atlas_box* available = malloc(sizeof(texture_atlas_box));
    uint16_t avail_length = 1;
    available[0].pos_x = 0;
    available[0].pos_y = 0;
    available[0].size_x = 1024;
    available[0].size_y = 1024;
    uint16_t width = 1024;
    uint16_t height = 1024;
    uint16_t f_width = 0;
    uint16_t f_height = 0;

    for(int i = 0; i < spr->animation_count; ++i) {
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
            available = realloc(available, avail_length * sizeof(texture_atlas_box));
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
                available = realloc(available, avail_length * sizeof(texture_atlas_box));
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
            available = realloc(available, avail_length * sizeof(texture_atlas_box));
        }
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_width, f_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    for(int i = 0; i < spr->animation_count; ++i) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, boxes[i].pos_x, boxes[i].pos_y, boxes[i].size_x, boxes[i].size_y, GL_RGBA, GL_UNSIGNED_BYTE, buffers[i]);
        spr->animations[i].offset_x = (float)boxes[i].pos_x / (float)f_width;
        spr->animations[i].offset_y = (float)boxes[i].pos_y / (float)f_width;
        spr->animations[i].dimensions_x = boxes[i].size_x / spr->animations[i].length;
        spr->animations[i].dimensions_y = boxes[i].size_y;
        spr->animations[i].size_x = (float)boxes[i].size_x / (float)f_width / spr->animations[i].length;
        spr->animations[i].size_y = (float)boxes[i].size_y / (float)f_height;
    }

    free(boxes);
    free(available);
    for(uint16_t i = 0; i < spr->animation_count; ++i)
        free(buffers[i]);
    free(buffers);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    spr->atlas->texture_id = texture;
    spr->atlas->texture_width = f_width;
    spr->atlas->texture_width = f_height;

    xmlFreeDoc(doc);
    return spr;
}

bool save_sprite_to_resource(sprite* spr, const char* resource_location, const char* resource_name)
{
    stub(false);
}
///////////////////////////////////////////////////////////////////////////////

tileset load_resource_to_tileset(const char* resource_location, const char* resource_name, GLuint texture_handle, uint8_t layer)
{
    // TODO: Set the variables here correctly
    tileset new_tileset;
    uint16_t w, h;
    char* path = construct_extended_resource_path(resource_location, resource_name);
    png_byte* image_data = load_png_to_buffer(path, &w, &h);
    free(path);
    if(!image_data || w != 1024 || h != 1024) {
        if(image_data)
            free(image_data);
        // TODO: Fail this out
    }
    if(resource_location)
        new_tileset.resource_location = strdup(resource_location);
    else
        new_tileset.resource_location = 0;
    new_tileset.resource_name = strdup(resource_name);

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 1024, 1024, 1, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    checkGLError();

    free(image_data);
    // TODO: Set solids
    return new_tileset;
}
