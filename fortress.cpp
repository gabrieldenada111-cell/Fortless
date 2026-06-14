#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <sys/select.h>
#include <ifaddrs.h>
#include <map>
#include <ctime>

using namespace std;

const string RESET   = "\033[0m";
const string VERDE   = "\033[32m";
const string VERMELHO= "\033[31m";
const string CIANO   = "\033[36m";
const string AMARELO = "\033[33m";
const string NEGRITO = "\033[1m";

vector<int> sockets_armadilha;
unsigned long long total_bloqueado = 0;
string caminho_salvamento = "/home/gabriel/Downloads/Zodiac_Intrusos_Bloqueados.txt";

map<string, vector<time_t>> historico_scans;

void desligar_fortaleza(int sinal) {
    cout << "\n\n" << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << AMARELO << NEGRITO << "             [ ZODIAC FORTRESS - ARMA WEB CLOUDFLARE DESATIVADA ]" << RESET << endl;
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    for (int sock : sockets_armadilha) close(sock);
    exit(sinal);
}

string obter_ip_local() {
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;
    string ip_detectado = "127.0.0.1";

    if (getifaddrs(&interfaces) == 0) {
        ifa = interfaces;
        while (ifa != NULL) {
            if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET) {
                tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                string nome_interface = ifa->ifa_name;
                if (nome_interface != "lo") {
                    ip_detectado = string(addressBuffer);
                    break;
                }
            }
            ifa = ifa->ifa_next;
        }
    }
    if (interfaces) freeifaddrs(interfaces);
    return ip_detectado;
}

bool detectar_nmap(const string& ip) {
    time_t agora = time(0);
    historico_scans[ip].push_back(agora);
    vector<time_t>& tempos = historico_scans[ip];
    while (!tempos.empty() && agora - tempos.front() > 5) {
        tempos.erase(tempos.begin());
    }
    return (tempos.size() > 3);
}

