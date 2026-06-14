#include <iostream>
#include <fstream> // CORREÇÃO: Adicionado para permitir o funcionamento do ofstream log
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

// Estrutura para detectar Nmap (Várias conexões do mesmo IP em pouco tempo)
map<string, vector<time_t>> historico_scans;

void desligar_fortaleza(int sinal) {
    cout << "\n\n" << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << AMARELO << NEGRITO << "             [ ZODIAC FORTRESS - ESCUDO INTEGRADO DESATIVADO ]" << RESET << endl;
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << CIANO << "[*] Total de ameaças/varreduras barradas: " << total_bloqueado << RESET << endl;
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

// Função para analisar e detectar se o comportamento do IP se parece com o Nmap
bool detectar_nmap(const string& ip) {
    time_t agora = time(0);
    historico_scans[ip].push_back(agora);

    // Remove registros mais velhos que 5 segundos
    vector<time_t>& tempos = historico_scans[ip];
    while (!tempos.empty() && agora - tempos.front() > 5) {
        tempos.erase(tempos.begin());
    }

    // Se o mesmo IP bateu mais de 3 vezes em menos de 5 segundos, é um Scanner de Portas ativo
    return (tempos.size() > 3);
}

int main() {
    signal(SIGINT, desligar_fortaleza);
    system("clear");

    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << VERDE << "                 /\\                                /\\" << endl;
    cout << VERDE << "                /__\\       [ ZODIAC FORTRESS ]    /__\\" << endl;
    cout << VERDE << "               /\\  /\\       (INTELLIGENT IDS)     /\\  /\\" << endl;
    cout << VERDE << "              |_____|______|___________|_____|______|_____|" << RESET << endl;
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;

    string meu_ip = obter_ip_local();
    cout << VERDE << NEGRITO << "\n[📡 SECURITY] MONITOR INTEGRADO ATIVO NO IP: " << AMARELO << meu_ip << RESET << endl;
    cout << CIANO << "[*] Monitore o alarme abaixo. Proteção de portas em tempo real ligada.\n" << RESET << endl;

    // Escuta nas principais portas de monitoramento (8080 para não conflitar com seu Apache Debian padrão)
    vector<int> portas_armadilha = {8080, 25565, 21, 22, 23};
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
        cout << VERDE << "    [+] Escudo ativado com sucesso no host/porta: " << AMARELO << porta << RESET << endl;
    }

    // Template HTML do CAPTCHA ZODIAC
    string html_captcha = 
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
        "<!DOCTYPE html><html><head><title>Zodiac Firewall</title>"
        "<style>"
        "  body { background-color: #0d0d0d; color: #ffffff; font-family: monospace; text-align: center; padding-top: 100px; }"
        "  .box { background-color: #1a1a1a; border: 1px solid #ff3333; display: inline-block; padding: 30px; border-radius: 8px; }"
        "  input { width: 24px; height: 24px; cursor: pointer; }"
        "</style></head>"
        "<body><div class='box'><h1>[🛡️] CAPTCHA DE SEGURANÇA ZODIAC [🛡️]</h1>"
        "<p>Verificação de integridade de IP exigida.</p>"
        "<div style='display:flex; align-items:center; justify-content:center; margin-top:20px;'>"
        "  <input type='checkbox' id='c' onclick='alert(\"Aparelho validado administrativamente.\")'>"
        "  <label for='c'><b>  Sou humano (Validar Acesso)</b></label>"
        "</div></div></body></html>";

    cout << VERDE << NEGRITO << "\n[+] ESCUDO ATIVO ONLINE! Monitorando intrusões..." << RESET << endl;

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

                    // 1. CHECAGEM DE SCANNER REAL (NMAP)
                    if (detectar_nmap(ip_intruso)) {
                        cout << VERMELHO << NEGRITO << "\n\a[🚨 ALERTA: SCANNER DETECTADO 🦅] ➔ ATTACK NA REDE!" << RESET << endl;
                        cout << VERMELHO << "   ➔ Host Origem: " << AMARELO << ip_intruso << VERMELHO << " está executando varredura em massa (Nmap)!" << RESET << endl;
                    } else {
                        cout << VERMELHO << NEGRITO << "\n\a[🚨 ZODIAC TRAP DETECTED 🦅] ➔ INTERCEPTAÇÃO DE HOST!" << RESET << endl;
                        cout << CIANO << "   ➔ IP do Intruso:       " << AMARELO << ip_intruso << RESET << endl;
                        cout << CIANO << "   ➔ Porta Solicitada:    " << VERMELHO << porta_atacada << RESET << endl;
                    }

                    // 2. CAPTURA E ANÁLISE DE EXPLOIT WEB (SQL/Injeção)
                    char req_buffer[2048];
                    memset(req_buffer, 0, sizeof(req_buffer));
                    int r_bytes = recv(client_sock, req_buffer, sizeof(req_buffer) - 1, 0);
                    
                    if (r_bytes > 0) {
                        string requisicao(req_buffer);
                        
                        if (requisicao.find("'") != string::npos || requisicao.find("OR") != string::npos || requisicao.find("select") != string::npos) {
                            cout << VERMELHO << NEGRITO << "   [🔥 ALERTA DE EXPLOIT] ➔ Tentativa de injeção de código (SQL) detectada de " << ip_intruso << "!" << RESET << endl;
                        }

                        // Entrega o CAPTCHA via Socket de volta para o Navegador do alvo
                        send(client_sock, html_captcha.c_str(), html_captcha.length(), 0);
                    }

                    // 3. SALVAMENTO AUTOMÁTICO EM LOG NO DISCO
                    ofstream log(caminho_salvamento, ios::app);
                    if (log.is_open()) {
                        log << "[🚨 Ameaça Bloqueada] IP: " << ip_intruso << " na Porta: " << porta_atacada << "\n";
                        log.close();
                    }

                    close(client_sock);
                }
            }
        }
    }
    return 0;
}
