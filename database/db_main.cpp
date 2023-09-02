#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\database.h"
#include <iostream>
#include <queue>

//#include <semaphore>
//#include "server_template.h"
//#include "messages_queue_class.h"

int main(int argc, char* argv[]) {

	logger_builder* builder = new logger_builder_concrete();

	logger* logger1 = builder->add_stream("console", logger::severity::debug)->construct();

	auto* db = new database(logger1);


	try {
		db->dialogue_with_user();
	}
	catch (const logic_error& err) {
		string ferr = "FATAL DIALOGUE ERROR: ";
		ferr += err.what();
		throw logic_error(ferr);
	}


	delete db;
	delete logger1;
	delete builder;
}