int main() {
    signal(SIGINT, desligar_fortaleza);
    system("clear");

    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << VERDE << "                 /\\                                /\\" << endl;
    cout << VERDE << "                /__\\       [ ZODIAC FORTRESS ]    /__\\" << endl;
    cout << VERDE << "               /\\  /\\       (CLOUDFLARE TRAP)     /\\  /\\" << endl;
    cout << VERDE << "              |_____|______|___________|_____|______|_____|" << RESET << endl;
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;

    string meu_ip = obter_ip_local();
    cout << VERDE << NEGRITO << "\n[📡 SECURITY] MONITOR CLOUDFLARE ATIVO NO HOST: " << AMARELO << meu_ip << RESET << endl;

    // Monitorando a porta 8080 (Livre para testes web no Firefox) e a 25565 de jogos
    vector<int> portas_armadilha = {8080, 25565};
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
        cout << VERDE << "    [+] Escudo ativado na porta local: " << AMARELO << porta << RESET << endl;
    }

    // Template HTML customizado com o Design oficial da página de validação Cloudflare
    string html_cloudflare = 
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
        "<!DOCTYPE html><html><head><title>Just a moment...</title>"
        "<style>"
        "  body { background-color: #f3f3f3; color: #313131; font-family: -apple-system, system-ui, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; padding-top: 80px; text-align: left; padding-left: 15%; padding-right: 15%; }"
        "  @media (prefers-color-scheme: dark) { body { background-color: #1c1b1b; color: #d9d9d9; } }"
        "  .cf-wrapper { max-width: 1000px; margin: 0 auto; }"
        "  h1 { font-size: 32px; font-weight: 500; margin-bottom: 10px; color: #f38020; }"
        "  .cf-captcha-container { background-color: #ffffff; border: 1px solid #e0e0e0; padding: 20px; border-radius: 4px; display: inline-block; margin-top: 25px; box-shadow: 0 4px 6px rgba(0,0,0,0.05); }"
        "  @media (prefers-color-scheme: dark) { .cf-captcha-container { background-color: #2b2a2a; border-color: #403f3f; } }"
        "  .checkbox-container { display: flex; align-items: center; font-size: 16px; color: #313131; }"
        "  @media (prefers-color-scheme: dark) { .checkbox-container { color: #d9d9d9; } }"
        "  input[type='checkbox'] { width: 28px; height: 28px; margin-right: 14px; cursor: pointer; border: 2px solid #ccc; border-radius: 4px; }"
        "  .footer { margin-top: 60px; font-size: 12px; color: #9c9c9c; border-top: 1px solid #e0e0e0; padding-top: 15px; display: flex; align-items: center; }"
        "  @media (prefers-color-scheme: dark) { .footer { border-top-color: #403f3f; } }"
        "  .cf-logo { background-image: url('https://wikimedia.org'); background-size: contain; background-repeat: no-repeat; width: 100px; height: 30px; margin-right: 15px; }"
        "</style></head>"
        "<body>"
        "  <div class='cf-wrapper'>"
        "    <h1>Checking if the site connection is secure</h1>"
        "    <p style='font-size:18px; color:#858585;'>Validating your browser session via Cloudflare node. Please wait...</p>"
        "    "
        "    <div class='cf-captcha-container'>"
        "      <div class='checkbox-container'>"
        "        <input type='checkbox' id='cf-check' onclick='alert(\"Success: Browser verified by Cloudflare WAF.\")'>"
        "        <label for='cf-check'><b>Verify you are human</b></label>"
        "      </div>"
        "    </div>"
        "    "
        "    <div class='footer'>"
        "      <div class='cf-logo'></div>"
        "      <div>Performance & security by Cloudflare. Ray ID: " + to_string(time(0)) + "</div>"
        "    </div>"
        "  </div>"
        "</body></html>";

    cout << VERDE << NEGRITO << "\n[+] ESCUDO ATIVO ONLINE! Monitorando intrusões locais..." << RESET << endl;

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

                    if (detectar_nmap(ip_intruso)) {
                        cout << VERMELHO << NEGRITO << "\n\a[🚨 ALERTA: SCANNER DETECTADO 🦅] ➔ ATTACK NA REDE!" << RESET << endl;
                        cout << VERMELHO << "   ➔ Host Origem: " << AMARELO << ip_intruso << VERMELHO << " esta executando varredura automatizada!" << RESET << endl;
                    } else {
                        cout << VERMELHO << NEGRITO << "\n\a[🚨 ZODIAC TRAP DETECTED 🦅] ➔ INTERCEPTAÇÃO DE HOST!" << RESET << endl;
                        cout << CIANO << "   ➔ IP do Intruso:       " << AMARELO << ip_intruso << RESET << endl;
                        cout << CIANO << "   ➔ Porta Solicitada:    " << VERMELHO << porta_atacada << RESET << endl;
                    }

                    char req_buffer[1024];
                    memset(req_buffer, 0, sizeof(req_buffer));
                    int r_bytes = recv(client_sock, req_buffer, sizeof(req_buffer) - 1, 0);
                    
                    if (r_bytes > 0) {
                        string requisicao(req_buffer);
                        
                        if (requisicao.find("'") != string::npos || requisicao.find("OR") != string::npos) {
                            cout << VERMELHO << NEGRITO << "   [🔥 ALERTA DE EXPLOIT] ➔ Tentativa de comando SQL detectada de " << ip_intruso << "!" << RESET << endl;
                        }

                        // Envia a página Cloudflare estilizada de volta para o Firefox
                        send(client_sock, html_cloudflare.c_str(), html_cloudflare.length(), 0);
                    }

                    ofstream log(caminho_salvamento, ios::app);
                    if (log.is_open()) {
                        log << "[🚨 Ameaça Monitorada] IP: " << ip_intruso << " na Porta: " << porta_atacada << "\n";
log.close(); 
        }
                    close(client_sock);
                }
            }
        }
    }
    return 0;
}
