#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <cerver/files.h>

#include "test.h"

static const char *bmp_extension = { "bmp" };
static const char *gif_extension = { "gif" };
static const char *png_extension = { "png" };
static const char *jpg_extension = { "jpg" };
static const char *jpeg_extension = { "jpeg" };

static const char *jpg_file_small = { "test.jpg" };
static const char *jpg_file_complete = { "/home/ermiry/Pictures/test.jpg" };
static const char *jpg_file_bad = { "test" };
static const char *jpg_file_wrong = { "/home/ermiry/Pictures/test" };

static const char *jpeg_file_small = { "test.jpeg" };
static const char *jpeg_file_complete = { "/home/ermiry/Pictures/test.jpeg" };
static const char *jpeg_file_bad = { "test" };
static const char *jpeg_file_wrong = { "/home/ermiry/Pictures/test" };

static const char *png_file_small = { "test.png" };
static const char *png_file_complete = { "/home/ermiry/Pictures/test.png" };
static const char *png_file_bad = { "test" };
static const char *png_file_wrong = { "/home/ermiry/Pictures/test" };

#pragma region main

static void test_files_get_file_extension_reference_jpg (void) {

	unsigned int ext_len = 0;
	test_check_str_eq (files_get_file_extension_reference (jpg_file_small, &ext_len), jpg_extension, NULL);
	test_check_unsigned_eq (ext_len, strlen (jpg_extension), NULL);

	ext_len = 0;
	test_check_str_eq (files_get_file_extension_reference (jpg_file_complete, &ext_len), jpg_extension, NULL);
	test_check_unsigned_eq (ext_len, strlen (jpg_extension), NULL);

	ext_len = 0;
	test_check_null_ptr (files_get_file_extension_reference (jpg_file_bad, &ext_len));
	test_check_unsigned_eq (ext_len, 0, NULL);

	ext_len = 0;
	test_check_null_ptr (files_get_file_extension_reference (jpg_file_wrong, &ext_len));
	test_check_unsigned_eq (ext_len, 0, NULL);

}

static void test_files_get_file_extension_reference_jpeg (void) {

	unsigned int ext_len = 0;
	test_check_str_eq (files_get_file_extension_reference (jpeg_file_small, &ext_len), jpeg_extension, NULL);
	test_check_unsigned_eq (ext_len, strlen (jpeg_extension), NULL);

	ext_len = 0;
	test_check_str_eq (files_get_file_extension_reference (jpeg_file_complete, &ext_len), jpeg_extension, NULL);
	test_check_unsigned_eq (ext_len, strlen (jpeg_extension), NULL);

	ext_len = 0;
	test_check_null_ptr (files_get_file_extension_reference (jpeg_file_bad, &ext_len));
	test_check_unsigned_eq (ext_len, 0, NULL);

	ext_len = 0;
	test_check_null_ptr (files_get_file_extension_reference (jpeg_file_wrong, &ext_len));
	test_check_unsigned_eq (ext_len, 0, NULL);

}

static void test_files_get_file_extension_reference_png (void) {

	unsigned int ext_len = 0;
	test_check_str_eq (files_get_file_extension_reference (png_file_small, &ext_len), png_extension, NULL);
	test_check_unsigned_eq (ext_len, strlen (png_extension), NULL);

	ext_len = 0;
	test_check_str_eq (files_get_file_extension_reference (png_file_complete, &ext_len), png_extension, NULL);
	test_check_unsigned_eq (ext_len, strlen (png_extension), NULL);

	ext_len = 0;
	test_check_null_ptr (files_get_file_extension_reference (png_file_bad, &ext_len));
	test_check_unsigned_eq (ext_len, 0, NULL);

	ext_len = 0;
	test_check_null_ptr (files_get_file_extension_reference (png_file_wrong, &ext_len));
	test_check_unsigned_eq (ext_len, 0, NULL);

}

static void test_files_get_file_extension_reference (void) {
	
	test_files_get_file_extension_reference_jpg ();
	test_files_get_file_extension_reference_jpeg ();
	test_files_get_file_extension_reference_png ();

}

