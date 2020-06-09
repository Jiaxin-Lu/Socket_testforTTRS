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
//#include "cryptopp/aes.h"
//using namespace CryptoPP;
using std::string;
using std::cerr;
using std::endl;
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
    char buffer[BUFFER_LEN], buffer_out[BUFFER_LEN];
    #define PORT 2333

    void PREP();
    void SEND_INFO(const string &bufout);
    char get_nx_char();
    string GET_INFO(string &com);
    void CLOSE();

    void PREP()
    {
        serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        

// 解决在close之后会有一个WAIT_TIME，导致bind失败的问题
    	int val = 1;
    	int ret = setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,(void *)&val,sizeof(int));
    	if(ret == -1)
    	{
            printf("setsockopt");
            exit(1);
   	    }
           
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(PORT);
        
        int flag_bind = bind(serv_sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
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

        socklen_t length = sizeof(client_addr);
    }

    char get_nx_char()
    {
        if (buffer_pos >= buffer_size)
        {
            memset(buffer,0,sizeof(buffer));
            buffer_size = recv(clnt_sock, buffer, sizeof(buffer), 0);
            buffer_pos = 0;
        }
        return buffer[buffer_pos ++];
    }

    string GET_INFO()
    {
        cerr << "START RECIEVING" << endl;
        clnt_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &length);
        if (clnt_sock < 0) return "";
        memset(buffer,0,sizeof(buffer));
        buffer_size = recv(clnt_sock, buffer, sizeof(buffer), 0);
        if (buffer_size == 0) return "";
        
        //#*#command#?#

        buffer_pos = 0;
        char ch = get_nx_char();
        while (true)
        {
            while (ch != '#') ch = get_nx_char();
            ch = get_nx_char();
            if (ch == '*')
            {
                ch = get_nx_char();
                if (ch == '#') break;
            }
        }
        //read the head "#*#"

        /*int length_of_command = 0;
        for (ch=get_nx_char();ch>='0' && ch<='9';ch=get_nx_char())
            length_of_command = length_of_command*10 + ch-'0';
        //read the length

        string _command = "";
        for (int i = 0;i < length_of_command; ++i)
        {
            ch = get_nx_char();
            _command = _command + ch;
        }
        //read the command*/
        string _command = "";
        while (true)
        {
            while ((ch = get_nx_char()) != '#') _command = _command + ch;
            ch = get_nx_char();
            if (ch == '?')
            {
                ch = get_nx_char();
                if (ch == '#') break;
                else
                {
                    _command = _command + "#?" + ch; continue;
                }
            }
            else
            {
                _command = _command + '#' + ch; continue;
            }
        }

        //decode the command
        string command = _command;
        //unfinished

        //EXECUTOR(command);
        cerr << "command : " << command << endl;
        return command;
    }

    void SEND_INFO(const string &bufout)//string or char. need to change
    {
        //should encrypt the bufout first!
        //unfinished!

        //#*xxx#out
        memset(buffer_out,0,sizeof(buffer_out));
        length_of_out = bufout.length();//or strlen(bufout);
        buffer_out[0] = '#', buffer_out[1] = '*', buffer_out[2] = '#';
        //char length_of_out_ch[10];
        //sprintf(length_of_out_ch,"%d",length_of_out);
        //int len_lenofout = strlen(length_of_out_ch);
        //for (int i=0;i<len_lenofout;++i)
        //    buffer_out[i+2] = length_of_out_ch[i];
        //buffer_out[len_lenofout+2] = '#';
        for (int i=0;i<length_of_out;++i)
            buffer_out[i+3] = bufout[i];
        buffer_out[length_of_out+3] = '#', buffer_out[length_of_out+4] = '?', buffer_out[length_of_out+5] = '#';

        cerr << "SEND INFO : " << buffer_out << endl;

        int send_sock = send(clnt_sock, buffer_out, strlen(buffer_out), 0);
        memset(buffer_out,0,sizeof(buffer_out));
        length_of_out = 0;
        usleep(1000);

        cerr << "SEND DONE!" << endl;
	//shutdown(clnt_sock,SHUT_RD);
	//shutdown(serv_sock,SHUT_RD);
    }

    void CLOSE()
    {
        close(serv_sock);
	close(clnt_sock);
    }
};


int main()
{
    string command;
    while (true)
    {
        _CON::PREP();
        cerr << "socket done!" << endl;
        command = _CON::GET_INFO();
        if (command != "")
        {
            cerr << "out command : " << command << endl;
            _CON::SEND_INFO(command);
        }
        _CON::CLOSE();
    }
    return 0;
}
