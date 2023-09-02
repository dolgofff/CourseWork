#ifndef CWW_DATABASE_H
#define CWW_DATABASE_H
#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\string_flyweight.h"
#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\avl_tree.h"
#include <iostream>
#include <ctime>
#include <queue>
#include <fstream>
#include <string.h>


class database final
{

public:

	//#### РЕАЛИЗОВАННЫЙ ФУНКЦИОНАЛ ####// 

	enum class trees {
		red_black_tree,
		avl_tree
	};

	enum class allocators {
		global_heap_allocator,
		border_descriptors_allocator
	};

	enum class status {
		neutral,      // для ГК
		first_available,
		best_available,
		worst_available
	};

private:

	//#### ОПИСАНИЕ ТИПА ДАННЫХ ####//

//уникальный ключ
	struct delivery_data {
		int consumer_id;
		int delivery_id;
	};

	//значение, которое может повторяться => определяем == !=
	// теперь храним ссылки вместо самих объектов
	struct consumer_info {
		string& delivery_description;
		string& consumer_name;
		string& consumer_surname;
		string& consumer_patronymic;
		string& consumer_email;
		string& consumer_phone_number;
		string& consumer_adress;
		string& consumer_comments;
		string& delivery_date_time;

		// достаем ссылки из приспособленца
		consumer_info(const string& delivery_de,
			const string& consumer_n,
			const string& consumer_s,
			const string& consumer_p,
			const string& consumer_e,
			const string& consumer_ph,
			const string& consumer_a,
			const string& consumer_c,
			const string& delivery_da) :
			delivery_description(string_flyweight::get_instance().get_flyweight(delivery_de)),
			consumer_name(string_flyweight::get_instance().get_flyweight(consumer_n)),
			consumer_surname(string_flyweight::get_instance().get_flyweight(consumer_s)),
			consumer_patronymic(string_flyweight::get_instance().get_flyweight(consumer_p)),
			consumer_email(string_flyweight::get_instance().get_flyweight(consumer_e)),
			consumer_phone_number(string_flyweight::get_instance().get_flyweight(consumer_ph)),
			consumer_adress(string_flyweight::get_instance().get_flyweight(consumer_a)),
			consumer_comments(string_flyweight::get_instance().get_flyweight(consumer_c)),
			delivery_date_time(string_flyweight::get_instance().get_flyweight(delivery_da))
		{}

		// явно описан оператор
		consumer_info& operator=(const consumer_info& other) {
			this->delivery_description = other.delivery_description;
			this->consumer_name = other.consumer_name;
			this->consumer_surname = other.consumer_surname;
			this->consumer_patronymic = other.consumer_patronymic;
			this->consumer_email = other.consumer_email;
			this->consumer_phone_number = other.consumer_phone_number;
			this->consumer_adress = other.consumer_adress;
			this->consumer_comments = other.consumer_comments;
			this->delivery_date_time = other.delivery_date_time;
			return *this;
		}


		bool operator ==(const consumer_info& other) const {
			return (
				this->consumer_adress == other.consumer_adress && this->consumer_comments == other.consumer_comments &&
				this->consumer_email == other.consumer_email && this->consumer_name == other.consumer_name &&
				this->consumer_patronymic == other.consumer_patronymic && this->consumer_phone_number == other.consumer_phone_number &&
				this->consumer_surname == other.consumer_surname && this->delivery_date_time == other.delivery_date_time &&
				this->delivery_description == other.delivery_description
				);
		}

		bool operator !=(const consumer_info& other) const {
			return !(
				this->consumer_adress == other.consumer_adress && this->consumer_comments == other.consumer_comments &&
				this->consumer_email == other.consumer_email && this->consumer_name == other.consumer_name &&
				this->consumer_patronymic == other.consumer_patronymic && this->consumer_phone_number == other.consumer_phone_number &&
				this->consumer_surname == other.consumer_surname && this->delivery_date_time == other.delivery_date_time &&
				this->delivery_description == other.delivery_description
				);
		}

	};

private:

	//#### КОМПАРАТОРЫ ####//

	class comparison_string
	{

	public:
		comparison_string() = default;
		int operator()(const string& s_c1, const string& s_c2) const {
			return s_c1.compare(s_c2);
			//s_c1 < s_c2 -> - ; s_c1 = s_c2 -> 0; s_c1 > s_c2 -> +
		}
	};

	class delivery_data_comparer
	{

	public:

		delivery_data_comparer() = default;

		int operator()(const delivery_data& del1, const delivery_data& del2) const {
			int result = del1.consumer_id - del2.consumer_id;
			if (result != 0) {
				return result;
			}
			else {
				return(del1.delivery_id - del2.delivery_id);
			}
		}

	};

private:

	//#### КОЛЛЕКЦИЯ ####//

//_database - это дерево,содержащее деревья[пулы](кч на разных аллокаторах),в которых содержатся деревья[схемы](кч),в которых содержатся деревья[коллекции](кч,авл) в которых лежат данные
	red_black_tree<string, // дб
		red_black_tree<string, //дерево пуллов
		red_black_tree<string, //дерево схем
		associative_container<delivery_data, consumer_info>*, //коллекция
		comparison_string>*,
		comparison_string>*,
		comparison_string>* _database;

	logger* _logger;
	memory_global_heap* _allocator;
	red_black_tree<string, memory*, comparison_string>* _all_allocators;




public:

	//#### КОНСТРУКТОР ####//

	explicit database(logger* logger1) {
		_logger = logger1;
		_allocator = new memory_global_heap(_logger);

		_database = new red_black_tree <string,
			red_black_tree<string,
			red_black_tree<string,
			associative_container<delivery_data, consumer_info>*,
			comparison_string>*,
			comparison_string>*,
			comparison_string>(_allocator, _logger);

		_all_allocators = new red_black_tree<string, memory*, comparison_string>(_allocator, _logger);

		_logger->log("#### Your database was successfully created and now it is ready for a work! ####", logger::severity::warning);
	}

	//запрещены конструкторы копирования,перемещения и соотвествующие операторы

	database(const database& other) = delete;
	database(database&& other) = delete;
	database& operator=(const database& other) = delete;
	database& operator=(database&& other) = delete;

public:

	//#### ДЕСТРУКТОР ####//

	~database() {

		reboot_database();

		delete _all_allocators;
		delete _database;
		delete _allocator;

		_logger->log("#### Your database was successfully deleted! ####", logger::severity::warning);
	}

private:

