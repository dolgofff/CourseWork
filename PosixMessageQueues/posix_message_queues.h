#ifndef POSIX_MESSAGE_QUEUES
#define POSIX_MESSAGE_QUEUES

#include <iostream>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <mqueue.h>
#include <fcntl.h>
#include <thread>
using namespace std;
//êîìïèëèðîâàòü ñ: g++ --std=c++11 -O3 main.cpp -lrt -lpthread
class message_queues {

public:
//Òèï òî÷êè, ê êîòîðîé îáðàùàåìñÿ
    enum point_type {
        Client,
        Server
    };

private:

    point_type _type;

    mqd_t _mqd1;
    mqd_t _mqd2; //äåñêðèïòîðû

    unsigned int _prio = 1;

    struct mq_attr _attr1;
    struct mq_attr _attr2;

public:

    //Ñîçäàíèå òî÷êè: message_queues *new_queue("message",message_queues::EndpointType::Server/Client);

    //êîíñòðóêòîð ñîçäà¸ò íîâóþ î÷åðåäü ñîîáùåíèé ñî ñâîèì id
    message_queues(string id, point_type type) : _type(type) {

        string name1 = "/" + id + "_1";
        string name2 = "/" + id + "_2";

        if (type == point_type::Server) {

            // Êîíå÷íàÿ òî÷êà ðàáîòàåò êàê ñåðâåð => ÷èñòèì î÷åðåäü è ñîçäà¸ì íîâóþ
            mq_unlink(name1.c_str());
            mq_unlink(name2.c_str());
        }

        _mqd1 = mq_open(name1.c_str(), O_RDWR | O_CREAT, 0664, 0);
        _mqd2 = mq_open(name2.c_str(), O_RDWR | O_CREAT, 0664, 0);

        if ((_mqd1 == -1) || (_mqd2 == -1)) {

            throw logic_error("Cannot open message queue.");
        }

        mq_getattr(_mqd1, &_attr1);
        mq_getattr(_mqd2, &_attr2);
    }

    virtual ~message_queues() {

        mq_close(_mqd1);
        mq_close(_mqd2);
    }


    // Ïðèíèìàåì âõîäÿùèå ñîîáùåíèÿ è ïåðåäà¸ì èõ â ëÿìáäó (àñèíõðîííûé ìåòîä, ðàáîòàþùèé ñ îäíèì ïîòîêîì)
    void listen(function<void(string)> cb)
    {
        if (_type == point_type::Client) {

            throw logic_error("Cannot call listen() on client.");
        }

        thread t([this, cb]() {

            while (true) {

                // ïî äåôîëòó 8192 áàéòà
                char* buffer = new char[9000];
                memset(buffer, '\0', 9000);

                if (mq_receive(_mqd2, buffer, 9000, &_prio) == -1) {

                    throw logic_error("Cannot read the message.");
                }

                cb(string(buffer));
                delete[] buffer;
            }
        });

        t.detach();
    }

    // Îòïðàâëÿåì ñîîáùåíèå ÷åðåç î÷åðåäü
    void send_message(string data) {

        int result;

        if (_type == point_type::Server){

            result = mq_send(_mqd1, data.c_str(), strlen(data.c_str()), _prio);
        }

        else {

            result = mq_send(_mqd2, data.c_str(), strlen(data.c_str()), _prio);
        }
        if (result == -1) {

            throw logic_error("Cannot send the message.");
        }
    }


    //×èòàåì è îòïðàâëÿåì 1 ñîîáùåíèå ñèíõðîííî èç î÷åðåäè ñîîáùåíèé ( ñèíõðîííûé ìåòîä, âûïîëíÿáùèé áëîêèðóâêó â òîì ñëó÷àå, åñëè åìó íå÷åãî ÷èòàòü äî òåõ ïîð, ïîêà ñîîáùåíèå íå ñòàíåò äîñòóïíûì.
    string read_message() {

        //ïî äåôîëòó 8192 áàéòà
        char* buffer = new char[9000];
        memset(buffer, '\0', 9000);

        if (mq_receive(_mqd1, buffer, 9000, &_prio) == -1) {

            throw logic_error("Cannot read the message.");
        }

        string message(buffer);

        delete[] buffer;

        return message;
    }

};


#endif // POSIX_MESSAGE_QUEUES

/*
int main(int argc, char *argv[]) {
	if(argc < 2) {
	std::cout << "Usage: ./main.cpp client|server" << std::endl;
		return 1;
	}

	message_queue *new_queue;

	if(std::string(argv[1]) == "server") {
		new_queue= new message_queue("example", message_queue::point_type::Server);
		new_queue->listen([new_queue](std::string message){
			std::cout << "Received: " << message << std::endl;
			new_queue->send_message("Hello from the server!");
		});
		while(true) std::this_thread::sleep_for (std::chrono::seconds(1));
	} else if(std::string(argv[1]) == "client") {
		new_queue = new message_queue("example", MQ::point_type::Client);
		std::string data;
		std::cout << "Write message to send and press ENTER:" << std::endl;
		getline(std::cin, data);
		new_queue->send_message(data);
		std::cout << "Received: " << new_queue->read_message() << std::endl;
	}

	delete new_queue;
	return 0;
}



*/