#include "s3tp.h"

#define S3TP_DEFAULT_PORT 99

namespace horst {

	S3tpCallback4Horst::S3tpCallback4Horst()
	{
	
	}

	S3TPServer::S3TPServer():cbs{},channel{this->s3tp_cfg, this->cbs}
	{	
		// create channel instance and default the config
		this->s3tp_cfg.port = S3TP_DEFAULT_PORT; // default Local port to bind to
		this->s3tp_cfg.options = 0;
		this->s3tp_cfg.channel = 3; // This represents the virtual channel used by NanoLink
	}
	
	S3TPServer::~S3TPServer()
	{
		uv_poll_stop(&this->connection);
	}

	void S3TPServer::on_s3tp_event(uv_poll_t *handle,
		                  int status,
		                  int events) 
	{
		S3TPServer *s3tp_link_ref = ((S3TPServer *)handle->data);
		int current_events = s3tp_link_ref->channel.getActiveEvents();
	
		if(current_events & S3TP_ASYNC_EVENT_READ){
			s3tp_link_ref->channel.handleIncomingData(); 
		}
		if(current_events & S3TP_ASYNC_EVENT_WRITE){
			s3tp_link_ref->channel.handleOutgoingData();
		}
	
	
		current_events = s3tp_link_ref->channel.getActiveEvents();
		if(current_events & S3TP_ASYNC_EVENT_READ)
			current_events |= UV_READABLE;
		if(current_events & S3TP_ASYNC_EVENT_WRITE)
			current_events |= UV_WRITABLE;
			                  
		uv_poll_start(handle,
		              current_events | UV_DISCONNECT,
		              S3TPServer::on_s3tp_event);


	}


	int S3TPServer::initiate(uint8_t port, uv_loop_t *loop_ref) 
	{
		int r;
		int error = 0;
		int current_events = 0;

		int s3tp_fd;
		
		//Bind channel to S3TP daemon
		this->s3tp_cfg.port = port;
		this->channel.bind(error);
		if (error != 0) {
			LOG_WARN(std::string("Failed to bind to s3tp: " + std::string(strerror(-r))));
			return 1;
		}
	
		if (this->channel.accept() < 0) {
			LOG_WARN(std::string("Failed to register for s3tp events: " + std::string(strerror(-r))));
			return 1;
		}

		current_events = this->channel.getActiveEvents();
		s3tp_fd = this->channel.getSocket()->getFileDescriptor();

		// initialize the s3tp fd events polling object
		uv_poll_init(loop_ref, &this->connection,
		             s3tp_fd);
	
		// make `this` reachable in event loop callbacks.
		this->connection.data = this;
		uv_poll_start(&this->connection,
		              UV_READABLE | UV_WRITABLE | UV_DISCONNECT,
		              S3TPServer::on_s3tp_event);
		              
		return 0;
	}

	void S3tpCallback4Horst::onConnected(S3tpChannel &channel) 
	{
    		LOG_DEBUG("horst s3tp connection is active");
	}

	void S3tpCallback4Horst::onDisconnected(S3tpChannel &channel,
			      int error) 
	{
    		LOG_DEBUG("horst s3tp connection closed with error ");}

	void S3tpCallback4Horst::onDataReceived(S3tpChannel &channel,
			  char *data, size_t len) 
	{
		//Copy the data immediately, as the buffer is used by the connector and will be overwritten by the next read operation
		char * bufferCopy = new char[len + 1];
		memcpy(bufferCopy, data, len);
		bufferCopy[len] = '\0';
		std::cout << bufferCopy << std::endl;
		delete[] bufferCopy;
	}

	void S3tpCallback4Horst::onBufferFull(S3tpChannel &channel) 
	{
	}

	void S3tpCallback4Horst::onBufferEmpty(S3tpChannel &channel) 
	{
	}

	void S3tpCallback4Horst::onError(int error) 
	{
	// TODO: reconnect after some timer!
	}

} // horst
