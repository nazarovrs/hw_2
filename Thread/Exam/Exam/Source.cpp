#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
std::mutex mtx;
enum Skill { BAD, OK, NORMAL, GOOD };
const int COUNT_OF_STUDENTS = 5;
std::random_device rd;
std::uniform_int_distribution<> dist(0, 3);
std::mt19937 gen(rd());

struct Student {
	int id = 0;
	bool isReady = false;
	Skill skill = OK;
	void Prepare() {
		mtx.lock();
		std::cout << "\n" << id << " студент начал подготовку.";
		mtx.unlock(); 
		int temp = dist(gen);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 + temp*1500));
		switch (temp) {
		case 0: 
			skill = BAD;
			break;
		case 1:
			skill = OK;
			break;
		case 2:
			skill = NORMAL;
			break;
		case 3:
			skill = GOOD;
			break;
		}
		mtx.lock();
		std::cout << "\nНу, я готов. (Студент номер " << id << " тянет руку) ";
		mtx.unlock();
		isReady = true;
	}
};

struct Teacher {
	bool isFree = true;
	void check(Student student) {
		mtx.lock();
		std::cout << "\nСтудент " << student.id <<" отвечает... ";
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		mtx.lock();
		switch (student.skill)
		{
			case BAD:
				if ((std::rand() % 4) >= 1)
					std::cout << "\nНатянем троечку... Следующий!";
				else
					std::cout << "\nНеуд, следующий!";
				break;
			case OK:
				std::cout << "\n\"Удовлетворительно\", следующий!";
				break;
			case NORMAL:
				std::cout << "\nВаша оценка \"Хорошо\", следующий!";
				break;
			case GOOD:
				std::cout << "\nОтлично, следующий!";
				break;
		default:
			std::cout << "\nСовсем не по теме. Однозначно пересдача, следующий!";
			break;
		}
		mtx.unlock();
	}
};

void PrepareStudent(Student* students, int i) {
	students[i].Prepare();
}

void StartExam(Student* students, int n) {
	std::thread* threads = new std::thread[n];
	for (int i = 0; i < n; i++) {
		students[i].id = i;
		threads[i] = std::thread(PrepareStudent, students, i);
	}
	
}

int main() { 
	setlocale(LC_ALL, "russian");
	Student* students = new Student[5]();
	Teacher* teach = new Teacher();
	std::thread preparing(StartExam, std::ref(students), COUNT_OF_STUDENTS);
	int count = 0;
	while (1) {
		for (int i = 0; i < 5; i++) {
			if (students[i].isReady == true) {
				teach->check(students[i]);
				students[i].isReady = false;
				count++;
			}
		}
		if (count == COUNT_OF_STUDENTS)
			break;
	}
	preparing.join();
	
}