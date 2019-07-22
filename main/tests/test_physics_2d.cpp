/*************************************************************************/
/*  test_physics_2d.cpp                                                  */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "test_physics_2d.h"

#include "core/map.h"
#include "core/os/input_event.h"
#include "core/os/main_loop.h"
#include "core/os/os.h"
#include "core/print_string.h"
#include "scene/resources/texture.h"
#include "servers/physics_2d_server.h"
#include "servers/visual_server.h"

static const unsigned char convex_png[] = {
	0x89, 0x50, 0x4e, 0x47, 0xd, 0xa, 0x1a, 0xa, 0x0, 0x0, 0x0, 0xd, 0x49, 0x48, 0x44, 0x52, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 0x40, 0x8, 0x6, 0x0, 0x0, 0x0, 0xaa, 0x69, 0x71, 0xde, 0x0, 0x0, 0x0, 0x1, 0x73, 0x52, 0x47, 0x42, 0x0, 0xae, 0xce, 0x1c, 0xe9, 0x0, 0x0, 0x0, 0x6, 0x62, 0x4b, 0x47, 0x44, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf9, 0x43, 0xbb, 0x7f, 0x0, 0x0, 0x0, 0x9, 0x70, 0x48, 0x59, 0x73, 0x0, 0x0, 0xb, 0x13, 0x0, 0x0, 0xb, 0x13, 0x1, 0x0, 0x9a, 0x9c, 0x18, 0x0, 0x0, 0x0, 0x7, 0x74, 0x49, 0x4d, 0x45, 0x7, 0xdb, 0x6, 0xa, 0x3, 0x13, 0x31, 0x66, 0xa7, 0xac, 0x79, 0x0, 0x0, 0x4, 0xef, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0xed, 0x9b, 0xdd, 0x4e, 0x2a, 0x57, 0x14, 0xc7, 0xf7, 0x1e, 0xc0, 0x19, 0x38, 0x32, 0x80, 0xa, 0x6a, 0xda, 0x18, 0xa3, 0xc6, 0x47, 0x50, 0x7b, 0xa1, 0xd9, 0x36, 0x27, 0x7e, 0x44, 0xed, 0x45, 0x4d, 0x93, 0x3e, 0x40, 0x1f, 0x64, 0x90, 0xf4, 0x1, 0xbc, 0xf0, 0xc2, 0x9c, 0x57, 0x30, 0x4d, 0xbc, 0xa8, 0x6d, 0xc, 0x69, 0x26, 0xb5, 0x68, 0x8b, 0x35, 0x7e, 0x20, 0xb4, 0xf5, 0x14, 0xbf, 0x51, 0x3c, 0x52, 0xe, 0xc, 0xe, 0xc8, 0xf0, 0xb1, 0x7a, 0x51, 0x3d, 0xb1, 0x9e, 0x19, 0x1c, 0x54, 0x70, 0x1c, 0xdc, 0x9, 0x17, 0x64, 0x8, 0xc9, 0xff, 0xb7, 0xd6, 0x7f, 0xcd, 0x3f, 0x2b, 0xd9, 0x8, 0xbd, 0x9c, 0xda, 0x3e, 0xf8, 0x31, 0xff, 0xc, 0x0, 0x8, 0x42, 0x88, 0x9c, 0x9f, 0x9f, 0xbf, 0xa, 0x87, 0xc3, 0xad, 0x7d, 0x7d, 0x7d, 0x7f, 0x23, 0x84, 0x78, 0x8c, 0x31, 0xaf, 0x55, 0x0, 0xc6, 0xc7, 0x14, 0x1e, 0x8f, 0xc7, 0xbf, 0x38, 0x3c, 0x3c, 0x6c, 0x9b, 0x9f, 0x9f, 0x6f, 0xb8, 0x82, 0x9b, 0xee, 0xe8, 0xe8, 0xf8, 0x12, 0x0, 0xbe, 0xd3, 0x2a, 0x8, 0xfc, 0x50, 0xd1, 0xf9, 0x7c, 0x9e, 0x8a, 0x46, 0xa3, 0x5f, 0x9d, 0x9e, 0x9e, 0x7e, 0xb2, 0xb0, 0xb0, 0x60, 0xe5, 0x79, 0x1e, 0xf1, 0xfc, 0x7f, 0x3a, 0x9, 0x21, 0x88, 0x10, 0x82, 0x26, 0x26, 0x26, 0xde, 0x77, 0x75, 0x75, 0x85, 0x59, 0x96, 0xfd, 0x5e, 0x6b, 0x20, 0xf0, 0x7d, 0x85, 0x4b, 0x92, 0xf4, 0xfa, 0xe0, 0xe0, 0xe0, 0xd3, 0xb9, 0xb9, 0xb9, 0x46, 0x49, 0x92, 0xea, 0x6f, 0xa, 0xbf, 0x7d, 0x8, 0x21, 0x68, 0x70, 0x70, 0xb0, 0x38, 0x39, 0x39, 0x79, 0xd6, 0xd9, 0xd9, 0xb9, 0xcf, 0x30, 0xcc, 0xa2, 0xd6, 0xad, 0x21, 0x2b, 0x1c, 0x0, 0x38, 0x41, 0x10, 0xfc, 0xdb, 0xdb, 0xdb, 0x27, 0x1e, 0x8f, 0x27, 0x4b, 0x8, 0x1, 0x84, 0x90, 0xea, 0xf, 0x21, 0x4, 0x3c, 0x1e, 0x4f, 0x76, 0x67, 0x67, 0x67, 0x3f, 0x9f, 0xcf, 0xff, 0x7c, 0x5, 0xf3, 0xd9, 0x0, 0xe0, 0x2, 0x81, 0xc0, 0xa9, 0xdb, 0xed, 0x2e, 0x94, 0x2b, 0x5c, 0xe, 0xc4, 0xca, 0xca, 0x8a, 0x18, 0x8d, 0x46, 0x3, 0x0, 0xc0, 0x69, 0x1e, 0x4, 0x0, 0x90, 0x48, 0x24, 0x12, 0xe4, 0x38, 0xee, 0x41, 0xc2, 0x6f, 0x43, 0xe0, 0x38, 0xe, 0xfc, 0x7e, 0xbf, 0x10, 0x8b, 0xc5, 0xd6, 0x35, 0xd, 0x22, 0x9b, 0xcd, 0x7a, 0x96, 0x97, 0x97, 0x33, 0xf, 0xad, 0x7c, 0x29, 0x10, 0x9b, 0x9b, 0x9b, 0xef, 0x2e, 0x2e, 0x2e, 0x7e, 0xd5, 0x1c, 0x8, 0x0, 0x20, 0xe1, 0x70, 0x38, 0xfc, 0x98, 0xd5, 0x57, 0x2, 0xe1, 0x76, 0xbb, 0xf3, 0xa1, 0x50, 0xe8, 0x38, 0x9b, 0xcd, 0xfe, 0xa2, 0x9, 0x8, 0x0, 0x40, 0x2e, 0x2f, 0x2f, 0x7d, 0x4b, 0x4b, 0x4b, 0xb9, 0x4a, 0x54, 0x5f, 0x9, 0xc4, 0xd2, 0xd2, 0x92, 0xb4, 0xb7, 0xb7, 0xf7, 0x36, 0x97, 0xcb, 0x4d, 0x3d, 0x29, 0x8, 0x0, 0xe0, 0x42, 0xa1, 0xd0, 0x71, 0xb5, 0xc4, 0xdf, 0xb6, 0xc5, 0x93, 0xe, 0x4a, 0x0, 0x20, 0xa9, 0x54, 0xea, 0x37, 0xb7, 0xdb, 0x5d, 0xa8, 0xa6, 0x78, 0x39, 0x10, 0x6b, 0x6b, 0x6b, 0xf1, 0x64, 0x32, 0xb9, 0x5a, 0x55, 0x10, 0x0, 0xc0, 0x6d, 0x6c, 0x6c, 0x9c, 0x57, 0xbb, 0xfa, 0x25, 0x40, 0x14, 0x3, 0x81, 0x40, 0x34, 0x93, 0xc9, 0x2c, 0x57, 0x1c, 0x4, 0x0, 0x90, 0x58, 0x2c, 0xb6, 0x5e, 0xe9, 0xc1, 0x77, 0x1f, 0x10, 0x53, 0x53, 0x53, 0x52, 0xc5, 0x83, 0x14, 0x0, 0x70, 0x7e, 0xbf, 0x5f, 0xd0, 0x42, 0xf5, 0x95, 0x40, 0xf8, 0x7c, 0xbe, 0xcb, 0xa3, 0xa3, 0xa3, 0x3f, 0x1e, 0xbd, 0x1b, 0x0, 0x80, 0x1c, 0x1f, 0x1f, 0x87, 0xb4, 0x56, 0xfd, 0xaa, 0x5, 0x29, 0x51, 0x14, 0xbf, 0xf5, 0xf9, 0x7c, 0x97, 0x5a, 0xad, 0xbe, 0x12, 0x88, 0xf5, 0xf5, 0xf5, 0xd8, 0x83, 0x83, 0x54, 0xb5, 0x42, 0x8f, 0x66, 0x83, 0x94, 0xd6, 0xbd, 0x5f, 0xce, 0x7c, 0x38, 0x3c, 0x3c, 0xfc, 0xb3, 0x50, 0x28, 0xb8, 0xcb, 0x2, 0x1, 0x0, 0xdc, 0xf4, 0xf4, 0xf4, 0xfe, 0x73, 0x15, 0x2f, 0x17, 0xa4, 0x22, 0x91, 0x48, 0x50, 0xb5, 0x2d, 0x0, 0x80, 0x9b, 0x99, 0x99, 0x79, 0xfb, 0xdc, 0x1, 0xc8, 0x5, 0xa9, 0x44, 0x22, 0xf1, 0xfb, 0x9d, 0x10, 0x0, 0x80, 0x9b, 0x9d, 0x9d, 0xd, 0xea, 0x5, 0xc0, 0xad, 0xfd, 0x43, 0x1a, 0x0, 0xb8, 0xdb, 0x9a, 0xa9, 0x8f, 0xb6, 0xa4, 0x46, 0xa3, 0xa4, 0xb7, 0xd5, 0x37, 0xcf, 0xf3, 0x68, 0x75, 0x75, 0xf5, 0x4c, 0xee, 0x99, 0x1c, 0x80, 0x9c, 0x1e, 0xf7, 0xff, 0x16, 0x8b, 0x45, 0x50, 0x5, 0xa0, 0xb7, 0xb7, 0xb7, 0x85, 0x10, 0xa2, 0x2b, 0xf1, 0x84, 0x10, 0xd4, 0xdf, 0xdf, 0x6f, 0x57, 0x3, 0x80, 0x37, 0x18, 0xc, 0x5, 0x3d, 0x2, 0xa0, 0x69, 0x3a, 0x8b, 0x10, 0xe2, 0x4b, 0x2, 0xc0, 0x18, 0xf3, 0xc1, 0x60, 0x70, 0x47, 0x8f, 0x16, 0x38, 0x3a, 0x3a, 0x5a, 0x93, 0x5b, 0xc3, 0x7f, 0x64, 0x81, 0xba, 0xba, 0x3a, 0x49, 0x8f, 0x0, 0x1a, 0x1a, 0x1a, 0xd4, 0xcd, 0x0, 0x93, 0xc9, 0xa4, 0xcb, 0x21, 0xe8, 0x74, 0x3a, 0xd5, 0x1, 0xa0, 0x69, 0x5a, 0x77, 0x1d, 0x80, 0x31, 0x2e, 0x38, 0x9d, 0x4e, 0xb1, 0x66, 0x1, 0x30, 0xc, 0x23, 0x28, 0x3d, 0x93, 0x9b, 0x1, 0xb9, 0x9a, 0x6, 0x60, 0x36, 0x9b, 0x75, 0xd7, 0x1, 0x4a, 0x21, 0xa8, 0x26, 0x0, 0x94, 0xa, 0x41, 0xb2, 0x0, 0x18, 0x86, 0xc9, 0xe9, 0xd, 0x80, 0x52, 0x8, 0x92, 0x5, 0x60, 0xb1, 0x58, 0x74, 0x67, 0x1, 0xa5, 0x10, 0xa4, 0x4, 0x40, 0x77, 0x43, 0xd0, 0xe1, 0x70, 0xa8, 0x9f, 0x1, 0x14, 0x45, 0x1, 0x45, 0x51, 0x79, 0x3d, 0x1, 0x68, 0x6e, 0x6e, 0x4e, 0xaa, 0x6, 0x80, 0x10, 0x42, 0x6, 0x83, 0x41, 0x37, 0x36, 0x28, 0x15, 0x82, 0x6a, 0x2, 0x0, 0x4d, 0xd3, 0xa9, 0x52, 0xcf, 0x95, 0x0, 0xe8, 0x66, 0xe, 0x98, 0xcd, 0x66, 0xa1, 0x6c, 0x0, 0x7a, 0x5a, 0x8b, 0x59, 0x2c, 0x96, 0x64, 0xcd, 0x2, 0xb8, 0x2b, 0x4, 0xe9, 0xde, 0x2, 0x77, 0x85, 0xa0, 0x9a, 0xb0, 0x40, 0xa9, 0x10, 0xa4, 0x8, 0xc0, 0x64, 0x32, 0xe9, 0x6, 0x40, 0xa9, 0x10, 0x54, 0xaa, 0x3, 0x74, 0xf3, 0x16, 0x70, 0xb9, 0x5c, 0xe5, 0x3, 0xe8, 0xe9, 0xe9, 0x69, 0xd5, 0xc3, 0x66, 0x18, 0x63, 0x5c, 0x68, 0x6a, 0x6a, 0x12, 0xcb, 0x5, 0xa0, 0x9b, 0xd5, 0x38, 0x4d, 0xd3, 0x29, 0x8a, 0xa2, 0xa0, 0x2c, 0x0, 0x18, 0x63, 0x3e, 0x14, 0xa, 0xfd, 0x55, 0xb, 0x21, 0x48, 0xd1, 0x2, 0x7a, 0x59, 0x8d, 0xdf, 0x1b, 0x80, 0x1e, 0x56, 0xe3, 0x84, 0x10, 0x34, 0x30, 0x30, 0x60, 0xbb, 0xeb, 0x77, 0x46, 0x5, 0xef, 0x48, 0xcf, 0x4d, 0xec, 0x8d, 0x99, 0x5, 0xf5, 0xf5, 0xf5, 0xef, 0x46, 0x47, 0x47, 0xb, 0x2e, 0x97, 0xeb, 0xbc, 0x54, 0x8, 0x52, 0x4, 0xc0, 0x30, 0x8c, 0xf4, 0x5c, 0x4, 0x9b, 0x4c, 0xa6, 0xf4, 0xf8, 0xf8, 0xb8, 0xc8, 0xb2, 0x6c, 0x32, 0x9d, 0x4e, 0xff, 0xd4, 0xdd, 0xdd, 0x7d, 0x66, 0x34, 0x1a, 0x8b, 0xd7, 0x3, 0xfd, 0xae, 0x5b, 0x29, 0xb2, 0x57, 0x66, 0xb6, 0xb6, 0xb6, 0xde, 0xc4, 0xe3, 0xf1, 0x6f, 0xae, 0xaf, 0xc1, 0x28, 0x5d, 0x85, 0x79, 0x2, 0xc1, 0x60, 0xb5, 0x5a, 0xa3, 0xa3, 0xa3, 0xa3, 0x45, 0xab, 0xd5, 0x9a, 0x2a, 0x16, 0x8b, 0x8b, 0x6d, 0x6d, 0x6d, 0xef, 0xd5, 0x8a, 0x55, 0xd, 0x20, 0x91, 0x48, 0xbc, 0x3e, 0x38, 0x38, 0xf8, 0xda, 0x6e, 0xb7, 0xf7, 0x5f, 0x5c, 0x5c, 0xd4, 0x7b, 0xbd, 0xde, 0xbc, 0x20, 0x8, 0xcd, 0x85, 0x42, 0x81, 0xfe, 0xf0, 0xae, 0xac, 0x10, 0x98, 0x9b, 0xd5, 0xc5, 0x18, 0x17, 0x59, 0x96, 0x3d, 0x1d, 0x19, 0x19, 0x1, 0x96, 0x65, 0x5, 0x8a, 0xa2, 0x7e, 0x6c, 0x69, 0x69, 0x49, 0x3d, 0x44, 0xb0, 0x2a, 0x0, 0x1f, 0xcc, 0x74, 0x75, 0x41, 0xea, 0xfa, 0x7b, 0x32, 0x99, 0x64, 0x76, 0x77, 0x77, 0x5d, 0xe, 0x87, 0xa3, 0x5f, 0x14, 0xc5, 0x57, 0x57, 0x60, 0x5a, 0x8b, 0xc5, 0xa2, 0xf1, 0xbe, 0x50, 0x6e, 0xa, 0x66, 0x18, 0x26, 0x31, 0x36, 0x36, 0x96, 0x65, 0x59, 0x36, 0x29, 0x49, 0x92, 0xb7, 0xbd, 0xbd, 0xfd, 0x9f, 0x72, 0xda, 0xf9, 0xd1, 0x1, 0xa8, 0x1, 0x93, 0xcf, 0xe7, 0xa9, 0x93, 0x93, 0x13, 0x1b, 0x4d, 0xd3, 0x9f, 0xb, 0x82, 0x60, 0xf5, 0x7a, 0xbd, 0xd9, 0x54, 0x2a, 0xe5, 0xcc, 0x64, 0x32, 0xe, 0xb9, 0x6e, 0xb9, 0x16, 0x8c, 0x31, 0x2e, 0xda, 0x6c, 0xb6, 0xc8, 0xd0, 0xd0, 0x10, 0x65, 0xb3, 0xd9, 0x92, 0x95, 0xa8, 0x6e, 0xc5, 0x0, 0xa8, 0xe9, 0x96, 0x68, 0x34, 0x6a, 0xdd, 0xdf, 0xdf, 0x6f, 0x76, 0xb9, 0x5c, 0x9f, 0x89, 0xa2, 0x58, 0xbf, 0xb8, 0xb8, 0x8, 0x26, 0x93, 0x29, 0x3b, 0x3c, 0x3c, 0x8c, 0xed, 0x76, 0x7b, 0xd2, 0x68, 0x34, 0xfe, 0xd0, 0xd8, 0xd8, 0x98, 0xae, 0xb6, 0xe0, 0x8a, 0x1, 0x50, 0xb, 0xe6, 0xa9, 0x5, 0xbf, 0x9c, 0x97, 0xf3, 0xff, 0xf3, 0x2f, 0x6a, 0x82, 0x7f, 0xf6, 0x4e, 0xca, 0x1b, 0xf5, 0x0, 0x0, 0x0, 0x0, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

class TestPhysics2DMainLoop : public MainLoop {

	GDCLASS(TestPhysics2DMainLoop, MainLoop);

	RID circle_img;
	RID circle_shape;
	RID space;
	RID canvas;
	RID ray;
	RID ray_query;
	Transform2D view_xform;

	Vector2 ray_from, ray_to;

	struct BodyShapeData {

		RID image;
		RID shape;
	};

	BodyShapeData body_shape_data[8];

	void _create_body_shape_data() {
		VisualServer *vs = VisualServer::get_singleton();
		Physics2DServer *ps = Physics2DServer::get_singleton();

		// SEGMENT

		{

			PoolVector<uint8_t> pixels;
			pixels.resize(32 * 2 * 2);
			for (int i = 0; i < 2; i++) {

				for (int j = 0; j < 32; j++) {

					pixels.set(i * 32 * 2 + j * 2 + 0, (j == 0) ? 255 : 0);
					pixels.set(i * 32 * 2 + j * 2 + 1, 255);
				}
			}

			Ref<Image> image = memnew(Image(32, 2, 0, Image::FORMAT_LA8, pixels));

			body_shape_data[Physics2DServer::SHAPE_SEGMENT].image = vs->texture_create_from_image(image);

			RID segment_shape = ps->segment_shape_create();
			Rect2 sg(Point2(-16, 0), Point2(16, 0));
			ps->shape_set_data(segment_shape, sg);

			body_shape_data[Physics2DServer::SHAPE_SEGMENT].shape = segment_shape;
		}
		// CIRCLE

		{

			PoolVector<uint8_t> pixels;
			pixels.resize(32 * 32 * 2);
			for (int i = 0; i < 32; i++) {

				for (int j = 0; j < 32; j++) {

					bool black = Vector2(i - 16, j - 16).length_squared() < 16 * 16;

					pixels.set(i * 32 * 2 + j * 2 + 0, (i == 16 || j == 16) ? 255 : 0);
					pixels.set(i * 32 * 2 + j * 2 + 1, black ? 255 : 0);
				}
			}

			Ref<Image> image = memnew(Image(32, 32, 0, Image::FORMAT_LA8, pixels));

			body_shape_data[Physics2DServer::SHAPE_CIRCLE].image = vs->texture_create_from_image(image);

			RID circle_shape = ps->circle_shape_create();
			ps->shape_set_data(circle_shape, 16);

			body_shape_data[Physics2DServer::SHAPE_CIRCLE].shape = circle_shape;
		}

		// BOX

		{

			PoolVector<uint8_t> pixels;
			pixels.resize(32 * 32 * 2);
			for (int i = 0; i < 32; i++) {

				for (int j = 0; j < 32; j++) {

					bool black = i > 0 && i < 31 && j > 0 && j < 31;

					pixels.set(i * 32 * 2 + j * 2 + 0, black ? 0 : 255);
					pixels.set(i * 32 * 2 + j * 2 + 1, 255);
				}
			}

			Ref<Image> image = memnew(Image(32, 32, 0, Image::FORMAT_LA8, pixels));

			body_shape_data[Physics2DServer::SHAPE_RECTANGLE].image = vs->texture_create_from_image(image);

			RID rectangle_shape = ps->rectangle_shape_create();
			ps->shape_set_data(rectangle_shape, Vector2(16, 16));

			body_shape_data[Physics2DServer::SHAPE_RECTANGLE].shape = rectangle_shape;
		}

		// CAPSULE

		{

			PoolVector<uint8_t> pixels;
			pixels.resize(32 * 64 * 2);
			for (int i = 0; i < 64; i++) {

				for (int j = 0; j < 32; j++) {

					int si = i > 48 ? i - 32 : (i < 16 ? i : 16);
					bool black = Vector2(si - 16, j - 16).length_squared() < 16 * 16;

					pixels.set(i * 32 * 2 + j * 2 + 0, (i == 16 || j == 16 || i == 48) ? 255 : 0);
					pixels.set(i * 32 * 2 + j * 2 + 1, black ? 255 : 0);
				}
			}

			Ref<Image> image = memnew(Image(32, 64, 0, Image::FORMAT_LA8, pixels));

			body_shape_data[Physics2DServer::SHAPE_CAPSULE].image = vs->texture_create_from_image(image);

			RID capsule_shape = ps->capsule_shape_create();
			ps->shape_set_data(capsule_shape, Vector2(16, 32));

			body_shape_data[Physics2DServer::SHAPE_CAPSULE].shape = capsule_shape;
		}

		// CONVEX

		{

			Ref<Image> image = memnew(Image(convex_png));

			body_shape_data[Physics2DServer::SHAPE_CONVEX_POLYGON].image = vs->texture_create_from_image(image);

			RID convex_polygon_shape = ps->convex_polygon_shape_create();

			PoolVector<Vector2> arr;
			Point2 sb(32, 32);
			arr.push_back(Point2(20, 3) - sb);
			arr.push_back(Point2(58, 23) - sb);
			arr.push_back(Point2(55, 54) - sb);
			arr.push_back(Point2(27, 60) - sb);
			arr.push_back(Point2(5, 56) - sb);
			arr.push_back(Point2(4, 20) - sb);
			arr.push_back(Point2(11, 7) - sb);
			ps->shape_set_data(convex_polygon_shape, arr);

			body_shape_data[Physics2DServer::SHAPE_CONVEX_POLYGON].shape = convex_polygon_shape;
		}
	}

	void _do_ray_query() {

		/*
		Physics2DServer *ps = Physics2DServer::get_singleton();
		ps->query_intersection_segment(ray_query,ray_from,ray_to);
		*/
	}

protected:
	void input_event(const Ref<InputEvent> &p_event) {

		Ref<InputEventMouseButton> mb = p_event;

		if (mb.is_valid()) {

			if (mb->is_pressed()) {

				Point2 p(mb->get_position().x, mb->get_position().y);

				if (mb->get_button_index() == 1) {
					ray_to = p;
					_do_ray_query();
				} else if (mb->get_button_index() == 2) {
					ray_from = p;
					_do_ray_query();
				}
			}
		}

		Ref<InputEventMouseMotion> mm = p_event;

		if (mm.is_valid()) {

			Point2 p = mm->get_position();

			if (mm->get_button_mask() & BUTTON_MASK_LEFT) {
				ray_to = p;
				_do_ray_query();
			} else if (mm->get_button_mask() & BUTTON_MASK_RIGHT) {
				ray_from = p;
				_do_ray_query();
			}
		}
	}

	RID _add_body(Physics2DServer::ShapeType p_shape, const Transform2D &p_xform) {

		VisualServer *vs = VisualServer::get_singleton();
		Physics2DServer *ps = Physics2DServer::get_singleton();

		RID body = ps->body_create();
		ps->body_add_shape(body, body_shape_data[p_shape].shape);
		ps->body_set_space(body, space);
		ps->body_set_continuous_collision_detection_mode(body, Physics2DServer::CCD_MODE_CAST_SHAPE);
		ps->body_set_state(body, Physics2DServer::BODY_STATE_TRANSFORM, p_xform);

		//print_line("add body with xform: "+p_xform);
		RID sprite = vs->canvas_item_create();
		vs->canvas_item_set_parent(sprite, canvas);
		vs->canvas_item_set_transform(sprite, p_xform);
		Size2 imgsize(vs->texture_get_width(body_shape_data[p_shape].image), vs->texture_get_height(body_shape_data[p_shape].image));
		vs->canvas_item_add_texture_rect(sprite, Rect2(-imgsize / 2.0, imgsize), body_shape_data[p_shape].image);

		ps->body_set_force_integration_callback(body, this, "_body_moved", sprite);
		//RID q = ps->query_create(this,"_body_moved",sprite);
		//ps->query_body_state(q,body);

		return body;
	}

	void _add_plane(const Vector2 &p_normal, real_t p_d) {

		Physics2DServer *ps = Physics2DServer::get_singleton();

		Array arr;
		arr.push_back(p_normal);
		arr.push_back(p_d);

		RID plane = ps->line_shape_create();
		ps->shape_set_data(plane, arr);

		RID plane_body = ps->body_create();
		ps->body_set_mode(plane_body, Physics2DServer::BODY_MODE_STATIC);
		ps->body_set_space(plane_body, space);
		ps->body_add_shape(plane_body, plane);
	}

	void _add_concave(const Vector<Vector2> &p_points, const Transform2D &p_xform = Transform2D()) {

		Physics2DServer *ps = Physics2DServer::get_singleton();
		VisualServer *vs = VisualServer::get_singleton();

		RID concave = ps->concave_polygon_shape_create();
		ps->shape_set_data(concave, p_points);
		RID body = ps->body_create();
		ps->body_set_mode(body, Physics2DServer::BODY_MODE_STATIC);
		ps->body_set_space(body, space);
		ps->body_add_shape(body, concave);
		ps->body_set_state(body, Physics2DServer::BODY_STATE_TRANSFORM, p_xform);

		RID sprite = vs->canvas_item_create();
		vs->canvas_item_set_parent(sprite, canvas);
		vs->canvas_item_set_transform(sprite, p_xform);
		for (int i = 0; i < p_points.size(); i += 2) {
			vs->canvas_item_add_line(sprite, p_points[i], p_points[i + 1], Color(0, 0, 0), 2);
		}
	}

	void _body_moved(Object *p_state, RID p_sprite) {
		Physics2DDirectBodyState *state = (Physics2DDirectBodyState *)p_state;
		VisualServer::get_singleton()->canvas_item_set_transform(p_sprite, state->get_transform());
	}

	void _ray_query_callback(const RID &p_rid, ObjectID p_id, int p_shape, const Vector2 &p_point, const Vector2 &p_normal) {

		Vector2 ray_end;

		if (p_rid.is_valid()) {
			ray_end = p_point;
		} else {
			ray_end = ray_to;
		}

		VisualServer *vs = VisualServer::get_singleton();

		vs->canvas_item_clear(ray);
		vs->canvas_item_add_line(ray, ray_from, ray_end, p_rid.is_valid() ? Color(0, 1, 0.4) : Color(1, 0.4, 0), 2);
		if (p_rid.is_valid())
			vs->canvas_item_add_line(ray, ray_end, ray_end + p_normal * 20, p_rid.is_valid() ? Color(0, 1, 0.4) : Color(1, 0.4, 0), 2);
	}

	static void _bind_methods() {

		ClassDB::bind_method(D_METHOD("_body_moved"), &TestPhysics2DMainLoop::_body_moved);
		ClassDB::bind_method(D_METHOD("_ray_query_callback"), &TestPhysics2DMainLoop::_ray_query_callback);
	}

public:
	virtual void init() {

		VisualServer *vs = VisualServer::get_singleton();
		Physics2DServer *ps = Physics2DServer::get_singleton();

		space = ps->space_create();
		ps->space_set_active(space, true);
		ps->set_active(true);
		ps->area_set_param(space, Physics2DServer::AREA_PARAM_GRAVITY_VECTOR, Vector2(0, 1));
		ps->area_set_param(space, Physics2DServer::AREA_PARAM_GRAVITY, 98);

		{

			RID vp = vs->viewport_create();
			canvas = vs->canvas_create();

			Size2i screen_size = OS::get_singleton()->get_window_size();
			vs->viewport_attach_canvas(vp, canvas);
			vs->viewport_set_size(vp, screen_size.x, screen_size.y);
			vs->viewport_attach_to_screen(vp, Rect2(Vector2(), screen_size));
			vs->viewport_set_active(vp, true);

			Transform2D smaller;
			//smaller.scale(Vector2(0.6,0.6));
			//smaller.elements[2]=Vector2(100,0);

			//view_xform = smaller;
			vs->viewport_set_canvas_transform(vp, canvas, view_xform);
		}

		ray = vs->canvas_item_create();
		vs->canvas_item_set_parent(ray, canvas);
		//ray_query = ps->query_create(this,"_ray_query_callback",Variant());
		//ps->query_intersection(ray_query,space);

		_create_body_shape_data();

		for (int i = 0; i < 32; i++) {

			Physics2DServer::ShapeType types[4] = {
				Physics2DServer::SHAPE_CIRCLE,
				Physics2DServer::SHAPE_CAPSULE,
				Physics2DServer::SHAPE_RECTANGLE,
				Physics2DServer::SHAPE_CONVEX_POLYGON,

			};

			Physics2DServer::ShapeType type = types[i % 4];
			//type=Physics2DServer::SHAPE_SEGMENT;
			_add_body(type, Transform2D(i * 0.8, Point2(152 + i * 40, 100 - 40 * i)));
			/*
			if (i==0)
				ps->body_set_mode(b,Physics2DServer::BODY_MODE_STATIC);
			*/
		}

		//RID b= _add_body(Physics2DServer::SHAPE_CIRCLE,Transform2D(0,Point2(101,140)));
		//ps->body_set_mode(b,Physics2DServer::BODY_MODE_STATIC);

		Point2 prev;

		Vector<Point2> parr;
		for (int i = 0; i < 30; i++) {

			Point2 p(i * 60, Math::randf() * 70 + 340);
			if (i > 0) {
				parr.push_back(prev);
				parr.push_back(p);
			}
			prev = p;
		}

		_add_concave(parr);
		//_add_plane(Vector2(0.0,-1).normalized(),-300);
		//_add_plane(Vector2(1,0).normalized(),50);
		//_add_plane(Vector2(-1,0).normalized(),-600);
	}

	virtual bool idle(float p_time) {

		return false;
	}
	virtual void finish() {
	}

	TestPhysics2DMainLoop() {}
};

namespace TestPhysics2D {

MainLoop *test() {

	return memnew(TestPhysics2DMainLoop);
}
} // namespace TestPhysics2D