	//#### ПАРСЕР И РАБОТА С ФАЙЛАМИ ####//

	static delivery_data key_parser(const string& key) {
		//key: [consumer_id,delivery_id]
		string str_cut;
		size_t division1, division2;

		//корректность формата ввода
		if (key.find("key: ") != 0) {
			throw logic_error("PARSING ERROR no.1: Invalid key format.");
		}

		str_cut = key.substr(5); //[consumer_id,delivery_id]

		division1 = str_cut.find('[');
		if (division1 != 0) {
			throw logic_error("PARSING ERROR no.1: Invalid key format.");
		}

		division2 = str_cut.find(",");
		if (division2 == string::npos) {
			throw logic_error("PARSING ERROR no.1: Invalid key format.");
		}

		//корректность самих данных
		string take_consumer_id = str_cut.substr(1, division2 - 1);

		if (take_consumer_id.empty()) {
			throw logic_error("PARSING ERROR no.2: Invalid consumer_id(empty data).");
		}
		for (size_t i = 0; i < take_consumer_id.size(); i++) {
			char c = take_consumer_id[i];
			if (!isdigit(c)) {
				throw logic_error("PARSING ERROR no.2: Invalid consumer_id(incorrect data).");
			}
		}
		int consumer_id = stoi(take_consumer_id);

		//коррекность формата ввода
		str_cut = str_cut.substr(division2 + 1); //delivery_id]
		division2 = str_cut.find("]");
		if (division2 == string::npos) {
			throw logic_error("PARSING ERROR no.1: Invalid key format.");
		}

		string take_delivery_id = str_cut.substr(0, division2);

		if (take_delivery_id.empty()) {
			throw logic_error("PARSING ERROR no.3: Invalid delivery_id(empty data).");
		}
		for (size_t i = 0; i < take_delivery_id.size(); i++) {
			char c = take_delivery_id[i];
			if (!isdigit(c)) {
				throw logic_error("PARSING ERROR no.3: Invalid delivery_id(incorrect data).");
			}
		}
		int delivery_id = stoi(take_delivery_id);

		delivery_data kf = { consumer_id,delivery_id };

		return kf;
	}

	static string one_unit_parser(string& str, const string& parameter) {

		string unit_value;
		size_t division1, division2;
		division1 = str.find('[');
		if (division1 != 0) {
			throw logic_error("PARSING ERROR no.4: Invalid unit format(begin sign).");
		}

		division2 = str.find("] ");
		if (division2 == string::npos) {
			throw logic_error("PARSING ERROR no.4: Invalid unit format(end sign).");
		}

		unit_value = str.substr(1, division2 - 1);
		if (unit_value.empty()) {
			string error_text = "PARSING ERROR no.5: Invalid paramerer: " + parameter + ".";
			throw logic_error(error_text);
		}

		str = str.substr(division2 + 2);

		return unit_value;
	}

	static string one_unit_parser_last(string& str, const string& parameter) {

		string unit_value;
		size_t division1, division2;
		division1 = str.find('[');
		if (division1 != 0) {
			throw logic_error("PARSING ERROR no.4: Invalid unit format(begin sign).");
		}

		division2 = str.find("]");
		if (division2 == string::npos) {
			throw logic_error("PARSING ERROR no.4: Invalid unit format(end sign).");
		}

		unit_value = str.substr(1, division2 - 1);
		if (unit_value.empty()) {
			string error_text = "PARSING ERROR no.5: Invalid paramerer: " + parameter + ".";
			throw logic_error(error_text);
		}

		return unit_value;
	}

	static consumer_info value_parser(const string& value) {

		string delivery_description;
		string consumer_name;
		string consumer_surname;
		string consumer_patronymic;
		string consumer_email;
		string consumer_phone_number;
		string consumer_adress;
		string consumer_comments;
		string delivery_date_time;

		string str_cut;

		if (value.find("value: ") != 0) {
			throw logic_error("PARSING ERROR no.6: Invalid value format.");
		}

		str_cut = value.substr(7);

		try {
			delivery_description = one_unit_parser(str_cut, "delivery description");
			consumer_name = one_unit_parser(str_cut, "consumer's name");
			consumer_surname = one_unit_parser(str_cut, "consumer's surname");
			consumer_patronymic = one_unit_parser(str_cut, "consumer's patronymic");
			consumer_email = one_unit_parser(str_cut, "consumer's email");
			consumer_phone_number = one_unit_parser(str_cut, "consumer's phone number");
			consumer_adress = one_unit_parser(str_cut, "consumer's adress");
			consumer_comments = one_unit_parser(str_cut, "consumer's comments");
			delivery_date_time = one_unit_parser_last(str_cut, "delivery date/time");
		}
		catch (const logic_error& err) {
			//throw err;
			string fix = err.what();
			fix += " value parsing error";
			throw logic_error(fix);
		}


		consumer_info vf = { delivery_description,consumer_name,consumer_surname,consumer_patronymic,consumer_email,consumer_phone_number,consumer_adress,consumer_comments,delivery_date_time };

		return vf;
	}

	static string value_transfer_string(const consumer_info& value) {
		string output = "\n DELIVERY INFORMATION \n Delivery description: ";
		output += value.delivery_description;
		output += "\n Consumer's name: ";
		output += value.consumer_name;
		output += "\n Consumer's surname: ";
		output += value.consumer_surname;
		output += "\n Consumer's patronymic: ";
		output += value.consumer_patronymic;
		output += "\n Consumer's email: ";
		output += value.consumer_email;
		output += "\n Consumer's phone number: ";
		output += value.consumer_phone_number;
		output += "\n Consumer's adress: ";
		output += value.consumer_adress;
		output += "\n Consumer's comments: ";
		output += value.consumer_comments;
		output += "\n Delivery date/time: ";
		output += value.delivery_date_time;
		output += "\n";
		return output;
	}

	static string key_value_print(const pair<delivery_data, consumer_info>& formed_pair) {

		delivery_data key = get<0>(formed_pair);
		consumer_info value = get<1>(formed_pair);
		string output = "\n #### key: [consumer id: ";
		output += to_string(key.consumer_id);
		output += ",delivery id: ";
		output += to_string(key.delivery_id);
		output += "] ####\n";
		output += value_transfer_string(value);

		return output;
	}

