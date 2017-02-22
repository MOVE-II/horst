#include "satellite.h"

#include <iostream>
#include <memory>

#include "action/action.h"
#include "client/tcp_client.h"
#include "logger.h"
#include "util.h"

namespace horst {

Satellite::Satellite(const arguments &args)
	:
	args{args},
	loop{},
	dbus{this},
	tcp_server{this},
	next_id{0} {

	uv_loop_init(&this->loop);
}


Satellite::~Satellite() {
	LOG_DEBUG("[satellite] destroying...");
	uv_loop_close(&this->loop);
}


int Satellite::run() {
	LOG_INFO("[satellite] starting up connections...");
	int ret;

	if (this->tcp_server.listen(this->args.port)) {
		LOG_ERROR(3, "[satellite] failed to set up tcp socket.");
		return 1;
	}

	if (this->dbus.connect()) {
		LOG_ERROR(4, "[satellite] failed to listen on dbus.");
		return 1;
	}

	if (this->listen_s3tp(this->args.port)) {
		LOG_ERROR(5, "[satellite] failed to listen on s3tp.");
		return 1;
	}

	// let the event loop run forever.
	LOG_INFO("[satellite] Starting event loop");
	ret = uv_run(&this->loop, UV_RUN_DEFAULT);
	LOG_INFO("[satellite] Stopping event loop");
	return ret;
}


int Satellite::listen_s3tp(int /*port*/) {
	// TODO register s3tp_connection to event loop.
	return 0;
}

std::string Satellite::get_scripts_path() {
	return args.scripts;
}


uv_loop_t *Satellite::get_loop() {
	return &this->loop;
}


id_t Satellite::add_client(std::unique_ptr<Client> &&client) {
	this->clients.emplace(this->next_id, std::move(client));
	return this->next_id++;
}


id_t Satellite::add_action(std::unique_ptr<Action> &&action) {
	this->actions.emplace(this->next_id, std::move(action));
	return this->next_id++;
}


Client *Satellite::get_client(id_t id) {
	auto loc = this->clients.find(id);
	if (loc == std::end(this->clients)) {
		return nullptr;
	} else {
		return loc->second.get();
	}
}


Action *Satellite::get_action(id_t id) {
	auto loc = this->actions.find(id);
	if (loc == std::end(this->actions)) {
		return nullptr;
	} else {
		return loc->second.get();
	}
}

const Procedure *Satellite::get_procedure(const std::string &name) const {
	return this->procedures.get_procedure(name);
}

State *Satellite::get_state() {
	return &this->current_state;
}


void Satellite::remove_action(id_t id) {
	auto pos = this->actions.find(id);
	if (pos != std::end(this->actions)) {
		this->actions.erase(pos);
	} else {
		LOG_WARN("[satellite] Attempt to remove an unknown action");
	}
}


void Satellite::remove_client(id_t id) {
	auto pos = this->clients.find(id);
	if (pos != std::end(this->clients)) {
		this->clients.erase(pos);
	} else {
		LOG_WARN("[satellite] Attempt to remove an unknown client");
	}
}


void Satellite::on_event(std::shared_ptr<Event> &&event) {
	// called for each event the satellite receives
	// it may come from earth or any other subsystem

	// if the event is a fact, update the current state
	if (event->is_fact()) {
		event->update(this->current_state);
	}

	// create the target state as a copy of the current state
	State target_state = this->current_state.copy();

	// it the event is a request (i.e. not a fact),
	// update it in the target state
	if (not event->is_fact()) {
		event->update(target_state);
	}

	// determine the actions needed to reach the target state
	auto actions = this->current_state.transform_to(target_state);

	for (auto &action_m : actions) {
		// store the action
		id_t id = this->add_action(std::move(action_m));

		// and fetch its new location
		Action *action = this->get_action(id);
		LOG_INFO("[action] run #" + std::to_string(id) + ": " + action->describe());

		// perform the action, this may just enqueue it in the event loop.
		// the callback is executed when the action is done.
		action->perform(
			this,
			[this, id] (bool success, Action *) {
				if (not success) {
					LOG_WARN("[action] #" + std::to_string(id) + " failed!");
				}
				else {
					LOG_INFO("[action] #" + std::to_string(id) + " succeeded");
				}

				// tell all systems that this action was finished.
				this->dbus.emit_action_done(success, id);

				// the last action in here must be the free of the
				// action from memory:
				this->remove_action(id);
			}
		);
	}
}


} // horst
