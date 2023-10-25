#include "database.h"
#include <iostream>
#include <queue>
#include "posix_message_queues.h"
#include "logger_builder.h"
#include "logger_builder_concrete.h"

using namespace std;

int main(int argc, char* argv[]) {

    logger_builder* builder = new logger_builder_concrete();

    logger* logger1 = builder->add_stream("console", logger::severity::debug)->construct();

    auto* db = new database(logger1);

	message_queues* message_queue;


    //bool quitness = true;
    try {
        message_queue = new message_queues("tested", message_queues::point_type::Server);
    }
    catch(const logic_error& err){
        throw err;
    }

    try {
        message_queue->listen([db,message_queue](string message) {


                try {
                    db->option_parser(message);
                    message_queue->send_message("$$$ SUCCESS $$$");
                }
                catch (const logic_error &err) {

                    string ferr = "FATAL DIALOGUE ERROR: ";
                    ferr += err.what();
                    //throw logic_error(ferr);
                    message_queue->send_message(ferr);
                }
        });

        while(true) std::this_thread::sleep_for (std::chrono::seconds(1));
    }
    catch(const logic_error& err){
       throw err;
    }

	//delete message_queue;

    //delete db;
    //delete logger1;
    //delete builder;
}