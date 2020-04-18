#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <pthread.h>

#include "AES.hpp"


void print(unsigned char* state, int len)
{
	int i;
	for(i=0; i<len; i++)
	{
        if ( i%16 == 0) printf("\n");
//		printf("%s%X ",state[i]>15 ? "" : "0", state[i]);
        printf("%d  ", (int)(state[i] & 0xff));
	}
	printf("\n");
}

using std::cerr;
using std::cin;
using std::endl;
using std::string;
namespace _CON
{
int serv_sock, clnt_sock;
const int LISTEN_LEN = 9000;
const int BUFFER_LEN = 10005;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
socklen_t length;
int buffer_pos = 0;
int length_of_out = 0;
int buffer_size;
char buffer[BUFFER_LEN];//, buffer_out[BUFFER_LEN];


AESModeOfOperation moo;

pthread_t thread;

#define PORT 2333

void EN_PREP();
void PREP();
void SEND_INFO(const string &bufout);
char get_nx_char();
string GET_INFO(string &com);
void CLOSE();
void EN_PREP()
{    
    unsigned char iv[]    = {103,35,148,239,76,213,47,118,255,222,123,176,106,134,98,92};
	unsigned char key[]   = {'z','h','u','a','n','g','y','o','n','g','h','a','o','t','q','l'};

	moo.set_key(key);
	moo.set_mode(MODE_OFB);
	moo.set_iv(iv);
}
string decrypt(string _com)
{
    int len = _com.length();
    unsigned char com[len] = {0};
    for (int i=0;i<len;++i) com[i] = _com[i];
    unsigned char bk[len << 1] = {0};
    int bk_len = moo.Decrypt(com, len, bk);
    string bk_com = "";
    for (int i=0;i<bk_len;++i) bk_com += com[i];
    return bk_com;
    //undo
}
string encrypt(string _com)
{
    int len = _com.length();
    unsigned char com[len] = {0};
    for (int i=0;i<len;++i) com[i] = _com[i];
    unsigned char bk[len << 1] = {0};
    int bk_len = moo.Encrypt(com, len, bk);
    string bk_com = "";
    for (int i=0;i<bk_len;++i) bk_com += com[i];
    return bk_com;
    //undo
}
void PREP()
{
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 解决在close之后会有一个WAIT_TIME，导致bind失败的问题
    int val = 1;
    int ret = setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(int));
    if (ret == -1)
    {
        printf("setsockopt");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    int flag_bind = bind(serv_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (flag_bind < 0)
    {
        perror("Bind!");
        exit(1);
    }
    //Do I need to check the connection?

    int flag_listen = listen(serv_sock, LISTEN_LEN);
    if (flag_listen < 0)
    {
        perror("Listen!");
        exit(1);
    }
    //Do I need to check the connection?

    //socklen_t length = sizeof(client_addr);
}

char get_nx_char()
{
    if (buffer_pos >= buffer_size)
    {
        memset(buffer, 0, sizeof(buffer));
        buffer_size = recv(clnt_sock, buffer, sizeof(buffer), 0);
        buffer_pos = 0;
    }
    return buffer[buffer_pos++];
}

string GET_INFO()
{
    //#*#command#?#

    buffer_pos = 0;
    char ch = get_nx_char();
    while (true)
    {
        while (ch != '#')
            ch = get_nx_char();
        ch = get_nx_char();
        if (ch == '*')
        {
            ch = get_nx_char();
            if (ch == '#')
                break;
        }
    }
    //read the head "#*#"
    string _command = "";
    while (true)
    {
        while ((ch = get_nx_char()) != '#')
            _command = _command + ch;
        ch = get_nx_char();
        if (ch == '?')
        {
            ch = get_nx_char();
            if (ch == '#')
                break;
            else
            {
                _command = _command + "#?" + ch;
                continue;
            }
        }
        else
        {
            _command = _command + '#' + ch;
            continue;
        }
    }

    //decode the command
    string command = decrypt(_command);
    //unfinished

    //EXECUTOR(command);
    cerr << "command : " << command << endl;
    return command;
}

/*string SEND_INFO(const string &bufout) //string or char. need to change
{
    //should encrypt the bufout first!
    //unfinished!

    //#*xxx#out
    //memset(buffer_out, 0, sizeof(buffer_out));
    length_of_out = bufout.length(); //or strlen(bufout);
    string buffer_out = "#*#";
    for (int i=0;i<length_of_out;++i) buffer_out = buffer_out + bufout[i];
    buffer_out = buffer_out + "#?#\n";
    // char buffer_out[length_of_out+10];
    // buffer_out[0] = '#', buffer_out[1] = '*', buffer_out[2] = '#';
    // for (int i = 0; i < length_of_out; ++i)
    //     buffer_out[i + 3] = bufout[i];
    // buffer_out[length_of_out + 3] = '#', buffer_out[length_of_out + 4] = '?', buffer_out[length_of_out + 5] = '#', buffer_out[length_of_out + 6] = '\n';

    cerr << "SEND INFO : " << buffer_out << endl;

    return buffer_out;
    // int send_sock = send(clnt_sock, buffer_out, strlen(buffer_out), 0);
    // memset(buffer_out, 0, sizeof(buffer_out));
    // length_of_out = 0;
    // usleep(1000);

    // cerr << "SEND DONE!" << endl;
    //shutdown(clnt_sock,SHUT_WR);
    //shutdown(serv_sock,SHUT_RD);
}*/
void *start_routine(void *ptr) 
{
    int sock_fd = *(int *)ptr;
    //char buf[100];
    //int numbytes;
    int i,c=0;
    printf("this is a new thread,you got connected\n");
    printf("fd=%d\n",sock_fd);

    //recv

    memset(buffer, 0, sizeof(buffer));
    buffer_size = recv(clnt_sock, buffer, sizeof(buffer), 0);
    if (buffer_size == 0)
    {
        perror("buffer_size error!");
        close(sock_fd);
        return 0;//exit(1);
    }

    //recv

    string _command = GET_INFO();
    string command = encrypt(_command);
    //SEND_INFO(command);

    string send_command = "#*#" + command + "#?#";
    int len_send_command = send_command.length();
    char buffer_out[len_send_command + 5];
    for (int i=0;i<len_send_command;++i) buffer_out[i] = send_command[i];
    buffer_out[len_send_command] = '\n';

    
    printf("fd=%d  ",sock_fd);
    cerr << buffer_out << endl;

    int send_sock = send(sock_fd, buffer_out, len_send_command+1, 0);
    cerr << "SEND DONE!" << endl;

    //     printf("receive message:%s\n",buf);
    // printf("numbytes=%d\n",numbytes);
    // for(i=0;i<numbytes;i++)
    // {
    //     str[i]=buf[numbytes-1-i];

    // }
    // printf("server will send:%s\n",str);
    // numbytes=send(fd,str,sizeof(str),0); 
    // printf("send numbytes=%d\n",numbytes);
    close(sock_fd);
}
void CLOSE()
{
    close(serv_sock);
    close(clnt_sock);
}

void WORK()
{
    cerr << "START RECIEVING" << endl;
    clnt_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &length);
    if (clnt_sock < 0)
    {
        perror("clnt_sock error!");
        return;
    }
    pthread_create(&thread,NULL,start_routine,(void *)&clnt_sock);
}
}; // namespace _CON

int main()
{
    _CON::EN_PREP();
    // string command;
    // string _command;
    _CON::PREP();
    while (true)
    {
        _CON::WORK();
    }
    _CON::CLOSE();
    return 0;
}