	static void psc_parser(string& nameof_pool, string& nameof_scheme, string& nameof_collection, string& command_str) {

		size_t division1, division2;

		try {
			nameof_pool = one_unit_parser(command_str, "name of pool");
			nameof_scheme = one_unit_parser(command_str, "name of scheme");
		}
		catch (const logic_error& err) {
			throw err;
		}

		division1 = command_str.find('[');
		if (division1 != 0) {
			throw logic_error("PARSING ERROR no.7: Invalid option string format.");
		}

		division2 = command_str.find(']');
		if (division2 == string::npos) {
			throw logic_error("PARSING ERROR no.7: Invalid option string format.");
		}

		nameof_collection = command_str.substr(1, division2 - 1);
		if (nameof_collection.empty()) {
			throw logic_error("PARSING ERROR no.8: Invalid name of collection");
		}
	}

	void option_parser(const string& option, const string& key, const string& value) {

		string str_cut;
		size_t division1, division2, division3;


		if (option.find("insert: ") == 0) {

			str_cut = option.substr(8);
			string nameof_pool, nameof_scheme, nameof_collection;

			try {
				psc_parser(nameof_pool, nameof_scheme, nameof_collection, str_cut);
			}
			catch (const logic_error& err) {
				throw err;
				/*string fix = err.what();
				fix += " parsing insert data error";
				throw logic_error(fix);*/
			}

			delivery_data loc_key;
			// теперь нельзя объявить пустую структуру consumer_info тк ссылки на объекты должны быть обязательно проинициализированы
			try {
				loc_key = key_parser(key);
				consumer_info loc_value = value_parser(value);
				insertion(loc_key, loc_value, nameof_pool, nameof_scheme, nameof_collection);
			}
			catch (const logic_error& err) {
				//throw err;
				string fix = err.what();
				fix += " key/value error";
				throw logic_error(fix);
			}

			string output = "#### Insertion done successfully!(with key[consumer_id:";
			output += to_string(loc_key.consumer_id);
			output += ",delivery_id:";
			output += to_string(loc_key.delivery_id);
			output += "]) ####";
			_logger->log(output, logger::severity::warning);
		}

		else if (option.find("read key: ") == 0) {

			str_cut = option.substr(10);
			string nameof_pool, nameof_scheme, nameof_collection;

			try {
				psc_parser(nameof_pool, nameof_scheme, nameof_collection, str_cut);
			}
			catch (const logic_error& err) {
				throw err;
			}

			delivery_data loc_key;
			// теперь нельзя объявить пустую структуру consumer_info тк ссылки на объекты должны быть обязательно проинициализированы
			try {
				loc_key = key_parser(key);
				consumer_info loc_value = reading_key(loc_key, nameof_pool, nameof_scheme, nameof_collection);
				string output = "\n #### [consumer_id:";
				output += to_string(loc_key.consumer_id);
				output += ",delivery_id:";
				output += to_string(loc_key.delivery_id);
				output += "] ####";
				output += value_transfer_string(loc_value);
				_logger->log(output, logger::severity::warning);
			}
			catch (logic_error& err) {
				throw err;
			}
		}

		else if (option.find("update key: ") == 0) {

			str_cut = option.substr(12);
			string nameof_pool, nameof_scheme, nameof_collection;

			try {
				psc_parser(nameof_pool, nameof_scheme, nameof_collection, str_cut);
			}
			catch (const logic_error& err) {
				throw err;
			}

			delivery_data loc_key;
			// теперь нельзя объявить пустую структуру consumer_info тк ссылки на объекты должны быть обязательно проинициализированы
			try {
				loc_key = key_parser(key);
				consumer_info loc_value = value_parser(value);
				updating_key(loc_key, loc_value, nameof_pool, nameof_scheme, nameof_collection);
			}
			catch (const logic_error& err) {
				throw err;
			}

			string output = "\n #### Key update done successfully!(with key[consumer_id:";
			output += to_string(loc_key.consumer_id);
			output += ",delivery_id:";
			output += to_string(loc_key.delivery_id);
			output += "]) ####";
			_logger->log(output, logger::severity::warning);
		}

		else if (option.find("remove: ") == 0) {

			str_cut = option.substr(8);

			string nameof_pool, nameof_scheme, nameof_collection;

			try {
				psc_parser(nameof_pool, nameof_scheme, nameof_collection, str_cut);
			}
			catch (const logic_error& err) {
				throw err;
			}

			delivery_data loc_key;

			try {
				loc_key = key_parser(key);
				removing(loc_key, nameof_pool, nameof_scheme, nameof_collection);
			}
			catch (logic_error& err) {
				throw err;
			}

			string output = "\n #### Key remove done successfully!(with key[consumer_id:";
			output += to_string(loc_key.consumer_id);
			output += ",delivery_id:";
			output += to_string(loc_key.delivery_id);
			output += "]) ####";
			_logger->log(output, logger::severity::warning);
		}
		//BACK BACK BACK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
		else if (option.find("add pool: ") == 0) {

			str_cut = option.substr(10);

			string nameof_pool;
			allocators nameof_allocator;
			size_t size;
			status current_status;

			try {
				nameof_pool = one_unit_parser(str_cut, "name of pool");
			}
			catch (const logic_error& err) {
				throw err;
			}

			division1 = str_cut.find('[');
			if (division1 != 0) {
				throw logic_error("PARSING ERROR no.8: Invalid option string format(outside).");
			}

			int endif = 0;
			division2 = str_cut.find(',');
			if (division2 == string::npos) {
				//throw logic_error("PARSING ERROR no.8: Invlaid option string format(inside).");
				division2 = str_cut.find(']');
				endif = 1;
			}

			string nameof_allocator_str = str_cut.substr(1, division2 - 1);

			if (nameof_allocator_str.empty()) {
				throw logic_error("PARSING ERROR no.9: Invalid name of allocator.");
			}
			if (nameof_allocator_str == "global heap") {
				nameof_allocator = allocators::global_heap_allocator;
			}
			else if (nameof_allocator_str == "border descriptors") {
				nameof_allocator = allocators::border_descriptors_allocator;
			}
			else {
				throw logic_error("PARSING ERROR no.9: Invalid name of allocator.");
			}

			if (endif == 1) {
				str_cut = str_cut.substr(division2);
			}
			else {
				str_cut = str_cut.substr(division2 + 1);
			}

			if (nameof_allocator != allocators::global_heap_allocator) {

				division2 = str_cut.find(',');
				if (division2 == string::npos) {
					throw logic_error("PARSING ERROR no.8: Invalid option string format(inside).");
				}

				string size_str = str_cut.substr(0, division2 - 1);

				if (size_str.empty()) {
					throw logic_error("PARSING ERROR no.8.e1: Invalid size.");
				}
				for (size_t i = 0; i < size_str.size(); i++) {
					char c = size_str[i];
					if (!isdigit(c)) {
						throw logic_error("PARSING ERROR no.8.e1: Invalid size.");
					}
				}
				size = (size_t)(stoi(size_str));
				str_cut = str_cut.substr(division2 + 1);
			}

			else {
				size = 1;
				division1 = str_cut.find(']');
				if (division1 != 0) {
					throw logic_error("PARSING ERROR no.8: Invlaid option string format(inside).");
				}
			}
			if (nameof_allocator == allocators::border_descriptors_allocator) {

				division2 = str_cut.find(']');
				if (division2 == string::npos) {
					throw logic_error("PARSING ERROR no.8: Invalid option string format(inside).");
				}

				string status_str = str_cut.substr(0, division2);

				if (status_str.empty()) {
					throw logic_error("PARSING ERROR no.8.e2: Invalid allocation status.");
				}
				if (status_str == "best") {
					current_status = status::best_available;
				}
				else if (status_str == "worst") {
					current_status = status::worst_available;
				}
				else if (status_str == "first") {
					current_status = status::first_available;
				}
				else {
					string debugging_name = "PARSING ERROR no.8.e2: Invalid allocation status(";
					debugging_name += status_str;
					debugging_name += ")";
					throw logic_error(debugging_name);
				}

			}
			else {
				current_status = status::neutral;
				division1 = str_cut.find(']');
				if (division1 != 0) {
					throw logic_error("PARSING ERROR no.8: Invalid option string format(inside).");
				}
			}

			try {
				add_pool(nameof_pool, nameof_allocator, size, current_status);
			}
			catch (const logic_error& err) {
				throw err;
			}

			string output = "\n #### Pool ";
			output += nameof_pool;
			output += " was successfully added! ####";
			_logger->log(output, logger::severity::warning);
		}

		else if (option.find("delete pool: ") == 0) {

			str_cut = option.substr(13);

			division1 = str_cut.find('[');
			if (division1 != 0) {
				throw logic_error("PARSING ERROR no.10: Invlaid option string format(deleting pool begin).");
			}

			division2 = str_cut.find(']');
			if (division2 == string::npos) {
				throw logic_error("PARSING ERROR no.10: Invlaid option string format(deleting pool end).");
			}

			string nameof_pool = str_cut.substr(1, division2 - 1);
			if (nameof_pool.empty()) {
				throw logic_error("PARSING ERROR no.11: Invlaid name of pool.");
			}

			try {
				delete_pool(nameof_pool);
			}
			catch (const logic_error& err) {
				throw err;
			}

			string output = "\n #### This pool: ";
			output += nameof_pool;
			output += " :was successfully deleted! ####";
			_logger->log(output, logger::severity::warning);
		}

		else if (option.find("add scheme: ") == 0) {

			str_cut = option.substr(12);

			string nameof_pool, nameof_scheme;

			try {
				nameof_pool = one_unit_parser(str_cut, "name of pool");
			}
			catch (const logic_error& err) {
				throw err;
			}

			division1 = str_cut.find('[');
			if (division1 != 0) {
				throw logic_error("PARSING ERROR no.12: Invalid option string format(creating scheme).");
			}

			division2 = str_cut.find(']');
			if (division2 == string::npos) {
				throw logic_error("PARSING ERROR no.12: Invalid option string format(creating scheme).");
			}

			nameof_scheme = str_cut.substr(1, division2 - 1);
			if (nameof_scheme.empty()) {
				throw logic_error("PARSING ERROR no.12: Invalid option string format(creating scheme).");
			}

			try {
				add_scheme(nameof_scheme, nameof_pool);
			}
			catch (const logic_error& err) {
				throw err;
			}

			string output = "\n #### This scheme(";
			output += nameof_scheme;
			output += ") was successfully added(pool:";
			output += nameof_pool;
			output += ") ####";
			_logger->log(output, logger::severity::warning);
		}

		else if (option.find("delete scheme: ") == 0) {

			str_cut = option.substr(15);

			string nameof_pool, nameof_scheme;

			try {
				nameof_pool = one_unit_parser(str_cut, "name of pool");
			}
			catch (const logic_error& err) {
				throw err;
			}

			division1 = str_cut.find('[');
			if (division1 != 0) {
				throw logic_error("PARSING ERROR no.13: Invalid option string format(deleting scheme).");
			}

			division2 = str_cut.find(']');
			if (division2 == string::npos) {
				throw logic_error("PARSING ERROR no.13: Invalid option string format(deleting scheme).");
			}

			nameof_scheme = str_cut.substr(1, division2 - 1);

			if (nameof_scheme.empty()) {
				throw logic_error("PARSING ERROR no.13: Invalid option string format(deleting scheme).");
			}

			try {
				delete_scheme(nameof_scheme, nameof_pool);
			}
			catch (const logic_error& err) {
				throw err;
			}

			string output = "\n #### This scheme(";
			output += nameof_scheme;
			output += ") was successfully deleted(pool:";
			output += nameof_pool;
			output += ") ####";
			_logger->log(output, logger::severity::warning);
		}
		else if (option.find("add collection: ") == 0) {

			str_cut = option.substr(16);

			string nameof_pool, nameof_scheme, nameof_collection;
			trees nameof_tree;

			try {
				nameof_pool = one_unit_parser(str_cut, "name of pool");
				nameof_scheme = one_unit_parser(str_cut, "name of scheme");
				nameof_collection = one_unit_parser(str_cut, "name of collection");
			}
			catch (const logic_error& err) {
				throw err;
			}
			//
			division1 = str_cut.find('[');
			if (division1 != 0) {
				throw logic_error("PARSING ERROR no.14: Invalid option string format(creating collection).");
			}

			division2 = str_cut.find(']');
			if (division2 == string::npos) {
				throw logic_error("PARSING ERROR no.14: Invalid option string format(creating collection).");
			}

			string nameof_tree_str = str_cut.substr(1, division2 - 1);

			if (nameof_tree_str.empty()) {
				throw logic_error("PARSING ERROR no.14: Invalid option string format(creating collection).");
			}

			if (nameof_tree_str == "red black tree") {
				nameof_tree = trees::red_black_tree;
			}
			else if (nameof_tree_str == "avl tree") {
				nameof_tree = trees::avl_tree;
			}
			else {
				throw logic_error("PARSING ERROR no.14: Invalid option string format(creating collection).");
			}

			try {
				add_collection(nameof_collection, nameof_pool, nameof_scheme, nameof_tree);
			}
			catch (const logic_error& err) {
				throw err;
			}

			string output = "\n #### This collection:";
			output += nameof_collection;
			output += " was successfully added(scheme:";
			output += nameof_scheme;
			output += ",pool:";
			output += nameof_pool;
			output += ") ####";
			_logger->log(output, logger::severity::warning);
		}
		else if (option.find("delete collection: ") == 0) {

			str_cut = option.substr(19);

			string nameof_pool, nameof_scheme, nameof_collection;

			try {
				psc_parser(nameof_pool, nameof_scheme, nameof_collection, str_cut);
				delete_collection(nameof_collection, nameof_pool, nameof_scheme);
			}
			catch (const logic_error& err) {
				throw err;
			}

			string output = "\n #### This collection:";
			output += nameof_collection;
			output += " was successfully deleted(scheme:";
			output += nameof_scheme;
			output += ",pool:";
			output += nameof_pool;
			output += ") ####";
			_logger->log(output, logger::severity::warning);
		}
		else if (option.find("reboot database:") == 0) {

		reboot_database();
		_logger->log("#### This database was successfully rebooted! ####", logger::severity::warning);
}
		else if (option.find("read range: ") == 0) {

			str_cut = option.substr(12);

			string nameof_pool, nameof_scheme, nameof_collection, loc_key_str;
			delivery_data loc_key_min, loc_key_max;
			delivery_data* p_loc_key_min = nullptr;
			delivery_data* p_loc_key_max = nullptr;
			delivery_data_comparer comparer;
			queue<pair<delivery_data, consumer_info>> results;

			try {
				psc_parser(nameof_pool, nameof_scheme, nameof_collection, str_cut);
			}
			catch (const logic_error& err) {
				throw err;
			}

			if (key.find("keys: ") != 0) {
				throw logic_error("PARSING ERROR no.15: Invalid key string format(reading range).1");
			}

			loc_key_str = key.substr(6);

			division1 = loc_key_str.find('[');
			if (division1 != 0) {
				throw logic_error("PARSING ERROR no.15: Invalid key string format(reading range).2");
			}

			division2 = loc_key_str.find("] ");
			if (division2 == string::npos) {
				throw logic_error("PARSING ERROR no.15: Invalid key string format(reading range).3");
			}

			if (division2 == 1) {
				loc_key_str = loc_key_str.substr(3);
			}
			else {
				division3 = loc_key_str.find(", ");
				if ((division3 == string::npos) || (division3 > division2)) {
					throw logic_error("PARSING ERROR no.15: Invalid key string format(reading range).");;
				}

				string consumer_id_str = loc_key_str.substr(1, division3 - 1);

				if (consumer_id_str.empty()) {
					throw logic_error("PARSING ERROR no.16: Invalid consumer id string format(reading range (key minimal) ).");
				}

				for (size_t i = 0; i < consumer_id_str.size(); i++) {
					char c = consumer_id_str[i];
					if (!isdigit(c)) {
						throw logic_error("PARSING ERROR no.16: Invalid consumer id string format(reading range (key minimal) ).");
					}
				}
				int loc_consumer_id = stoi(consumer_id_str);

				string delivery_id_str = loc_key_str.substr(division3 + 2, division2 - division3 - 2);
				
				if (delivery_id_str.empty()) {
					throw logic_error("PARSING ERROR no.17: Invalid delivery id string format(reading range (key minimal) ).");
				}

				for (size_t i = 0; i < delivery_id_str.size(); i++) {
					char c = delivery_id_str[i];
					if (!isdigit(c)) {
						throw logic_error("PARSING ERROR no.17: Invalid delivery id string format(reading range (key minimal) ).");
					}
				}
				int loc_delivery_id = stoi(delivery_id_str);

				loc_key_min = { loc_consumer_id,loc_delivery_id };
				p_loc_key_min = &loc_key_min;

				loc_key_str = loc_key_str.substr(division2 + 2);
			}

			division1 = loc_key_str.find('[');
			if (division1 != 0) {
				throw logic_error("PARSING ERROR no.15: Invalid key string format(reading range).1");
			}

			division2 = loc_key_str.find(']');
			if (division2 == string::npos) {
				throw logic_error("PARSING ERROR no.15: Invalid key string format(reading range).2");
			}

			if (division2 != 1) {

				division3 = loc_key_str.find(", ");
				if ((division3 == string::npos) || (division3 > division2)) {
					throw logic_error("PARSING ERROR no.15: Invalid key string format(reading range).3");
				}

				string consumer_id_str = loc_key_str.substr(1, division3 - 1);

				if (consumer_id_str.empty()) {
					throw logic_error("PARSING ERROR no.18: Invalid consumer id string format(reading range (key maximal)).");
				}

				for (size_t i = 0; i < consumer_id_str.size(); i++) {
					char c = consumer_id_str[i];
					if (!isdigit(c)) {
						throw logic_error("PARSING ERROR no.18: Invalid consumer id string format(reading range(key maximal)).");
					}
				}
				int loc_consumer_id = stoi(consumer_id_str);

				string delivery_id_str = loc_key_str.substr(1, division3 - 1);

				if (delivery_id_str.empty()) {
					throw logic_error("PARSING ERROR no.19: Invalid delivery id string format(reading range (key maximal)).");
				}

				for (size_t i = 0; i < delivery_id_str.size(); i++) {
					char c = delivery_id_str[i];
					if (!isdigit(c)) {
						throw logic_error("PARSING ERROR no.19: Invalid delivery id string format(reading range(key maximal)).");
					}
				}
				int loc_delivery_id = stoi(delivery_id_str);

				loc_key_max = { loc_consumer_id,loc_delivery_id };
				p_loc_key_max = &loc_key_max;
			}

			if (p_loc_key_min != nullptr && p_loc_key_max != nullptr && comparer(loc_key_min, loc_key_max) > 0) {
				throw logic_error("PARSING ERROR no.20: Maximum bound is lower than minimum bound!");
			}

			try {
				read_range(results, nameof_pool, nameof_scheme, nameof_collection, p_loc_key_min, p_loc_key_max);
			}
			catch (const logic_error& ex) {
				throw ex;
			}

			string output = "\n #### Reading range from ";

			if (p_loc_key_min == nullptr) {
				output += "Begin to ";
			}
			else {
				output += "[consumer_id:";
				output += to_string(loc_key_min.consumer_id);
				output += ",delivery_id:";
				output += to_string(loc_key_min.delivery_id);
				output += "] to ";
			}

			if (p_loc_key_max == nullptr) {
				output += "end";
			}
			else {
				output += "[consumer_id:";
				output += to_string(loc_key_max.consumer_id);
				output += ",delivery_id:";
				output += to_string(loc_key_max.delivery_id);
				output += "]\n";
			}

			if (results.empty()) {
				output += "\n\n No data found\n";
			}
			else {
				while (!results.empty()) {
					output += key_value_print(results.front());
					results.pop();
				}
			}
			_logger->log(output, logger::severity::warning);
		}
		else {
			throw logic_error("PARSING ERROR no.21: You have entered a option which do not exist or do not support adviced format.");
		}

	}
	/*
	 #include <filesystem>

std::string getFileName(const std::string& path) {
	std::filesystem::path filePath(path);
	std::string fileName = filePath.filename().string();

	// Заменяем обратные слеши на прямые слеши
	std::replace(fileName.begin(), fileName.end(), '\\', '/');

	return fileName;
}
	*/
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

public:

