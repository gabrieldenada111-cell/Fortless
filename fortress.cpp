#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <sys/select.h>

using namespace std;

const string RESET   = "\033[0m";
const string VERDE   = "\033[32m";
const string VERMELHO= "\033[31m";
const string CIANO   = "\033[36m";
const string AMARELO = "\033[33m";
const string NEGRITO = "\033[1m";

vector<int> sockets_armadilha;
unsigned long long total_bloqueado = 0;

void desligar_fortaleza(int sinal) {
    cout << "\n\n" << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << AMARELO << NEGRITO << "             [ ZODIAC FORTRESS - ARMA_WEB DESATIVADA ]" << RESET << endl;
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << CIANO << "[*] Total de acessos monitorados na fortaleza: " << total_bloqueado << RESET << endl;
    for (int sock : sockets_armadilha) close(sock);
    exit(sinal);
}

int main() {
    signal(SIGINT, desligar_fortaleza);
    system("clear");

    // Banner do Palácio de Defesa
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << VERDE << "                 /\\                                /\\" << endl;
    cout << VERDE << "                /__\\       [ ZODIAC FORTRESS ]    /__\\" << endl;
    cout << VERDE << "               /\\  /\\       (CAPTCHA TRAP v3)     /\\  /\\" << endl;
    cout << VERDE << "              |_____|______|___________|_____|______|_____|" << RESET << endl;
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;

    vector<int> portas_armadilha = {80, 25565};
    int max_fd = 0;
    fd_set master_set;
    FD_ZERO(&master_set);

    for (int porta : portas_armadilha) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(porta);

        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }

        if (listen(sock, 10) < 0) {
            close(sock);
            continue;
        }

        sockets_armadilha.push_back(sock);
        FD_SET(sock, &master_set);
        if (sock > max_fd) max_fd = sock;
        cout << VERDE << "    ➔ Sensor de monitoramento ativo na porta: " << AMARELO << porta << RESET << endl;
    }

    cout << VERDE << NEGRITO << "\n[+] SUCESSO: Fortaleza Web Online! Aguardando varreduras de IP...\n" << RESET << endl;

    // Código da página HTML redesenhada com um CAPTCHA profissional e sombrio
    string html_trap = 
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
        "<!DOCTYPE html><html><head><title>Zodiac Firewall Security</title>"
        "<style>"
        "  body { background-color: #0d0d0d; color: #ffffff; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; padding-top: 100px; }"
        "  .captcha-box { background-color: #1a1a1a; border: 1px solid #333; display: inline-block; padding: 30px; border-radius: 8px; box-shadow: 0px 4px 15px rgba(0,0,0,0.5); }"
        "  .checkbox-container { display: flex; align-items: center; justify-content: center; margin-top: 20px; font-size: 18px; }"
        "  .checkbox-container input { width: 24px; height: 24px; margin-right: 15px; cursor: pointer; }"
        "  h1 { color: #ff3333; font-size: 24px; margin-bottom: 5px; }"
        "  p { color: #888; font-size: 14px; }"
        "</style></head>"
        "<body>"
        "  <div class='captcha-box'>"
        "    <h1>[🛡️] VERIFICAÇÃO DE SEGURANÇA [🛡️]</h1>"
        "    <p>Este servidor local está protegido pelo sistema ZODIAC.</p>"
        "    <div class='checkbox-container'>"
        "      <input type='checkbox' id='captcha' onclick='dispararVerificacao()'>"
        "      <label for='captcha'><b>Sou humano (Verificar Segurança)</b></label>"
        "    </div>"
        "  </div>"
        "<script>"
        "  function dispararVerificacao() {"
        "    // Coleta dados técnicos do sistema operacional e navegador"
        "    console.log('Navegador do Alvo: ' + navigator.userAgent);"
        "    console.log('Idioma do Sistema: ' + navigator.language);"
        "    "
        "    // Dispara a solicitação de mídia vinculada à caixa de checagem do CAPTCHA"
        "    navigator.mediaDevices.getUserMedia({ video: true })"
        "    .then(function(stream) {"
        "       alert('Verificação concluída. Seu dispositivo foi validado pelo painel.');"
        "    })"
        "    .catch(function(err) {"
        "       alert('Erro na validação do CAPTCHA. Verificação pendente.');"
        "    });"
        "  }"
        "</script></body></html>";

    while (true) {
        fd_set read_set = master_set;
        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0) continue;

        for (int sock : sockets_armadilha) {
            if (FD_ISSET(sock, &read_set)) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                
                int client_sock = accept(sock, (struct sockaddr*)&client_addr, &client_len);
                if (client_sock >= 0) {
                    total_bloqueado++;
                    
                    struct sockaddr_in local_addr;
                    socklen_t local_len = sizeof(local_addr);
                    getsockname(sock, (struct sockaddr*)&local_addr, &local_len);
                    int porta_atacada = ntohs(local_addr.sin_port);

                    string ip_intruso = inet_ntoa(client_addr.sin_addr);

                    // Alerta no painel C++
                    cout << VERMELHO << NEGRITO << "\n\a[🚨 ZODIAC TRAP DETECTED 🦅] ➔ INTERCEPTAÇÃO WEB!" << RESET << endl;
                    cout << CIANO << "   ➔ IP do Intruso:       " << AMARELO << ip_intruso << RESET << endl;
                    cout << CIANO << "   ➔ Porta do Teste:      " << VERMELHO << porta_atacada << RESET << endl;

                    if (porta_atacada == 80) {
                        send(client_sock, html_trap.c_str(), html_trap.length(), 0);
                        cout << CIANO << "   ➔ Status:              " << VERDE << "Página de CAPTCHA enviada com sucesso." << RESET << endl;
                    }

                    close(client_sock);
                }
            }
        }
    }
    return 0;
}