#pragma endregion

#pragma region images

static const char *bmp_type = { "BMP" };
static const char *gif_type = { "GIF" };
static const char *png_type = { "PNG" };
static const char *jpeg_type = { "JPEG" };
static const char *other_type = { "None" };

static void test_files_image_type_to_string (void) {

	test_check_str_eq (files_image_type_to_string (IMAGE_TYPE_NONE), other_type, NULL);

	test_check_str_eq (files_image_type_to_string (IMAGE_TYPE_PNG), png_type, NULL);
	test_check_str_eq (files_image_type_to_string (IMAGE_TYPE_JPEG), jpeg_type, NULL);
	test_check_str_eq (files_image_type_to_string (IMAGE_TYPE_GIF), gif_type, NULL);
	test_check_str_eq (files_image_type_to_string (IMAGE_TYPE_BMP), bmp_type, NULL);

	test_check_str_eq (files_image_type_to_string ((ImageType) 64), other_type, NULL);
	test_check_str_eq (files_image_type_to_string ((ImageType) 100), other_type, NULL);

}

static void test_files_image_get_type_by_extension (void) {
	
	test_check_unsigned_eq (files_image_get_type_by_extension (jpg_file_small), IMAGE_TYPE_JPEG, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (jpg_file_complete), IMAGE_TYPE_JPEG, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (jpg_file_bad), IMAGE_TYPE_NONE, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (jpg_file_wrong), IMAGE_TYPE_NONE, NULL);

	test_check_unsigned_eq (files_image_get_type_by_extension (jpeg_file_small), IMAGE_TYPE_JPEG, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (jpeg_file_complete), IMAGE_TYPE_JPEG, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (jpeg_file_bad), IMAGE_TYPE_NONE, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (jpeg_file_wrong), IMAGE_TYPE_NONE, NULL);

	test_check_unsigned_eq (files_image_get_type_by_extension (png_file_small), IMAGE_TYPE_PNG, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (png_file_complete), IMAGE_TYPE_PNG, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (png_file_bad), IMAGE_TYPE_NONE, NULL);
	test_check_unsigned_eq (files_image_get_type_by_extension (png_file_wrong), IMAGE_TYPE_NONE, NULL);

}

static void test_files_image_extension_is_jpeg (void) {
	
	test_check_bool_eq (files_image_extension_is_jpeg (jpg_file_small), true, NULL);
	test_check_bool_eq (files_image_extension_is_jpeg (jpg_file_complete), true, NULL);
	test_check_bool_eq (files_image_extension_is_jpeg (jpg_file_bad), false, NULL);
	test_check_bool_eq (files_image_extension_is_jpeg (jpg_file_wrong), false, NULL);

	test_check_bool_eq (files_image_extension_is_jpeg (jpeg_file_small), true, NULL);
	test_check_bool_eq (files_image_extension_is_jpeg (jpeg_file_complete), true, NULL);
	test_check_bool_eq (files_image_extension_is_jpeg (jpeg_file_bad), false, NULL);
	test_check_bool_eq (files_image_extension_is_jpeg (jpeg_file_wrong), false, NULL);

}

static void test_files_image_extension_is_png (void) {
	
	test_check_bool_eq (files_image_extension_is_png (png_file_small), true, NULL);
	test_check_bool_eq (files_image_extension_is_png (png_file_complete), true, NULL);
	test_check_bool_eq (files_image_extension_is_png (png_file_bad), false, NULL);
	test_check_bool_eq (files_image_extension_is_png (png_file_wrong), false, NULL);

}

#pragma endregion

int main (int argc, char **argv) {

	(void) printf ("Testing FILES...\n");

	// main
	test_files_get_file_extension_reference ();

	// images
	test_files_image_type_to_string ();
	test_files_image_get_type_by_extension ();
	test_files_image_extension_is_jpeg ();
	test_files_image_extension_is_png ();

	(void) printf ("\nDone with FILES tests!\n\n");

	return 0;

}