	void file_parse_options(const string& read_path) {

		string output = "#### Now the system is parsing this file: ";
		output += fname(read_path);
		output += " ####";
		_logger->log(output, logger::severity::warning);

		ifstream fin;
		fin.open(read_path);
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

			else if ( s1.find("read key: ") == 0 || s1.find("read range: ") == 0 || s1.find("remove: ") == 0 ) {

				if (!getline(fin, s2)) {
					fin.close();
					throw logic_error("FILE PARSING ERROR 2: Invalid option string format,enter your key!");
				}
			}

			else if (s1.find("add pool: ") == 0 || s1.find("delete pool: ") == 0 || s1.find("add scheme: ") == 0 || s1.find("delete scheme: ") == 0 || s1.find("add collection: ") == 0 || s1.find("delete collection: ") == 0 || s1.find("reboot database:") == 0) {

				try {
					option_parser(s1, s2, s3);
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
				option_parser(s1, s2, s3);
			}
			catch (const logic_error& err) {
				fin.close();
				throw err;
			}
		}

		fin.close();
		_logger->log("#### The system finished parsing your file successfully! ####", logger::severity::warning);
	}


	//РЕАЛИЗАЦИЯ КОМАНД

private:

	red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* get_pool(const string& nameof_pool) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;

		try {
			cur_pool = _database->get(nameof_pool);
		}
		catch (const logic_error& err) {
			throw logic_error("TECHNICAL ERROR no.1: Adviced pool was not found!");
		}

