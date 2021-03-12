#include "Socket.cpp"
#include <pthread.h>
#include <chrono>
using namespace std;

int packets[256] = {0};

struct Record {
    int id;
    float ax, ay, az, gx, gy, gz, mx, my, mz;
};

struct ThreadArgs {
    Socket clientSocket;
    Record* recordBuffer;
};

Record storage[1000000] = {0};
std::chrono::duration<long int, std::ratio<1, 1000000000> >::rep startTime;
void printRecord(Record* rec) {
    cout << (std::chrono::system_clock::now().time_since_epoch().count() - startTime)/1000000 << " "
        << rec->id << " " 
        << rec->ax << " " 
        << rec->ay << " " 
        << rec->az << " "
        << rec->gx << " " 
        << rec->gy << " " 
        << rec->gz << " "
        << rec->mx << " " 
        << rec->my << " " 
        << rec->mz << "\n";
}

void showPacketCount() {
    for (int i = 0; i < 256; i++) {
        int c = packets[i];
        if (c)
            cout << i << ":" << packets[i] << ",";
    }
    cout << endl;
}

void* clientThread(void* _args) {
    ThreadArgs* args = (ThreadArgs*)_args;
    memset(args->recordBuffer, 0, sizeof(Record));
    while (1) {
        int tmp = receiveDataTCP(args->clientSocket, (char*)(args->recordBuffer), sizeof(Record));
        if (0 >= tmp) break;
        if (tmp != sizeof(Record)) continue;
        if ((args->recordBuffer->id <= 0) || (args->recordBuffer->id > 255)) continue;
        if ((packets[args->recordBuffer->id] % 1) == 0) {
            cout << packets[args->recordBuffer->id] << " ";
            printRecord(args->recordBuffer);
            // showPacketCount();
        }
        packets[args->recordBuffer->id]++;
    }
    cout << "Thread offline " <<  args->clientSocket << endl;
    close(args->clientSocket);
    delete args->recordBuffer;
    delete args;
    return 0;
}


int main() {
    Socket sock = createSocketTCP();
    SocketProps* props = createSocketProps(1234);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 0, sizeof(int));
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    startTime = std::chrono::system_clock::now().time_since_epoch().count();
    cout << sock << endl;
    bindSocket(sock, props);
    listenSocket(sock);
    while(1) {
        cout << "Waiting" << endl;
        Socket clientSocket = acceptSocket(sock);
        cout << "Client connected " << clientSocket << endl;
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        ThreadArgs* args = new ThreadArgs;
        args->clientSocket = clientSocket;
        args->recordBuffer = new Record;
        pthread_t thread_ptr;
        pthread_create(&thread_ptr, NULL, clientThread, args);
    }
    return 0;
}
