#include "database.h"
#include <iostream>
#include <queue>
#include "posix_message_queues.h"
#include "logger_builder.h"
#include "logger_builder_concrete.h"

using namespace std;


static string fname(const string& read_path) {

	size_t first_divider = read_path.find('\\');
	size_t ed;

	do {
		ed = first_divider;
		first_divider = read_path.find('\\', ed + 1);
		} while (first_divider != string::npos);
		
		string tag = read_path.substr(ed + 1);

		return tag;
	}
	

int main(int argc, char* argv[]) {

	logger_builder* builder = new logger_builder_concrete();
	logger* _logger = builder->add_stream("console", logger::severity::debug)->construct();

	message_queues* message_queue;
    try {
        message_queue = new message_queues("tested", message_queues::point_type::Client);
    }
    catch(const logic_error& err){
        throw err;
    }

	string selected_option, input;

	_logger->log("\n\n=================== Welcome to the program menu! ===================", logger::severity::warning);

	while (selected_option != "4") {

		_logger->log("\n=================== Please, select one of the options provided: ====\n*1. Run options in file.\n*2. Instantly enter an option.\n*3. Reboot database.\n*4. Quit.\n\n", logger::severity::warning);
		cin.clear();
		getline(cin, selected_option);

		if (selected_option == "1") {

			_logger->log("\n========== Please, enter your file's path. ==========\n\n", logger::severity::warning);
			cin.clear();
			getline(cin, input);

			if (input.empty()) {

				_logger->log("\n======= DIALOGUE ERROR no.1: Entered path is empty. =======\n", logger::severity::warning);
				continue;
			}

			try {
				//file_parse_options(input);
				string output = "#### Now the system is parsing this file: ";
				output += fname(input);
				output += " ####";
				_logger->log(output, logger::severity::warning);

				ifstream fin;
				fin.open(input);
				if (!fin.is_open()) {
					throw logic_error("FILE PARSING ERROR 1: Unable to open given path!");
				}

				string s1, s2, s3;

				while (getline(fin, s1)) {

					if ((s1.find("insert: ") == 0) || (s1.find("update key: ") == 0)) {

						if (!getline(fin, s2)) {
							fin.close();
							throw logic_error("FILE PARSING ERROR 2: Invalid option string format,enter your key!");
						}

						if (!getline(fin, s3)) {
							fin.close();
							throw logic_error("FILE PARSING ERROR 2: Invalid option string format,enter your value!");
						}
					}

					else if (s1.find("read key: ") == 0 || s1.find("read range: ") == 0 || s1.find("remove: ") == 0) {

						if (!getline(fin, s2)) {
							fin.close();
							throw logic_error("FILE PARSING ERROR 2: Invalid option string format,enter your key!");
						}
					}

					else if (s1.find("add pool: ") == 0 || s1.find("delete pool: ") == 0 || s1.find("add scheme: ") == 0 || s1.find("delete scheme: ") == 0 || s1.find("add collection: ") == 0 || s1.find("delete collection: ") == 0 || s1.find("reboot database:") == 0) {

						try {
							//######################## option_parser(s1, s2, s3); ################################
							//[option] [key] [value]
							string my_message = '{' + s1 + '}' + ' ' + '{' + s2 + '}' + ' ' + '{' + s3 + '}';

							//���������� � �� ������
                            try {
                                message_queue->send_message(my_message);
                                _logger->log(message_queue->read_message(),logger::severity::debug);
                            }
                            catch(const logic_error& err){
                                throw err;
                            }

						}
						catch (const logic_error& err) {
							throw err;
						}

						continue;
					}

					else {
						fin.close();
						throw logic_error("FILE PARSING ERROR 3: You have entered a option which do not exist or do not support adviced format.");
					}

					try {

						//#####################option_parser(s1, s2, s3); #######################3

						//[option] [key] [value]
						string my_message = '{' + s1 + '}' + ' ' + '{' + s2 + '}' + ' ' + '{' + s3 + '}';

						//���������� � �� ������
                        try{
						message_queue->send_message(my_message);
                        _logger->log(message_queue->read_message(),logger::severity::debug);
                        }
                        catch(const logic_error& err){
                            throw err;
                        }
					}

					catch (const logic_error& err) {
						fin.close();
						throw err;
					}
				}

				fin.close();
				_logger->log("#### The system finished parsing your file successfully! ####", logger::severity::warning);
				_logger->log("\n===== Entered path has been processed! =====\n\n======== Please,select one of the options provided: ========\n*1. Continue.\n*2. Quit.\n\n", logger::severity::warning);

				string taken_option;
				cin.clear();
				getline(cin, taken_option);

				if (taken_option == "1") {
					continue;
				}

				else if (taken_option == "2") {

					_logger->log("\n===== The program has been interupted! =====", logger::severity::warning);
					break;
				}

				else {
					_logger->log("\n======== DIALOGUE ERROR no.2: Invalid path.Try again! ========\n", logger::severity::warning);
					continue;
				}

			}
			catch (const logic_error& err) {

				string taken_option, output;
				output += err.what();
				output += "\n";
				_logger->log(output, logger::severity::debug);
				_logger->log("\n========== DIALOGUE ERROR no.3: Parsing option failed. ==========\n\n======== Please,select one of the options provided: ========\n*1. Continue.\n*2. Reboot and continue.\n*3. Quit.\n", logger::severity::debug);
				cin.clear();
				getline(cin, taken_option);

				if (taken_option == "1") {
					continue;
				}
				else if (taken_option == "2") {
					_logger->log("\n=========== Continue and reboot have been started ===========", logger::severity::debug);
					//reboot_database();
                    string e1 = "reboot database:";
                    string rbd = '{' + e1 + '}' + ' ' + '{' + '}' + ' ' + '{' + '}';

                    try{
                        message_queue->send_message(rbd);
                        _logger->log(message_queue->read_message(),logger::severity::debug);
                    }
                    catch(const logic_error& err){
                        throw err;
                    }
					continue;
				}
				else if (taken_option == "3") {
					_logger->log("\n===== The program has been interupted! =====", logger::severity::debug);
                    //break;
				}
				else {
					_logger->log("\n======== DIALOGUE ERROR no.2: Invalid path.Try again! ========\n", logger::severity::debug);
					continue;
				}
			}
		}

		else if (selected_option == "2") {

			string taken_option, option_line, key_line, value_line;

			_logger->log("\n=================== Please, select one of the options provided: ====\n*1. Do an insertion.\n*2. Read the key.\n*3. Read the range.\n*4. Update the key.\n*5. Remove.\n*6. Add the new pool.\n*7. Delete the chosen pool.\n*8. Add the new scheme.\n*9. Delete the chosen scheme.\n*10. Add the new collection.\n*11. Delete the chosen collection.\n", logger::severity::debug);
			_logger->log("\n=================== Note, after selecting a particular option you will be given a specific template for using it! ===================", logger::severity::information);

			cin.clear();
			getline(cin, taken_option);

			if (taken_option == "1" || taken_option == "2" || taken_option == "3" || taken_option == "4" || taken_option == "5" || taken_option == "6" || taken_option == "7" || taken_option == "8" || taken_option == "9" || taken_option == "10" || taken_option == "11") {

				if (taken_option == "1") {
					_logger->log("\n*Template: insert: [nameof_pool] [nameof_scheme] [nameof_collection]\nkey: [consumer_id,delivery_id]\nvalue: [delivery_description] [consumer_name] [consumer_surname] [consumer_patronymic] [consumer_email] [consumer_phone_number] [consumer_adress] [consumer_comments] [delivery_date_time]\n", logger::severity::debug);
				}
				else if (taken_option == "2") {
					_logger->log("\n*Template: read key: [nameof_pool] [nameof_scheme] [nameof_collection]\nkey: [consumer_id,delivery_id]\n", logger::severity::debug);
				}
				else if (taken_option == "3") {
					_logger->log("\n*Template: read range: [nameof_pool] [nameof_scheme] [nameof_collection]\nkeys: [consumer_id, delivery_id] [consumer_id, delivery_id] \n", logger::severity::debug);
				}
				else if (taken_option == "4") {
					_logger->log("\n*Template: update key: [nameof_pool] [nameof_scheme] [nameof_collection]\nkey: [consumer_id,delivery_id]\nvalue: [delivery_description] [consumer_name] [consumer_surname] [consumer_patronymic] [consumer_email] [consumer_phone_number] [consumer_adress] [consumer_comments] [delivery_date_time]\n", logger::severity::debug);
				}
				else if (taken_option == "5") {
					_logger->log("\n*Template: remove: [nameof_pool] [nameof_scheme] [nameof_collection]\nkey: [consumer_id,delivery_id]\n", logger::severity::debug);
				}
				else if (taken_option == "6") {
					_logger->log("\n*Template: add pool: [nameof_pool] ['global heap' / 'border descriptors' / 'buddies' / 'sorted list' ,size(optional),'best'/'worst'/'first'(optional)]\n", logger::severity::debug);
				}
				else if (taken_option == "7") {
					_logger->log("\n*Template: delete pool: [nameof_pool]\n", logger::severity::debug);
				}
				else if (taken_option == "8") {
					_logger->log("\n*Template: add scheme: [nameof_pool] [nameof_scheme]\n", logger::severity::debug);
				}
				else if (taken_option == "9") {
					_logger->log("\n*Template: delete scheme: [nameof_pool] [nameof_scheme]\n", logger::severity::debug);
				}
				else if (taken_option == "10") {
					_logger->log("\n*Template: add collection: [nameof_pool] [nameof_scheme] [nameof_collection] ['avl tree'/'red black tree'/'splay tree']\n", logger::severity::debug);
				}
				else if (taken_option == "11") {
					_logger->log("\n*Template: delete collection: [nameof_pool] [nameof_scheme] [nameof_collection]\n", logger::severity::debug);
				}
				cin.clear();
				getline(cin, option_line);

				if (taken_option == "1" || taken_option == "4") {

					_logger->log("\n\n====> Enter the key:\n", logger::severity::warning);

					cin.clear();
					getline(cin, key_line);

					_logger->log("\n\n====> Enter a value:\n", logger::severity::warning);

					cin.clear();
					getline(cin, value_line);
				}

				else if (taken_option == "2" || taken_option == "3" || taken_option == "5") {

					_logger->log("\n\n====> Enter the key:\n", logger::severity::warning);

					cin.clear();
					getline(cin, key_line);

					value_line = "";
				}

				else {
					key_line = value_line = "";
				}

				try {
					//###################### option_parser(option_line, key_line, value_line); #####################################
					
					//[option] [key] [value]
					string my_message = '{' + option_line + '}' + ' ' + '{' + key_line + '}' + ' ' + '{' + value_line + '}';
					
					//���������� � �� ������
                    try{
					    message_queue->send_message(my_message);
                        _logger->log(message_queue->read_message(),logger::severity::debug);
                    }
                    catch(const logic_error& err){
                        throw err;
                    }


					_logger->log("\n============================ *Session* ============================\n ", logger::severity::debug);
					continue;
				}
				catch (const logic_error& err) {

					string output = "\n\n";
					output += err.what();
					output += "\n";
					_logger->log(output, logger::severity::warning);
					_logger->log("\n============ An unexpected error occurred while executing an option. ============\n **** Try again! **** \n", logger::severity::debug);
					continue;
				}
			}
			else {

				_logger->log("n======== DIALOGUE ERROR no.4: Invalid option.Try again! ========\n", logger::severity::debug);
				continue;
			}
		}

		else if (selected_option == "3") {

			_logger->log("\n============= Database reboot has been started! =============\n", logger::severity::debug);
			//reboot_database();
            string e1 = "reboot database:";
            string rbd = '{' + e1 + '}' + ' ' + '{' + '}' + ' ' + '{' + '}';

            try{
                message_queue->send_message(rbd);
                _logger->log(message_queue->read_message(),logger::severity::debug);
            }
            catch(const logic_error& err){
                throw err;
            }
			continue;
		}

		else if (selected_option == "4") {

			_logger->log("\n============= Thank you for using this app. =============\n", logger::severity::debug);
			 break;
		}
		else {

			_logger->log("\n============= DIALOGUE ERROR no.4: Invalid action has been entered. Try another time! =============\n", logger::severity::debug);
			 break;
		}


	}

	_logger->log("\n============= Now the dialogue was quit. ================\n\n", logger::severity::debug);





	delete message_queue;
	return 0;
}