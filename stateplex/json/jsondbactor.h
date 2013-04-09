#include <stateplex/core/callbackmessage.h>
#include <stateplex/core/actor.h>

class JsonDbActor : public Stateplex::Actor
{



public:

	JsonDbActor();
	~JsonDbActor();

	class JsonMessage: public Stateplex::CallbackMessage<CallbackActor, CallbackMessage> {
		JsonMessageType mType;
		String *mParameter;
		JsonItem *mResult;

	public:
		enum JsonMessageType {
			JSON_MESSAGE_TYPE_GET,
			JSON_MESSAGE_TYPE_SET
		};

		JsonMessage();
		~JsonMessage();

	protected:
		void handle(Actor *sender, Actor *receiver);
	};


};