		return cur_pool;
	}

	static red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* get_scheme(const string& nameof_scheme, const red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool) {

		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;

		try {
			cur_scheme = cur_pool->get(nameof_scheme);
		}
		catch (const logic_error& err) {
			throw logic_error("TECHNICAL ERROR no.2: Adviced scheme was not found!");
		}

		return cur_scheme;
	}

	static associative_container<delivery_data, consumer_info>* get_collection(const string& nameof_collection, const red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme) {

		associative_container<delivery_data, consumer_info>* cur_collection;

		try {
			cur_collection = cur_scheme->get(nameof_collection);
		}
		catch (logic_error& err) {
			throw logic_error("TECHNICAL ERROR no.3: Adviced collection was not found!");
		}

		return cur_collection;
	}

	void insertion(const delivery_data& key, const consumer_info& value, const string& nameof_pool, const string& nameof_scheme, const string& nameof_collection) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;
		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;
		associative_container<delivery_data, consumer_info>* cur_collection;

		try {
			cur_pool = get_pool(nameof_pool);
			cur_scheme = get_scheme(nameof_scheme, cur_pool);
			cur_collection = get_collection(nameof_collection, cur_scheme);
		}
		catch (const logic_error& err) {
			throw err;
		}

		try {
			cur_collection->insert(key, value);
		}
		catch (const logic_error& err) {
			string output = "TECHNICAL ERROR no.4: In collection: ";
			output += nameof_collection;
			output += " :";
			output += err.what();
			throw logic_error(output);
		}

		_logger->log("#### Insertion was successfully completed. ####", logger::severity::warning);
	}

	consumer_info reading_key(const delivery_data& key, const string& nameof_pool, const string& nameof_scheme, const string& nameof_collection) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;
		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;
		associative_container<delivery_data, consumer_info>* cur_collection;

		try {
			cur_pool = get_pool(nameof_pool);
			cur_scheme = get_scheme(nameof_scheme, cur_pool);
			cur_collection = get_collection(nameof_collection, cur_scheme);
		}
		catch (const logic_error& err) {
			throw err;
		}

		try {
			consumer_info value = cur_collection->get(key);
			_logger->log("#### Reading key was successfully completed. ####", logger::severity::warning);
			return value;
		}
		catch (const logic_error& err) {
			string output = "TECHNICAL ERROR no.5: In collection: ";
			output += nameof_collection;
			output += " : key was not found!";
			throw logic_error(output);
		}

	}

	void removing(const delivery_data& key, const string& nameof_pool, const string& nameof_scheme, const string& nameof_collection) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;
		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;
		associative_container<delivery_data, consumer_info>* cur_collection;

		try {
			cur_pool = get_pool(nameof_pool);
			cur_scheme = get_scheme(nameof_scheme, cur_pool);
			cur_collection = get_collection(nameof_collection, cur_scheme);
		}
		catch (const logic_error& err) {
			throw err;
		}

		// при удалении нужно обновить счетчик использования слов
		try {
			consumer_info removed_value = cur_collection->remove(key);
			string_flyweight::get_instance().check_flyweight(removed_value.delivery_description);
			string_flyweight::get_instance().check_flyweight(removed_value.consumer_name);
			string_flyweight::get_instance().check_flyweight(removed_value.consumer_surname);
			string_flyweight::get_instance().check_flyweight(removed_value.consumer_patronymic);
			string_flyweight::get_instance().check_flyweight(removed_value.consumer_adress);
			string_flyweight::get_instance().check_flyweight(removed_value.consumer_email);
			string_flyweight::get_instance().check_flyweight(removed_value.consumer_phone_number);
			string_flyweight::get_instance().check_flyweight(removed_value.consumer_comments);
			string_flyweight::get_instance().check_flyweight(removed_value.delivery_date_time);
		}
		catch (const logic_error& err) {
			string output = "TECHNICAL ERROR no.5: In collection: ";
			output += nameof_collection;
			output += " : key was not found!";
			throw logic_error(output);
		}

		_logger->log("#### Removing key was successfully completed. ####", logger::severity::warning);
	}

	void updating_key(const delivery_data& key, const consumer_info& value, const string& nameof_pool, const string& nameof_scheme, const string& nameof_collection) {

		try {
			removing(key, nameof_pool, nameof_scheme, nameof_collection);
			insertion(key, value, nameof_pool, nameof_scheme, nameof_collection);
		}
		catch (const logic_error& err) {
			throw err;
		}

		_logger->log("#### Updating key was successfully completed. ####", logger::severity::warning);
	}

	void add_pool(const string& nameof_pool, allocators nameof_allocator, size_t size, status cur_status = status::neutral) {

		int exist = 0;

		try {
			_database->get(nameof_pool);
		}
		catch (const logic_error& err) {
			exist = 1;
		}

		if (!exist) {
			string output = "TECHNICAL ERROR no.6: Adviced pool(";
			output += nameof_pool;
			output += ") is already exist!";
			throw logic_error(output);
		}

		memory* used_allocator;

		if (nameof_allocator == allocators::border_descriptors_allocator) {


			if (cur_status == status::best_available) {
				used_allocator = new allocator_border_descriptors(size, _logger, nullptr, allocator_border_descriptors::status::best_available);
			}
			else if (cur_status == status::worst_available) {
				used_allocator = new allocator_border_descriptors(size, _logger, nullptr, allocator_border_descriptors::status::worst_available);
			}
			else if (cur_status == status::first_available) {
				used_allocator = new allocator_border_descriptors(size, _logger, nullptr, allocator_border_descriptors::status::first_available);
			}
			else {
				throw logic_error("TECHNICAL ERROR no.6: Invalid allocation mode!");
			}
		}

		else {
			used_allocator = new memory_global_heap(_logger);
		}

		auto* cur_tree = reinterpret_cast<red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>*>(used_allocator->allocate(sizeof(red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>)));
		if (cur_tree == nullptr) {
			throw logic_error("#### Not enough memory! ####");
		}
		new (cur_tree) red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>(used_allocator, _logger);
		_database->insert(nameof_pool, cur_tree);
		_all_allocators->insert(nameof_pool, used_allocator);

		_logger->log("#### Creating pool was successfully completed. ####", logger::severity::warning);
	}

	void delete_pool(const string& nameof_pool) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;

		try {
			cur_pool = get_pool(nameof_pool);
		}
		catch (const logic_error& err) {
			throw err;
		}

		auto iterator = cur_pool->begin_postf();
		auto iteratorG = cur_pool->end_postf();

		for (; iterator != iteratorG; ++iterator) {
			auto nameof_scheme = get<0>(*iterator);
			delete_scheme(nameof_scheme, nameof_pool);
		}

		auto* used_allocator = _all_allocators->get(nameof_pool);
		cur_pool->~red_black_tree();

		used_allocator->deallocate(reinterpret_cast<void*>(cur_pool));

		_all_allocators->remove(nameof_pool);
		delete used_allocator;

		_database->remove(nameof_pool);

		_logger->log("#### Deleting pool was successfully completed. ####", logger::severity::warning);
	}

	void add_scheme(const string& nameof_scheme, const string& nameof_pool) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;

		try {
			cur_pool = get_pool(nameof_pool);
		}
		catch (const logic_error& err) {
			throw err;
		}

		int exist = 0;

		try {
			cur_pool->get(nameof_scheme);
		}
		catch (const logic_error& err) {
			exist = 1;
		}

		if (!exist) {
			string output = "TECHNICAL ERROR no.7: Adviced scheme(";
			output += nameof_scheme;
			output += ") is already exist!";
			throw logic_error(output);
		}

		auto* used_allocator = _all_allocators->get(nameof_pool);

		auto* cur_tree = reinterpret_cast<red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*>(used_allocator->allocate(sizeof(red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>)));
		
		new(cur_tree) red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>(used_allocator, _logger);

		cur_pool->insert(nameof_scheme, cur_tree);

		_logger->log("#### Creating scheme was successfully completed! ####", logger::severity::warning);
	}

	void delete_scheme(const string& nameof_scheme, const string& nameof_pool) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;
		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;

		try {
			cur_pool = get_pool(nameof_pool);
			cur_scheme = get_scheme(nameof_scheme, cur_pool);
		}
		catch (const logic_error& err) {
			throw err;
		}

		auto iterator = cur_scheme->begin_postf();
		auto iteratorG = cur_scheme->end_postf();

		for (; iterator != iteratorG; ++iterator) {
			auto nameof_collection = get<0>(*iterator);
			delete_collection(nameof_collection, nameof_pool, nameof_scheme);
		}

		auto* used_allocator = _all_allocators->get(nameof_pool);
		cur_scheme->~red_black_tree();

		used_allocator->deallocate(reinterpret_cast<void*>(cur_scheme));

		cur_pool->remove(nameof_scheme);

		_logger->log("#### Deleting scheme was successfully completed! ####", logger::severity::warning);
	}

	void add_collection(const string& nameof_collection, const string& nameof_pool, const string& nameof_scheme, trees tree_v) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;
		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;

		try {
			cur_pool = get_pool(nameof_pool);
			cur_scheme = get_scheme(nameof_scheme, cur_pool);
		}
		catch (const logic_error& err) {
			throw err;
		}

		int exist = 0;

		try {
			cur_scheme->get(nameof_collection);
		}
		catch (const logic_error& err) {
			exist = 1;
		}

		if (!exist) {
			string output = "TECHNICAL ERROR no.8: Adviced collection(";
			output += nameof_collection;
			output += ") is already exist!";
			throw logic_error(output);
		}

		auto* used_allocator = _all_allocators->get(nameof_pool);

		if (tree_v == trees::avl_tree) {

			auto* cur_tree = reinterpret_cast<avl_tree<delivery_data, consumer_info, delivery_data_comparer>*>(used_allocator->allocate(sizeof(avl_tree<delivery_data, consumer_info, delivery_data_comparer>)));
			new (cur_tree) avl_tree<delivery_data, consumer_info, delivery_data_comparer>(used_allocator, _logger);

			cur_scheme->insert(nameof_collection, cur_tree);
		}
		else {

			auto* cur_tree = reinterpret_cast<red_black_tree<delivery_data, consumer_info, delivery_data_comparer>*>(used_allocator->allocate(sizeof(red_black_tree<delivery_data, consumer_info, delivery_data_comparer>)));
			new (cur_tree) red_black_tree<delivery_data, consumer_info, delivery_data_comparer>(used_allocator, _logger);

			cur_scheme->insert(nameof_collection, cur_tree);
		}

		_logger->log("#### Creating collection was successfully completed! ####", logger::severity::warning);
	}

	void delete_collection(const string& nameof_collection, const string& nameof_pool, const string& nameof_scheme) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;
		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;
		associative_container<delivery_data, consumer_info>* cur_collection;

		try {
			cur_pool = get_pool(nameof_pool);
			cur_scheme = get_scheme(nameof_scheme, cur_pool);
			cur_collection = get_collection(nameof_collection, cur_scheme);
		}
		catch (const logic_error& err) {
			throw err;
		}

		auto iterator = dynamic_cast<binary_search_tree<delivery_data, consumer_info, delivery_data_comparer>*>(cur_collection)->begin_inf();
		auto iteratorG = dynamic_cast<binary_search_tree<delivery_data, consumer_info, delivery_data_comparer>*>(cur_collection)->end_inf();

		for (; iterator != iteratorG; ++iterator) {
			auto& info = get<1>(*iterator);
			string_flyweight::get_instance().check_flyweight(info.delivery_description);
			string_flyweight::get_instance().check_flyweight(info.consumer_name);
			string_flyweight::get_instance().check_flyweight(info.consumer_surname);
			string_flyweight::get_instance().check_flyweight(info.consumer_patronymic);
			string_flyweight::get_instance().check_flyweight(info.consumer_adress);
			string_flyweight::get_instance().check_flyweight(info.consumer_email);
			string_flyweight::get_instance().check_flyweight(info.consumer_phone_number);
			string_flyweight::get_instance().check_flyweight(info.consumer_comments);
			string_flyweight::get_instance().check_flyweight(info.delivery_date_time);
		}

		auto* used_allocator = _all_allocators->get(nameof_pool);

		cur_collection->~associative_container();

		used_allocator->deallocate(reinterpret_cast<void*>(cur_collection));

		cur_scheme->remove(nameof_collection);

		_logger->log("#### Deleting collection was successfully completed! ####", logger::severity::warning);
	}

	void reboot_database() {

		auto iterator = _database->begin_postf();
		auto iteratorG = _database->end_postf();

		for (; iterator != iteratorG; ++iterator) {
			auto nameof_pool = get<0>(*iterator);
			delete_pool(nameof_pool);
		}
		_logger->log("#### Database was successfully rebooted! ####", logger::severity::warning);
	}

	void read_range(queue<pair<delivery_data, consumer_info>>& result, const string& nameof_pool, const string& nameof_scheme, const string& nameof_collection, const delivery_data* frst, const delivery_data* lst) {

		red_black_tree<string, red_black_tree<string, associative_container<delivery_data, consumer_info>*, comparison_string>*, comparison_string>* cur_pool;
		red_black_tree <string, associative_container<delivery_data, consumer_info>*, comparison_string>* cur_scheme;
		associative_container<delivery_data, consumer_info>* cur_collection;

		try {
			cur_pool = get_pool(nameof_pool);
			cur_scheme = get_scheme(nameof_scheme, cur_pool);
			cur_collection = get_collection(nameof_collection, cur_scheme);
		}
		catch (const logic_error& err) {
			throw err;
		}

		auto* ready_collection = dynamic_cast<binary_search_tree<delivery_data, consumer_info, delivery_data_comparer>*>(cur_collection);

		if (ready_collection == nullptr) {
			throw logic_error("TECHNICAL ERROR no.9: Invalid tree type of collection!");
		}

		auto iterator = ready_collection->begin_inf();
		auto iteratorG = ready_collection->end_inf();

		delivery_data_comparer cur_comparer;

		int minimal = 1;

		if (frst != nullptr) {

			while (iterator != iteratorG && cur_comparer(get<0>(*iterator), *frst) != 0) {

				++iterator;
			}
			if (iterator == iteratorG) {
				minimal = 0;
			}
		}

		if (minimal) {

			if (lst != nullptr) {

				while (iterator != iteratorG && cur_comparer(get<0>(*iterator), *lst) != 0) {

					result.emplace(get<0>(*iterator), get<1>(*iterator));
					++iterator;
				}

				if (iterator != iteratorG) {
					result.emplace(get<0>(*iterator), get<1>(*iterator));
				}
			}

			else {

				for (; iterator != iteratorG; ++iterator) {
					result.emplace(get<0>(*iterator), get<1>(*iterator));
				}
			}
		}

		_logger->log("#### Reading range was successfully done! ####", logger::severity::debug);
	}

	//ДИАЛОГ С ПОЛЬЗОВАТЕЛЕМ//

public:

	void dialogue_with_user() {

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
					file_parse_options(input);
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
						reboot_database();
						continue;
					}
					else if (taken_option == "3") {
						_logger->log("\n===== The program has been interupted! =====", logger::severity::debug);
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
						_logger->log("\n*Template: add pool: [nameof_pool] ['global heap' / 'border descriptors',size(for border descriptors),'best'/'worst'/'first'(for border descriptors)]\n", logger::severity::debug);
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
						_logger->log("\n*Template: add collection: [nameof_pool] [nameof_scheme] [nameof_collection] ['avl tree'/'red black tree']\n", logger::severity::debug);
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

						option_parser(option_line, key_line, value_line);
						_logger->log("\n============================ Completed! ============================\n ", logger::severity::debug);
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
				reboot_database();
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
	}



	

















};

#endif //CWW_DATABASE_H
