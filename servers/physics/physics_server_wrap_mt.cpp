/*************************************************************************/
/*  physics_server_wrap_mt.cpp                                           */
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

#include "physics_server_wrap_mt.h"

#include "core/os/os.h"


void PhysicsServerWrapMT::thread_exit() {

	exit = true;
}

void PhysicsServerWrapMT::thread_step(real_t p_delta) {

	physics_server->step(p_delta);
	step_sem->post();
}

void PhysicsServerWrapMT::_thread_callback(void *_instance) {

	PhysicsServerWrapMT *vsmt = reinterpret_cast<PhysicsServerWrapMT *>(_instance);

	vsmt->thread_loop();
}

void PhysicsServerWrapMT::thread_loop() {

	server_thread = Thread::get_caller_id();

	OS::get_singleton()->make_rendering_thread();

	physics_server->init();

	exit = false;
	step_thread_up = true;
	while (!exit) {
		// flush commands one by one, until exit is requested
		command_queue.wait_and_flush_one();
	}

	command_queue.flush_all(); // flush all

	physics_server->finish();
}

/* EVENT QUEUING */

void PhysicsServerWrapMT::step(real_t p_step) {

	if (create_thread) {

		command_queue.push(this, &PhysicsServerWrapMT::thread_step, p_step);
	} else {

		command_queue.flush_all(); //flush all pending from other threads
		physics_server->step(p_step);
	}
}

void PhysicsServerWrapMT::sync() {

	if (step_sem) {
		if (first_frame)
			first_frame = false;
		else
			step_sem->wait(); //must not wait if a step was not issued
	}
	physics_server->sync();
}

void PhysicsServerWrapMT::flush_queries() {

	physics_server->flush_queries();
}

void PhysicsServerWrapMT::end_sync() {

	physics_server->end_sync();
}

void PhysicsServerWrapMT::init() {

	if (create_thread) {

		step_sem = Semaphore::create();
		//OS::get_singleton()->release_rendering_thread();
		if (create_thread) {
			thread = Thread::create(_thread_callback, this);
		}
		while (!step_thread_up) {
			OS::get_singleton()->delay_usec(1000);
		}
	} else {

		physics_server->init();
	}
}

void PhysicsServerWrapMT::finish() {

	if (thread) {

		command_queue.push(this, &PhysicsServerWrapMT::thread_exit);
		Thread::wait_to_finish(thread);
		memdelete(thread);

		thread = NULL;
	} else {
		physics_server->finish();
	}

	line_shape_free_cached_ids();
	ray_shape_free_cached_ids();
	segment_shape_free_cached_ids();
	circle_shape_free_cached_ids();
	rectangle_shape_free_cached_ids();
	convex_polygon_shape_free_cached_ids();
	concave_polygon_shape_free_cached_ids();

	space_free_cached_ids();
	area_free_cached_ids();
	body_free_cached_ids();

	if (step_sem)
		memdelete(step_sem);
}

PhysicsServerWrapMT::PhysicsServerWrapMT(PhysicsServer *p_contained, bool p_create_thread) :
		command_queue(p_create_thread) {

	physics_server = p_contained;
	create_thread = p_create_thread;
	thread = NULL;
	step_sem = NULL;
	step_pending = 0;
	step_thread_up = false;
	alloc_mutex = Mutex::create();

	pool_max_size = GLOBAL_GET("memory/limits/multithreaded_server/rid_pool_prealloc");

	if (!p_create_thread) {
		server_thread = Thread::get_caller_id();
	} else {
		server_thread = 0;
	}

	main_thread = Thread::get_caller_id();
	first_frame = true;
}

PhysicsServerWrapMT::~PhysicsServerWrapMT() {

	memdelete(physics_server);
	memdelete(alloc_mutex);
	//finish();
}
