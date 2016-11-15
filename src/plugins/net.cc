#include <functional>
#include <string>
#include <cstring>
#include <queue>
#include "plugin.h"
#include "include/poe.h"
#include "include/v8.h"
#include "plugins/net.h"
// #include "v8ToCbor.h"

struct response_t {
	uint32_t connection;
	std::string message;
};

std::queue<response_t> responseQueue;

void ResponseSendCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 2) return;
  HandleScope scope(args.GetIsolate());

  uint32_t connection = args[0]->Uint32Value();
  v8::String::Utf8Value msg(args[1]);

  // v8::Handle<v8::Value> value = {args[1]};
  response_t response = {
  	connection,
  	std::string(* msg)
  	// Blank::v8ValueToCbor(args[1])
  };

  responseQueue.push(response);
}

namespace Blank {

	void Net::ExtendShellTemplate(v8::Isolate * isolate, v8::Local<v8::ObjectTemplate> shell) {
		shell->Set(
		  String::NewFromUtf8(isolate, "send"),
		  FunctionTemplate::New(isolate, ResponseSendCallback)
		);
	}

	void Net::OnInit() {
		_server = new Poe::Server();

		_server->OnConnect([](Poe::Server * server, Poe::Connection * conn){
			// TODO Set permissions
			conn->SetCodec(new Poe::Codec());
		});

		_server->OnDisconnect([](Poe::Server * server, Poe::Connection * conn){
			delete conn->GetCodec();
		});

		_server->Start(_port);
	}

	void Net::OnTick() {
		if (! _server->IsRunning()) {
			this->Finish();
			return;
		}

		// TODO Write responses from queue
		while (! responseQueue.empty()) {			
			response_t response = responseQueue.front();

			int connId = response.connection;
			std::string str = response.message;

			responseQueue.pop();

			if (! _server->HasConn(connId)) continue;

			Poe::Connection * conn = _server->GetConn(connId);

			if (conn->IsClosed()) continue;

			Poe::Pack * pack = new Poe::Pack();
			pack->data = (char *) malloc(sizeof(char) * str.length());
			pack->size = str.length();
			memcpy(pack->data, str.c_str(), str.length());

			// TODO Change connection
			
			conn->Write((void *) pack);
			delete pack;
		}

		// TODO get shell and env
		if (! _server->HasRequest()) return;

		v8::Isolate * isolate = _env->GetIsolate();
		v8::Local<v8::Object> shell = isolate
			->GetCurrentContext()
			->Global()
			->ToObject();

		v8::Handle<v8::Value> onMessage = shell->Get(String::NewFromUtf8(isolate, "onMessage"));

		if (! onMessage->IsFunction()) {
			// TODO Produce error and exit
			_server->Stop();
			Finish();
			_env->Error("Method shell.onMessage is not a function");
			return;
		}

		Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(onMessage);
		v8::TryCatch tryCatch;

		while (_server->HasRequest()) {
			Poe::Request * req = _server->GetRequest();
			Poe::Pack * pack = reinterpret_cast<Poe::Pack *>(req->data);

			// TODO parse message with CBOR decoder
			std::string message(pack->data, pack->size);
			if (message.compare("exit") == 0) {
				_server->Stop();
				Finish();
				return;
			}

			Handle<Value> args[2] = {
				v8::Integer::New(isolate, req->conn->id),
				v8::String::NewFromUtf8(isolate, message.c_str())
			};

			func->Call(shell, 2, args);
			delete req;

			if (tryCatch.HasCaught()) {
				v8::String::Utf8Value error(tryCatch.StackTrace());
                _server->Stop();
                Finish();
                _env->Error(* error);
                return;
			}
		}
	}

	void Net::SetPort(int port) {
		_port = port;
	}

	int Net::GetPort() {
		return _port;
	}
}