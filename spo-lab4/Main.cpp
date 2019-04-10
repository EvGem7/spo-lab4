#include <iostream>
#include <stack>
#include <windows.h>
#include <conio.h>
#include <string>

using namespace std;

HANDLE createThread(HANDLE mutex);
void stopThread(HANDLE thread);
DWORD WINAPI threadBody(LPVOID params);

int main(int argc, char** argv) {
	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	stack<HANDLE> threads;

	char input;
	bool isExit = false;
	while (!isExit) {
		input = _getch();
		switch (input) {
		case '+':
		{
			HANDLE thread = createThread(mutex);
			if (thread != NULL) {
				threads.push(thread);
			} else {
				if (WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0) {
					cout << "Cannot create thread" << endl;
					ReleaseMutex(mutex);
				}
			}
		}
			break;
		case '-':
			if (WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0) {
				if (!threads.empty()) {
					stopThread(threads.top());
					threads.pop();
				} else {
					cout << "What is dead may never die" << endl;
				}
				ReleaseMutex(mutex);
			}
			break;
		case 'q':
			isExit = true;
			while (!threads.empty()) {
				stopThread(threads.top());
				threads.pop();
			}
			break;
		}
	}
	CloseHandle(mutex);
	return 0;
}

HANDLE createThread(HANDLE mutex) {
	return CreateThread(
		NULL,
		0,
		threadBody,
		mutex,
		0,
		NULL
	);
}

DWORD WINAPI threadBody(LPVOID params) {
	DWORD threadId = GetCurrentThreadId();
	string msg = "Hello! I'm a thread with id ";
	msg += to_string(threadId);

	HANDLE mutex = reinterpret_cast<HANDLE>(params);
	for (;;) {
		if (WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0) {
			Sleep(500);
			for (char c : msg) {
				cout << c;
				Sleep(40);
			}
			cout << endl;
			ReleaseMutex(mutex);
		}
	}
}

void stopThread(HANDLE thread) {
	TerminateThread(thread, EXIT_SUCCESS);
}