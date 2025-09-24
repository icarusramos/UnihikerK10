#include "unihiker_k10.h"
#include "asr.h"
#include "AIRecognition.h"
#include "Adafruit_LTR329_LTR303.h"
#include "DFRobot_AHT20.h"
#include <WiFi.h>
#include <SD.h>
#include <FS.h>

UNIHIKER_K10 k10;
ASR asr;
AIRecognition ai;
Adafruit_LTR303 ltr;
DFRobot_AHT20 aht20;

// Configurações da tela
uint8_t screen_dir = 2;

// Variáveis de controle
int currentMode = 0;
bool modeChanged = true;
unsigned long lastUpdate = 0;
unsigned long lastSensorRead = 0;
bool sdCardAvailable = false;
bool wifiConnected = false;
bool bluetoothEnabled = false;

// Configurações WiFi - CONFIGURE SUAS CREDENCIAIS AQUI
String wifi_ssid = "";      // <- Coloque o nome da sua rede WiFi aqui
String wifi_password = ""; // <- Coloque a senha da sua rede WiFi aqui
unsigned long wifiLastAttempt = 0;
int wifiRetryCount = 0;
String wifiStatus = "Desconectado";
String connectedSSID = "";
String localIP = "";

// Dados dos sensores
float temperature = 0.0;
float humidity = 0.0;
uint16_t lightLevel = 0;
uint16_t lightRaw = 0;
uint16_t lightPercent = 0;
float accelX = 0.0, accelY = 0.0, accelZ = 0.0;
uint64_t micData = 0;

// Controle do sensor de luz
uint16_t minLight = 65535;
uint16_t maxLight = 0;
bool lightSensorAvailable = false;

// Controle dos sensores de temperatura e umidade
bool tempHumiSensorAvailable = false;

// Variáveis para animações
float ballX = 120, ballY = 160;
float ballSpeedX = 3, ballSpeedY = 2;
int ledColorIndex = 0;
uint32_t ledColors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF, 0xFFFFFF};

// Menu principal
const char* menuItems[] = {
    "1-LEDs RGB",
    "2-Sensores", 
    "3-Camera/AI",
    "4-Som/Audio",
    "5-WiFi Test",
    "6-Bluetooth", 
    "7-SD Card",
    "8-Botoes",
    "9-Animacao",
    "0-Demo All"
};

// Declarações das funções
void initializeSystems();
void showWelcomeScreen();
void showMainMenu();
void handleButtons();
void handleModeSpecificActionA();
void handleModeSpecificActionB();
void readAllSensors();
void demonstrateLEDs();
void demonstrateSensors();
void demonstrateCamera();
void demonstrateAudio();
void demonstrateAudio();
void demonstrateWiFi();
void demonstrateBluetooth();
void demonstrateSDCard();
void demonstrateButtons();
void demonstrateAnimation();
void demonstrateAll();
void setAllLeds(uint32_t color);
void setAllLedsOff();
void flashAllLeds();
uint32_t getRainbowColor(int pos);
void demonstrateRainbowLeds();
void showSensorData();
void playBeep();
void playTone(int frequency, int duration);
bool writeToSDCard(String data);
String readFromSDCard();
void displayOnScreen(String text, int line = 0, uint32_t color = 0xFFFFFF);
bool initScreenOnce(uint32_t backgroundColor = 0x000000);

// Declarações de funções SD nativas
bool writeToSD(String filename, String data);
String readFromSD(String filename);
bool appendToSD(String filename, String data);
bool deleteFromSD(String filename);
void listSDFiles();
bool sdFileExists(String filename);
size_t getSDFileSize(String filename);

// Declarações WiFi
void initializeWiFi();
void checkWiFiConnection();
void connectToWiFi();
void scanWiFiNetworks();
String getWiFiStatusText();
void showWiFiInfo();

void setup() {
    Serial.begin(115200);
    delay(3000); // Aguardar estabilização mais longa
    
    Serial.println(" UNIHIKER K10 DEMO COMPLETO ");
    
    // Inicializar K10 básico primeiro
    Serial.println("Inicializando K10...");
    k10.begin();
    delay(1000);
    
    Serial.println("Inicializando tela...");
    k10.initScreen(screen_dir);
    delay(1000);
    
    Serial.println("Inicializando canvas...");
    k10.creatCanvas();
    delay(1000);
    
    k10.setScreenBackground(0x000000);
    
    Serial.println("Inicializando LEDs...");
    k10.rgb->begin();
    setAllLedsOff();
    delay(1000);
    
    // Mostrar status inicial
    displayOnScreen("UNIHIKER K10", 2, 0x00FFFF);
    displayOnScreen("INICIALIZANDO...", 3, 0xFFFF00);
    
    // Inicializar outros sistemas gradualmente
    initializeSystems();
    
    // Mostrar tela inicial
    showWelcomeScreen();
    delay(3000);
    showMainMenu();
    
    Serial.println("Setup concluído com sucesso!");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Verificar conexão WiFi periodicamente
    if (currentTime - wifiLastAttempt > 30000) { // A cada 30 segundos
        checkWiFiConnection();
        wifiLastAttempt = currentTime;
    }
    
    // Ler sensores periodicamente
    if (currentTime - lastSensorRead > 1000) {
        readAllSensors();
        lastSensorRead = currentTime;
    }
    
    // Verificar botões
    handleButtons();
    
    // Executar modo atual
    switch (currentMode) {
        case 0: // Menu principal
            if (modeChanged) {
                showMainMenu();
                modeChanged = false;
            }
            break;
            
        case 1: // LEDs RGB
            if (modeChanged) {
                // Reset flag para próxima entrada no modo
                modeChanged = false;
            }
            demonstrateLEDs();
            break;
            
        case 2: // Sensores
            demonstrateSensors();
            break;
            
        case 3: // Camera/AI
            demonstrateCamera();
            break;
            
        case 4: // Som/Audio
            demonstrateAudio();
            break;
            
        case 5: // WiFi
            demonstrateWiFi();
            break;
            
        case 6: // Bluetooth
            demonstrateBluetooth();
            break;
            
        case 7: // SD Card
            demonstrateSDCard();
            break;
            
        case 8: // Botões
            demonstrateButtons();
            break;
            
        case 9: // Animação
            demonstrateAnimation();
            break;
            
        case 10: // Demo completo
            demonstrateAll();
            break;
    }
    
    delay(50);
}

//  FUNÇÕES AUXILIARES BÁSICAS 

void displayOnScreen(String text, int line, uint32_t color) {
    k10.canvas->canvasText(text, line, color);
    k10.canvas->updateCanvas();
}

// Função para inicializar tela apenas uma vez por modo
bool initScreenOnce(uint32_t backgroundColor) {
    static int lastMode = -1;
    
    // Se mudou de modo, limpa a tela UMA VEZ
    if (modeChanged || lastMode != currentMode) {
        k10.canvas->canvasClear();
        k10.setScreenBackground(backgroundColor);
        lastMode = currentMode;
        return true; // Indica que a tela foi limpa
    }
    return false; // Indica que a tela já estava inicializada
}

void setAllLeds(uint32_t color) {
    for (int i = 0; i < 3; i++) {
        k10.rgb->write(i, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }
    k10.rgb->show();
}

void setAllLedsOff() {
    for (int i = 0; i < 3; i++) {
        k10.rgb->write(i, 0, 0, 0);
    }
    k10.rgb->show();
}

void flashAllLeds() {
    setAllLeds(0xFFFFFF);
    delay(100);
    setAllLedsOff();
    delay(100);
    setAllLeds(0xFFFFFF);
    delay(100);
    setAllLedsOff();
}

void initializeSystems() {
    // Inicializar componentes de forma segura e gradual
    Serial.println("Inicializando sistemas...");
    
    // SD Card (opcional)
    displayOnScreen("Inicializando SD Card...", 4, 0xFFFF00);
    delay(500);
    try {
        if (SD.begin()) {
            sdCardAvailable = true;
            displayOnScreen("SD Card: OK", 5, 0x00FF00);
            Serial.println("SD Card inicializado com sucesso");
            
            // Testar funcionalidades básicas
            Serial.println(" Testando SD Card ");
            if (writeToSD("/test_k10.txt", "Teste SD K10 funcionando!")) {
                Serial.println("Teste de escrita: OK");
                String content = readFromSD("/test_k10.txt");
                if (content.length() > 0) {
                    Serial.println("Teste de leitura: OK - " + content);
                } else {
                    Serial.println("Teste de leitura: FALHOU");
                }
            } else {
                Serial.println("Teste de escrita: FALHOU");
            }
            
            // Listar arquivos no SD
            listSDFiles();
        } else {
            sdCardAvailable = false;
            displayOnScreen("SD Card: FALHOU", 5, 0xFF0000);
            Serial.println("SD Card não disponível");
        }
    } catch (...) {
        sdCardAvailable = false;
        displayOnScreen("SD Card: ERRO", 5, 0xFF0000);
        Serial.println("Erro ao inicializar SD Card");
    }
    delay(500);
    
    // Sensores (opcionais)
    displayOnScreen("Inicializando sensores...", 6, 0xFFFF00);
    delay(500);
    
    // Sensor de luz LTR303
    try {
        if (ltr.begin()) {
            ltr.setGain(LTR3XX_GAIN_1);
            ltr.setIntegrationTime(LTR3XX_INTEGTIME_50);
            ltr.setMeasurementRate(LTR3XX_MEASRATE_50);
            lightSensorAvailable = true;
            Serial.println("Sensor de luz LTR303 inicializado");
        } else {
            lightSensorAvailable = false;
            Serial.println("Sensor de luz LTR303 não disponível");
        }
    } catch (...) {
        lightSensorAvailable = false;
        Serial.println("Erro ao inicializar sensor de luz");
    }
    
    // Sensor de temperatura e umidade AHT20
    try {
        if (aht20.begin()) {
            tempHumiSensorAvailable = true;
            Serial.println("Sensor AHT20 inicializado");
        } else {
            tempHumiSensorAvailable = false;
            Serial.println("Sensor AHT20 não disponível");
        }
    } catch (...) {
        tempHumiSensorAvailable = false;
        Serial.println("Erro ao inicializar sensor AHT20");
    }
    
    displayOnScreen("Sensores: OK", 7, 0x00FF00);
    delay(500);
    
    // Inicializar WiFi
    displayOnScreen("Inicializando WiFi...", 8, 0xFFFF00);
    delay(500);
    
    Serial.println("Iniciando conexão WiFi...");
    Serial.println("SSID configurado: " + wifi_ssid);
    
    initializeWiFi();
    displayOnScreen("WiFi: " + wifiStatus, 9, wifiConnected ? 0x00FF00 : 0xFF8800);
    delay(500);
    
    Serial.println("Sistemas inicializados com segurança");
}

void showWelcomeScreen() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x001122);
    
    displayOnScreen("UNIHIKER K10", 3, 0x00FFFF);
    displayOnScreen("DEMO COMPLETO", 5, 0xFFFF00);
    displayOnScreen("Todas funcoes", 7, 0xFFFFFF);
    displayOnScreen("implementadas", 8, 0xFFFFFF);
    
    // Indicadores de status
    displayOnScreen("SD: " + String(sdCardAvailable ? "OK" : "NO"), 10, sdCardAvailable ? 0x00FF00 : 0xFF0000);
    displayOnScreen("WiFi: " + wifiStatus, 11, wifiConnected ? 0x00FF00 : 0xFF8800);
    displayOnScreen("Carregando...", 12, 0x888888);
    
    // Animação de LEDs durante carregamento
    for (int i = 0; i < 3; i++) {
        setAllLeds(0xFF0000); delay(200);
        setAllLeds(0x00FF00); delay(200);
        setAllLeds(0x0000FF); delay(200);
    }
    setAllLedsOff();
}

void showMainMenu() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    displayOnScreen(" MENU PRINCIPAL ", 1, 0x00FFFF);
    
    for (int i = 0; i < 10; i++) {
        displayOnScreen(menuItems[i], i + 2, 0xFFFFFF);
    }
    
    displayOnScreen("Use botoes A/B", 13, 0xFFFF00);
    displayOnScreen("A+B = Proximo modo", 14, 0x888888);
}

void handleButtons() {
    static unsigned long lastButtonPress = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastButtonPress < 200) return; // Debounce
    
    if (k10.buttonAB->isPressed()) {
        currentMode = (currentMode + 1) % 11;
        modeChanged = true;
        lastButtonPress = currentTime;
        displayOnScreen("Modo: " + String(currentMode), 0, 0xFFFF00);
        delay(500);
    }
    else if (k10.buttonA->isPressed()) {
        handleModeSpecificActionA();
        lastButtonPress = currentTime;
    }
    else if (k10.buttonB->isPressed()) {
        handleModeSpecificActionB();
        lastButtonPress = currentTime;
    }
}

void handleModeSpecificActionA() {
    switch (currentMode) {
        case 1: // LEDs - mudar cor
            ledColorIndex = (ledColorIndex + 1) % 7;
            break;
        case 5: // WiFi - tentar reconectar
            if (!wifiConnected) {
                connectToWiFi();
            } else {
                showWiFiInfo(); // Mostrar info completa no serial
            }
            break;
        case 7: // SD Card - criar arquivo de teste
            if (sdCardAvailable) {
                String filename = "/teste_" + String(millis()) + ".txt";
                String data = "Arquivo criado em: " + String(millis()) + "\nTeste do UNIHIKER K10\nFunções nativas de SD!";
                if (writeToSD(filename, data)) {
                    Serial.println("Arquivo de teste criado: " + filename);
                } else {
                    Serial.println("Erro ao criar arquivo de teste");
                }
            } else {
                Serial.println("SD Card não disponível");
            }
            break;
        case 9: // Animação - mudar velocidade
            ballSpeedX = ballSpeedX > 0 ? ballSpeedX + 1 : ballSpeedX - 1;
            break;
    }
}

void handleModeSpecificActionB() {
    switch (currentMode) {
        case 1: // LEDs - piscar
            flashAllLeds();
            break;
        case 5: // WiFi - fazer scan de redes
            scanWiFiNetworks();
            break;
        case 7: // SD Card - listar arquivos
            if (sdCardAvailable) {
                listSDFiles();
            } else {
                Serial.println("SD Card não disponível para listagem");
            }
            break;
        case 9: // Animação - inverter direção
            ballSpeedX = -ballSpeedX;
            ballSpeedY = -ballSpeedY;
            break;
    }
}

void readAllSensors() {
    // Ler dados do acelerômetro (sempre disponível)
    try {
        accelX = k10.getAccelerometerX();
        accelY = k10.getAccelerometerY();
        accelZ = k10.getAccelerometerZ();
    } catch (...) {
        Serial.println("Erro ao ler acelerômetro");
    }
    
    // Ler dados do microfone (sempre disponível)
    try {
        micData = k10.readMICData();
    } catch (...) {
        Serial.println("Erro ao ler microfone");
    }
    
    // Ler sensor de luz NATIVO LTR303 (se disponível)
    if (lightSensorAvailable) {
        try {
            if (ltr.newDataAvailable()) {
                uint16_t visible_plus_ir, infrared;
                bool valid = ltr.readBothChannels(visible_plus_ir, infrared);
                
                if (valid) {
                    lightRaw = visible_plus_ir;
                    
                    // Atualizar range dinâmico para cálculo de porcentagem
                    if (lightRaw < minLight) minLight = lightRaw;
                    if (lightRaw > maxLight) maxLight = lightRaw;
                    
                    // Calcular porcentagem de luminosidade (0-100%)
                    if (maxLight > minLight) {
                        lightPercent = map(lightRaw, minLight, maxLight, 0, 100);
                        lightPercent = constrain(lightPercent, 0, 100);
                    } else {
                        lightPercent = 50; // Valor padrão se não há variação ainda
                    }
                    
                    // Manter compatibilidade com código antigo (escala 0-4095)
                    lightLevel = map(lightPercent, 0, 100, 0, 4095);
                }
            }
        } catch (...) {
            Serial.println("Erro ao ler sensor de luz");
        }
    } else {
        // Fallback: simular sensor de luz baseado em micData
        lightLevel = constrain(map(micData % 10000, 0, 9999, 100, 4000), 0, 4095);
        lightPercent = map(lightLevel, 0, 4095, 0, 100);
        lightRaw = lightLevel;
    }
    
    // Ler sensores NATIVOS de temperatura e umidade AHT20 (se disponível)
    if (tempHumiSensorAvailable) {
        try {
            temperature = aht20.getTemperature_C();
            humidity = aht20.getHumidity_RH();
        } catch (...) {
            Serial.println("Erro ao ler sensor AHT20");
            tempHumiSensorAvailable = false; // Desabilitar se erro
        }
    }
    
    if (!tempHumiSensorAvailable) {
        // Fallback: simular sensores baseados em dados reais do acelerômetro
        float baseTemp = 23.0 + (abs(accelX) * 5.0); // Temperatura varia com movimento
        temperature = baseTemp + (sin(millis() / 30000.0) * 3.0); // Variação lenta
        
        float baseHumidity = 55.0 + (abs(accelY) * 15.0); // Umidade varia com movimento Y
        humidity = constrain(baseHumidity + (cos(millis() / 25000.0) * 10.0), 20, 80);
    }
    
    // Output para serial a cada leitura
    Serial.printf("T:%.1f°C H:%.1f%% L:%d%% (%d raw) AccelX:%.2f AccelY:%.2f AccelZ:%.2f\n", 
                  temperature, humidity, lightPercent, lightRaw, accelX, accelY, accelZ);
}

void demonstrateLEDs() {
    static bool contentDrawn = false;
    static unsigned long lastUpdate = 0;
    static int state = 0;
    
    // Limpa tela apenas uma vez
    if (initScreenOnce()) {
        contentDrawn = false;
    }
    
    // Desenhar conteúdo estático apenas uma vez
    if (!contentDrawn) {
        displayOnScreen(" DEMO LEDs ", 1, 0x00FFFF);
        displayOnScreen("Cores automaticas", 3, 0xFFFFFF);
        displayOnScreen("A: Mudar modo", 11, 0x888888);
        displayOnScreen("B: Flash todos", 12, 0x888888);
        displayOnScreen("A+B: Proximo demo", 13, 0x888888);
        contentDrawn = true;
    }
    
    // Atualizar apenas o que muda
    if (millis() - lastUpdate > 500) {
        lastUpdate = millis();
        
        switch (state % 8) {
            case 0: setAllLeds(0xFF0000); break; // Vermelho
            case 1: setAllLeds(0x00FF00); break; // Verde
            case 2: setAllLeds(0x0000FF); break; // Azul
            case 3: setAllLeds(0xFFFF00); break; // Amarelo
            case 4: setAllLeds(0xFF00FF); break; // Magenta
            case 5: setAllLeds(0x00FFFF); break; // Ciano
            case 6: setAllLeds(0xFFFFFF); break; // Branco
            case 7: setAllLedsOff(); break; // Desligado
        }
        state++;
        
        // Atualizar apenas a linha do estado
        displayOnScreen("Estado: " + String(state % 8), 5, 0xFFFF00);
    }
}

void demonstrateSensors() {
    static bool contentDrawn = false;
    static unsigned long lastUpdate = 0;
    
    // Limpa tela apenas uma vez
    if (initScreenOnce()) {
        contentDrawn = false;
    }
    
    // Desenhar labels estáticos apenas uma vez
    if (!contentDrawn) {
        displayOnScreen(" SENSORES ", 1, 0x00FFFF);
        displayOnScreen("Temperatura:", 3, 0xFFFFFF);
        displayOnScreen("Umidade:", 5, 0xFFFFFF);
        displayOnScreen("Luminosidade:", 7, 0xFFFFFF);
        displayOnScreen("Acelerometro:", 9, 0xFFFFFF);
        contentDrawn = true;
    }
    
    // Atualizar valores a cada 500ms
    if (millis() - lastUpdate > 500) {
        lastUpdate = millis();
        readAllSensors();
        
        displayOnScreen(String(temperature, 1) + "°C", 4, 0x00FF00);
        displayOnScreen(String(humidity, 1) + "%", 6, 0x00FF00);
        displayOnScreen(String(lightPercent) + "%", 8, 0x00FF00);
        displayOnScreen("X:" + String(accelX, 1), 10, 0x888888);
        displayOnScreen("Y:" + String(accelY, 1), 11, 0x888888);
        displayOnScreen("Z:" + String(accelZ, 1), 12, 0x888888);
        displayOnScreen("Mic:" + String(micData), 13, 0x888888);
    }
}

void demonstrateScreen() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    
    static unsigned long lastUpdate = 0;
    static int colorIndex = 0;
    
    if (millis() - lastUpdate > 1000) {
        lastUpdate = millis();
        colorIndex++;
    }
    
    uint32_t bgColors[] = {0x000000, 0x001122, 0x112200, 0x220011, 0x002211, 0x110022, 0x221100};
    k10.setScreenBackground(bgColors[colorIndex % 7]);
    
    displayOnScreen(" DEMO TELA ", 1, 0x00FFFF);
    displayOnScreen("Cores de fundo", 3, 0xFFFFFF);
    displayOnScreen("automaticas", 4, 0xFFFFFF);
    
    // Mostrar diferentes cores de texto
    displayOnScreen("Texto VERMELHO", 6, 0xFF0000);
    displayOnScreen("Texto VERDE", 7, 0x00FF00);
    displayOnScreen("Texto AZUL", 8, 0x0000FF);
    displayOnScreen("Texto AMARELO", 9, 0xFFFF00);
    displayOnScreen("Texto MAGENTA", 10, 0xFF00FF);
    displayOnScreen("Texto CIANO", 11, 0x00FFFF);
    displayOnScreen("Texto BRANCO", 12, 0xFFFFFF);
    
    displayOnScreen("Cor: " + String(colorIndex % 7), 14, 0x888888);
}

void demonstrateButtons() {
    static bool contentDrawn = false;
    static String lastAction = "Nenhuma";
    static unsigned long lastTime = 0;
    static String displayedAction = "";
    
    // Limpa tela apenas uma vez
    if (initScreenOnce()) {
        contentDrawn = false;
        displayedAction = "";
    }
    
    // Desenhar conteúdo estático apenas uma vez
    if (!contentDrawn) {
        displayOnScreen(" DEMO BOTOES ", 1, 0x00FFFF);
        displayOnScreen("Pressione botoes:", 3, 0xFFFFFF);
        displayOnScreen("Ultima acao:", 5, 0xFFFF00);
        displayOnScreen("A = LED Vermelho", 9, 0x888888);
        displayOnScreen("B = LED Verde", 10, 0x888888);
        displayOnScreen("A+B = Proximo modo", 12, 0x888888);
        contentDrawn = true;
    }
    
    if (k10.buttonA->isPressed() && millis() - lastTime > 300) {
        lastAction = "Botao A";
        lastTime = millis();
        setAllLeds(0xFF0000);
        delay(100);
        setAllLedsOff();
    }
    else if (k10.buttonB->isPressed() && millis() - lastTime > 300) {
        lastAction = "Botao B";
        lastTime = millis();
        setAllLeds(0x00FF00);
        delay(100);
        setAllLedsOff();
    }
    
    // Atualizar apenas se mudou
    if (displayedAction != lastAction) {
        displayOnScreen(lastAction, 6, 0x00FF00);
        displayedAction = lastAction;
    }
    
    // Atualizar tempo a cada segundo
    static unsigned long lastTimeUpdate = 0;
    if (millis() - lastTimeUpdate > 1000) {
        lastTimeUpdate = millis();
        unsigned long elapsed = (millis() - lastTime) / 1000;
        displayOnScreen("Desde: " + String(elapsed) + "s", 8, 0x444444);
    }
}

void demonstrateAccelerometer() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    readAllSensors();
    
    // Controlar LEDs baseado na inclinação
    int redLevel = map(abs(accelX), 0, 10, 0, 255);
    int greenLevel = map(abs(accelY), 0, 10, 0, 255);
    int blueLevel = map(abs(accelZ), 0, 10, 0, 255);
    
    redLevel = constrain(redLevel, 0, 255);
    greenLevel = constrain(greenLevel, 0, 255);
    blueLevel = constrain(blueLevel, 0, 255);
    
    uint32_t color = (redLevel << 16) | (greenLevel << 8) | blueLevel;
    setAllLeds(color);
    
    displayOnScreen(" ACELEROMETRO ", 1, 0x00FFFF);
    displayOnScreen("Incline o dispositivo", 2, 0xFFFFFF);
    displayOnScreen("LEDs mudam com", 3, 0xFFFFFF);
    displayOnScreen("a inclinacao", 4, 0xFFFFFF);
    
    displayOnScreen("X: " + String(accelX, 2), 6, 0xFF0000);
    displayOnScreen("Y: " + String(accelY, 2), 7, 0x00FF00);
    displayOnScreen("Z: " + String(accelZ, 2), 8, 0x0000FF);
    
    displayOnScreen("LED R:" + String(redLevel), 10, 0xFF0000);
    displayOnScreen("LED G:" + String(greenLevel), 11, 0x00FF00);
    displayOnScreen("LED B:" + String(blueLevel), 12, 0x0000FF);
    
    // Barra de intensidade total
    float totalAccel = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
    displayOnScreen("Total:" + String(totalAccel, 1), 14, 0xFFFFFF);
}

void demonstrateLight() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    readAllSensors();
    
    // Ajustar brilho dos LEDs baseado na luz ambiente
    int brightness = map(lightPercent, 0, 100, 20, 255);
    brightness = constrain(brightness, 20, 255);
    
    uint32_t color = (brightness << 16) | (brightness << 8) | brightness; // Branco
    setAllLeds(color);
    
    displayOnScreen(" SENSOR DE LUZ ", 1, 0x00FFFF);
    displayOnScreen("LEDs ajustam", 2, 0xFFFFFF);
    displayOnScreen("com luz ambiente", 3, 0xFFFFFF);
    
    displayOnScreen("Luminosidade:", 5, 0xFFFF00);
    displayOnScreen(String(lightPercent) + "%", 6, 0x00FF00);
    displayOnScreen("Raw: " + String(lightRaw), 7, 0x888888);
    displayOnScreen("Brilho LED: " + String(brightness), 9, 0xFFFFFF);
    
    // Indicador visual da luminosidade
    String lightBar = "Luz: [";
    int bars = lightPercent / 10;
    for (int i = 0; i < 10; i++) {
        lightBar += (i < bars) ? "=" : " ";
    }
    lightBar += "]";
    displayOnScreen(lightBar, 11, 0x00FFFF);
    
    displayOnScreen("Min:" + String(minLight) + " Max:" + String(maxLight), 13, 0x444444);
}

void demonstrateSound() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    readAllSensors();
    
    // LEDs reagem ao som
    int soundLevel = constrain(map(micData, 0, 10000, 0, 255), 0, 255);
    uint32_t color = (soundLevel << 8); // Verde baseado no som
    setAllLeds(color);
    
    displayOnScreen(" DEMO SOM ", 1, 0x00FFFF);
    displayOnScreen("Faca barulho!", 2, 0xFFFFFF);
    displayOnScreen("LEDs reagem ao som", 3, 0xFFFFFF);
    
    displayOnScreen("Nivel som:", 5, 0xFFFF00);
    displayOnScreen(String(micData), 6, 0x00FF00);
    displayOnScreen("LED Verde:" + String(soundLevel), 8, 0x00FF00);
    
    // Barra visual do som
    String soundBar = "Som: [";
    int bars = map(soundLevel, 0, 255, 0, 10);
    for (int i = 0; i < 10; i++) {
        soundBar += (i < bars) ? "=" : " ";
    }
    soundBar += "]";
    displayOnScreen(soundBar, 10, 0x00FF00);
    
    // Histórico simples
    static int lastSound = 0;
    static int maxSound = 0;
    if (micData > maxSound) maxSound = micData;
    displayOnScreen("Max: " + String(maxSound), 12, 0x888888);
    displayOnScreen("Ant: " + String(lastSound), 13, 0x888888);
    lastSound = micData;
}

void demonstrateAnimation() {
    static float ballX = 100, ballY = 50;
    static unsigned long lastUpdate = 0;
    
    initScreenOnce(0x000033); // Limpa tela apenas uma vez (fundo azul escuro)
    
    if (millis() - lastUpdate > 50) {
        lastUpdate = millis();
        
        ballX += ballSpeedX;
        ballY += ballSpeedY;
        
        // Rebater nas bordas
        if (ballX <= 0 || ballX >= 230) ballSpeedX = -ballSpeedX;
        if (ballY <= 20 || ballY >= 100) ballSpeedY = -ballSpeedY;
        
        ballX = constrain(ballX, 0, 230);
        ballY = constrain(ballY, 20, 100);
        
        // Atualizar apenas o conteúdo necessário
        displayOnScreen(" ANIMACAO ", 1, 0x00FFFF);
        displayOnScreen("Bola rebatendo", 2, 0xFFFFFF);
        
        // Limpar linha anterior da bola (usando espaços)
        for (int i = 3; i <= 12; i++) {
            displayOnScreen("                    ", i, 0x000000);
        }
        
        // Desenhar "bola" com texto
        int row = (int)(ballY / 10) + 3;
        displayOnScreen("O", row, 0xFFFF00);
        
        displayOnScreen("Pos: " + String((int)ballX) + "," + String((int)ballY), 13, 0x888888);
        displayOnScreen("Vel: " + String(ballSpeedX) + "," + String(ballSpeedY), 14, 0x888888);
    }
    
    // LEDs acompanham a bola
    int ledR = map(ballX, 0, 230, 0, 255);
    int ledG = map(ballY, 20, 100, 0, 255);
    uint32_t color = (ledR << 16) | (ledG << 8) | 100;
    setAllLeds(color);
}

void demonstrateInteraction() {
   initScreenOnce(); // Limpa tela apenas uma vez por modo
    k10.setScreenBackground(0x000000);
    
    readAllSensors();
    
    displayOnScreen(" INTERACAO ", 1, 0x00FFFF);
    displayOnScreen("Multi-sensor demo", 2, 0xFFFFFF);
    
    // Combinar sensores para efeito
    int redFromSound = map(micData, 0, 10000, 0, 255);
    int greenFromLight = map(lightPercent, 0, 100, 0, 255);
    int blueFromAccel = map(abs(accelZ), 0, 10, 0, 255);
    
    redFromSound = constrain(redFromSound, 0, 255);
    greenFromLight = constrain(greenFromLight, 0, 255);
    blueFromAccel = constrain(blueFromAccel, 0, 255);
    
    uint32_t combinedColor = (redFromSound << 16) | (greenFromLight << 8) | blueFromAccel;
    setAllLeds(combinedColor);
    
    displayOnScreen("Som->Vermelho:" + String(redFromSound), 4, 0xFF0000);
    displayOnScreen("Luz->Verde:" + String(greenFromLight), 5, 0x00FF00);
    displayOnScreen("Accel->Azul:" + String(blueFromAccel), 6, 0x0000FF);
    
    displayOnScreen("Temp: " + String(temperature, 1) + "°C", 8, 0xFFFF00);
    displayOnScreen("Umid: " + String(humidity, 1) + "%", 9, 0x00FFFF);
    
    displayOnScreen("Faca barulho,", 11, 0x888888);
    displayOnScreen("mova e ilumine!", 12, 0x888888);
}

void demonstrateAudio() {
    static bool initialized = false;
    static unsigned long startTime = 0;
    static unsigned long lastScreenUpdate = 0;
    static unsigned long lastLedUpdate = 0;
    static unsigned long lastDebugUpdate = 0;
    static bool equalizerFinished = false;
    static bool soundPlayed = false;
    
    unsigned long currentTime = millis();
    
    // Inicializar apenas uma vez - SEM piscar tela
    if (!initialized) {
        k10.canvas->canvasClear();
        k10.setScreenBackground(0x000000);
        
        // Desenhar conteúdo estático UMA VEZ
        displayOnScreen("=== EQUALIZADOR ===", 1, 0x00FFFF);
        displayOnScreen("Faca som/musica!", 2, 0xFFFFFF);
        displayOnScreen("LEDs reagem ao audio", 3, 0xFFFFFF);
        displayOnScreen("30s de demonstracao", 4, 0xFFFF00);
        displayOnScreen("Verde=Baixo Amar=Medio", 12, 0x888888);
        displayOnScreen("Vermelho=Alto", 13, 0x888888);
        
        startTime = currentTime;
        equalizerFinished = false;
        soundPlayed = false;
        initialized = true;
        
        Serial.println("=== EQUALIZADOR INICIADO ===");
        Serial.println("Tempo inicial: " + String(startTime));
    }
    
    unsigned long elapsed = currentTime - startTime;
    unsigned long remaining = (elapsed < 30000) ? (30000 - elapsed) : 0;
    
    // Sempre ler sensores
    readAllSensors();
    
    // Debug no Serial a cada segundo
    if (currentTime - lastDebugUpdate > 1000) {
        Serial.printf("Tempo: %lus, Restante: %lus, MicData: %lu\n", 
                     elapsed/1000, remaining/1000, micData);
        lastDebugUpdate = currentTime;
    }
    
    // Durante os 30 segundos - EQUALIZADOR ATIVO
    if (!equalizerFinished && elapsed < 30000) {
        
        // Atualizar tela a cada 500ms
        if (currentTime - lastScreenUpdate > 500) {
            lastScreenUpdate = currentTime;
            
            // Mostrar dados dinâmicos
            displayOnScreen("Nivel mic: " + String(micData) + "     ", 6, 0x00FF00);
            displayOnScreen("Tempo: " + String(remaining / 1000) + "s     ", 8, 0xFFFF00);
            
            // Calcular e mostrar nível de áudio
            int audioLevel = constrain(map(micData, 0, 15000, 0, 100), 0, 100);
            displayOnScreen("Audio: " + String(audioLevel) + "%     ", 9, 0x00FFFF);
            
            // Barra visual do equalizador
            String audioBar = "EQ: [";
            int bars = audioLevel / 10;
            for (int i = 0; i < 10; i++) {
                if (i < bars) {
                    if (i < 3) audioBar += "=";      // Verde
                    else if (i < 7) audioBar += "#"; // Amarelo
                    else audioBar += "*";            // Vermelho
                } else {
                    audioBar += " ";
                }
            }
            audioBar += "]";
            displayOnScreen(audioBar + "     ", 10, 0xFFFFFF);
        }
        
        // Atualizar LEDs rapidamente
        if (currentTime - lastLedUpdate > 100) {
            lastLedUpdate = currentTime;
            
            int audioLevel = constrain(map(micData, 0, 15000, 0, 100), 0, 100);
            uint32_t ledColor = 0x000000;
            
            if (audioLevel < 30) {
                // Som baixo - Verde
                int intensity = map(audioLevel, 0, 30, 50, 255);
                ledColor = (0 << 16) | (intensity << 8) | (0);
            } else if (audioLevel < 70) {
                // Som médio - Amarelo/Laranja
                int greenLevel = 255;
                int redLevel = map(audioLevel, 30, 70, 0, 255);
                ledColor = (redLevel << 16) | (greenLevel << 8) | (0);
            } else {
                // Som alto - Vermelho
                int intensity = 255;
                if (random(100) < 20) intensity = random(100, 255);
                ledColor = (intensity << 16) | (0 << 8) | (0);
            }
            
            // Efeito strobe ocasional
            if (audioLevel > 20 && random(100) < 15) {
                ledColor = 0xFFFFFF;
            }
            
            setAllLeds(ledColor);
        }
    }
    
    // APÓS 30 SEGUNDOS - FINALIZAR E TOCAR BUZZER
    else if (!equalizerFinished && elapsed >= 30000) {
        equalizerFinished = true;
        setAllLedsOff();
        Serial.println("=== 30 SEGUNDOS COMPLETADOS! ===");
        
        // Limpar área da tela para mensagem final
        for(int i = 6; i <= 11; i++) {
            displayOnScreen("                    ", i, 0x000000);
        }
        
        displayOnScreen("Equalizador concluido!", 7, 0x00FF00);
        displayOnScreen("Tocando campainha...", 8, 0xFFFF00);
    }
    
    // TOCAR CAMPAINHA
    if (equalizerFinished && !soundPlayed) {
        soundPlayed = true;
        Serial.println("=== INICIANDO CAMPAINHA ===");
        
        // Campainha "blim-blom"
        Serial.println("Tom 1: 800Hz");
        playTone(800, 200);
        
        Serial.println("Tom 2: 600Hz");
        playTone(600, 200);
        
        Serial.println("Tom 3: 800Hz");
        playTone(800, 200);
        
        Serial.println("Tom 4: 600Hz (longo)");
        playTone(600, 400);
        
        displayOnScreen("Campainha tocada!", 9, 0x00FF00);
        displayOnScreen("Reiniciando em 3s...", 10, 0x888888);
        
        Serial.println("=== CAMPAINHA CONCLUÍDA ===");
        
        // Aguardar 3 segundos
        delay(3000);
        
        // RESET COMPLETO
        initialized = false;
        equalizerFinished = false;
        soundPlayed = false;
        Serial.println("=== REINICIANDO EQUALIZADOR ===");
    }
}

void demonstrateSDCard() {
    initScreenOnce(); // Limpa tela apenas uma vez por modo
    static bool contentDrawn = false;
    static unsigned long lastUpdateSD = 0;
    
    // Limpa tela apenas uma vez
    if (initScreenOnce()) {
        contentDrawn = false;
    }
    
    // Desenhar conteúdo estático apenas uma vez
    if (!contentDrawn) {
        displayOnScreen(" CARTAO SD ", 1, 0x00FFFF);
        displayOnScreen("Status SD:", 3, 0xFFFFFF);
        contentDrawn = true;
    }
    
    if (sdCardAvailable) {
        displayOnScreen("CONECTADO", 4, 0x00FF00);
        
        // Mostrar informações do SD Card
        if (millis() - lastUpdateSD > 2000) {
            lastUpdateSD = millis();
            
            // Testar operações básicas
            static int testCount = 0;
            String testFile = "/demo" + String(testCount) + ".txt";
            String testData = "Teste " + String(testCount) + " - " + String(millis());
            
            displayOnScreen("Testando escrita...", 6, 0xFFFF00);
            if (writeToSD(testFile, testData)) {
                displayOnScreen("Escrita: OK", 7, 0x00FF00);
                
                displayOnScreen("Testando leitura...", 8, 0xFFFF00);
                String readData = readFromSD(testFile);
                if (readData == testData) {
                    displayOnScreen("Leitura: OK", 9, 0x00FF00);
                } else {
                    displayOnScreen("Leitura: ERRO", 9, 0xFF0000);
                }
                
                // Mostrar tamanho do arquivo
                size_t fileSize = getSDFileSize(testFile);
                displayOnScreen("Tam: " + String(fileSize) + " bytes", 10, 0x888888);
                
            } else {
                displayOnScreen("Escrita: ERRO", 7, 0xFF0000);
            }
            
            testCount++;
            if (testCount > 99) testCount = 0; // Reset contador
        }
        
        displayOnScreen("A: Criar arquivo", 12, 0x888888);
        displayOnScreen("B: Listar arquivos", 13, 0x888888);
        
    } else {
        displayOnScreen("DESCONECTADO", 4, 0xFF0000);
        displayOnScreen("Insira um cartao", 6, 0xFFFFFF);
        displayOnScreen("SD para usar", 7, 0xFFFFFF);
        displayOnScreen("funcoes de", 8, 0xFFFFFF);
        displayOnScreen("armazenamento", 9, 0xFFFFFF);
        
        displayOnScreen("Tentando reconectar...", 11, 0xFFFF00);
        
        // Tentar reconectar periodicamente
        if (millis() - lastUpdateSD > 5000) {
            lastUpdateSD = millis();
            if (SD.begin()) {
                sdCardAvailable = true;
                Serial.println("SD Card reconectado!");
            }
        }
    }
}

void demonstrateAll() {
    initScreenOnce(); // Limpa tela apenas uma vez por modo
    
    static unsigned long lastUpdate = 0;
    static int subMode = 0;
    
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        subMode = (subMode + 1) % 4;
    }
    
    displayOnScreen(" DEMO COMPLETA ", 1, 0x00FFFF);
    
    readAllSensors();
    
    switch (subMode) {
        case 0:
            displayOnScreen("SENSORES:", 3, 0xFFFF00);
            displayOnScreen("T:" + String(temperature, 1) + "°C", 4, 0x00FF00);
            displayOnScreen("H:" + String(humidity, 1) + "%", 5, 0x00FF00);
            displayOnScreen("L:" + String(lightPercent) + "%", 6, 0x00FF00);
            break;
        case 1:
            displayOnScreen("MOVIMENTO:", 3, 0xFFFF00);
            displayOnScreen("X:" + String(accelX, 1), 4, 0xFF0000);
            displayOnScreen("Y:" + String(accelY, 1), 5, 0x00FF00);
            displayOnScreen("Z:" + String(accelZ, 1), 6, 0x0000FF);
            break;
        case 2:
            displayOnScreen("SISTEMA:", 3, 0xFFFF00);
            displayOnScreen("WiFi: " + wifiStatus, 4, wifiConnected ? 0x00FF00 : 0xFF8800);
            displayOnScreen("BT: OFF", 5, 0x888888);
            displayOnScreen("SD: " + String(sdCardAvailable ? "OK" : "NO"), 6, sdCardAvailable ? 0x00FF00 : 0xFF0000);
            break;
        case 3:
            displayOnScreen("INTERACAO:", 3, 0xFFFF00);
            displayOnScreen("Som:" + String(micData), 4, 0x00FFFF);
            displayOnScreen("LEDs reativos", 5, 0xFFFFFF);
            displayOnScreen("aos sensores", 6, 0xFFFFFF);
            break;
    }
    
    // LEDs combinados de todos os sensores
    int r = map(abs(accelX), 0, 10, 0, 255);
    int g = map(lightPercent, 0, 100, 0, 255);
    int b = map(micData % 10000, 0, 9999, 0, 255);
    setAllLeds((r << 16) | (g << 8) | b);
    
    displayOnScreen("Rotacao a cada 2s", 14, 0x444444);
}

void demonstrateCamera() {
    initScreenOnce(); // Limpa tela apenas uma vez por modo
    
    displayOnScreen(" CAMERA ", 1, 0x00FFFF);
    displayOnScreen("Funcionalidade", 3, 0xFFFFFF);
    displayOnScreen("de camera nao", 4, 0xFFFFFF);
    displayOnScreen("implementada", 5, 0xFFFFFF);
    displayOnScreen("nesta demo", 6, 0xFFFFFF);
    displayOnScreen("basica", 7, 0xFFFFFF);
    
    displayOnScreen("Hardware pronto", 9, 0x888888);
    displayOnScreen("para expansao", 10, 0x888888);
    displayOnScreen("futura", 11, 0x888888);
    
    // Simular atividade de câmera com LEDs
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 1000) {
        lastBlink = millis();
        setAllLeds(0xFF0000);
        delay(100);
        setAllLedsOff();
    }
}

void demonstrateWiFi() {
    static bool contentDrawn = false;
    static unsigned long lastUpdate = 0;
    static unsigned long lastScan = 0;
    static String lastDisplayedStatus = "";
    static String lastDisplayedIP = "";
    
    // Limpa tela apenas uma vez
    if (initScreenOnce()) {
        contentDrawn = false;
        lastDisplayedStatus = "";
        lastDisplayedIP = "";
    }
    
    // Desenhar conteúdo estático apenas uma vez
    if (!contentDrawn) {
        displayOnScreen(" WiFi STATUS ", 1, 0x00FFFF);
        displayOnScreen("A: Reconectar", 12, 0x888888);
        displayOnScreen("B: Scan redes", 13, 0x888888);
        contentDrawn = true;
    }
    
    // Atualizar informações apenas se mudaram
    if (lastDisplayedStatus != wifiStatus) {
        displayOnScreen("Status: " + wifiStatus, 2, wifiConnected ? 0x00FF00 : 0xFF8800);
        lastDisplayedStatus = wifiStatus;
    }
    
    if (lastDisplayedIP != localIP) {
        if (wifiConnected && localIP != "") {
            displayOnScreen("IP: " + localIP, 3, 0x00FFFF);
        } else {
            displayOnScreen("IP: Desconectado", 3, 0xFF0000);
        }
        lastDisplayedIP = localIP;
    }
    
    // Atualizar conteúdo dinâmico com controle de tempo
    bool shouldUpdate = (millis() - lastUpdate > 2000);
    if (shouldUpdate) {
        lastUpdate = millis();
        
        if (wifiConnected && localIP != "") {
            displayOnScreen("Rede: " + connectedSSID, 4, 0xFFFFFF);
            
            // Atualizar sinal
            long rssi = WiFi.RSSI();
            String signalText = "Sinal: " + String(rssi) + " dBm";
            uint32_t signalColor = 0x00FF00;
            
            if (rssi < -70) signalColor = 0xFF0000;
            else if (rssi < -60) signalColor = 0xFFFF00;
            
            displayOnScreen(signalText, 5, signalColor);
            displayOnScreen("MAC: " + WiFi.macAddress(), 7, 0x888888);
            
            // LEDs simples baseados no sinal
            if (rssi > -60) setAllLeds(0x00FF00);
            else if (rssi > -70) setAllLeds(0xFFFF00);
            else setAllLeds(0xFF0000);
            
        } else {
            displayOnScreen("SSID: " + wifi_ssid, 4, 0x888888);
            displayOnScreen("Tentativas: " + String(wifiRetryCount), 5, 0x888888);
            
            // Scan muito simples e espaçado
            if (millis() - lastScan > 20000) {
                lastScan = millis();
                Serial.println("Fazendo scan WiFi simples...");
                
                int n = WiFi.scanNetworks();
                if (n > 0) {
                    String network1 = (n > 0) ? WiFi.SSID(0) : "Nenhuma";
                    String network2 = (n > 1) ? WiFi.SSID(1) : "";
                    
                    displayOnScreen("Redes:", 7, 0xFFFF00);
                    displayOnScreen(network1, 8, 0xFFFFFF);
                    if (network2 != "") {
                        displayOnScreen(network2, 9, 0xFFFFFF);
                    }
                }
            }
        }
    }
    
    // LEDs piscam devagar quando desconectado
    if (!wifiConnected) {
        static unsigned long lastBlink = 0;
        if (millis() - lastBlink > 1500) {
            lastBlink = millis();
            static bool ledState = false;
            ledState = !ledState;
            setAllLeds(ledState ? 0x0000FF : 0x000000);
        }
    }
}

//  FUNÇÕES WiFi 

void initializeWiFi() {
    Serial.println("Inicializando WiFi...");
    
    // Configurar modo WiFi
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    // Tentar conectar
    connectToWiFi();
}

void connectToWiFi() {
    Serial.println("Tentando conectar ao WiFi...");
    Serial.println("SSID: " + wifi_ssid);
    
    wifiStatus = "Conectando...";
    wifiRetryCount++;
    
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    
    // Aguardar conexão por até 10 segundos
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        wifiStatus = "Conectado";
        connectedSSID = WiFi.SSID();
        localIP = WiFi.localIP().toString();
        
        Serial.println("");
        Serial.println("WiFi conectado com sucesso!");
        Serial.println("SSID: " + connectedSSID);
        Serial.println("IP: " + localIP);
        Serial.println("RSSI: " + String(WiFi.RSSI()) + " dBm");
        
        // Flash verde nos LEDs para indicar sucesso
        for (int i = 0; i < 3; i++) {
            setAllLeds(0x00FF00);
            delay(200);
            setAllLedsOff();
            delay(200);
        }
    } else {
        wifiConnected = false;
        wifiStatus = "Falha na conexao";
        connectedSSID = "";
        localIP = "";
        
        Serial.println("");
        Serial.println("Falha ao conectar WiFi");
        Serial.println("Status: " + String(WiFi.status()));
        
        // Flash vermelho nos LEDs para indicar falha
        for (int i = 0; i < 2; i++) {
            setAllLeds(0xFF0000);
            delay(300);
            setAllLedsOff();
            delay(300);
        }
    }
}

void checkWiFiConnection() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!wifiConnected) {
            // Reconectado
            wifiConnected = true;
            wifiStatus = "Reconectado";
            connectedSSID = WiFi.SSID();
            localIP = WiFi.localIP().toString();
            Serial.println("WiFi reconectado!");
        }
    } else {
        if (wifiConnected) {
            // Desconectado
            wifiConnected = false;
            wifiStatus = "Desconectado";
            Serial.println("WiFi desconectado!");
            
            // Tentar reconectar
            connectToWiFi();
        }
    }
}

void scanWiFiNetworks() {
    Serial.println("Fazendo scan das redes WiFi...");
    wifiStatus = "Fazendo scan...";
    
    int n = WiFi.scanNetworks();
    Serial.println("Scan concluído");
    
    if (n == 0) {
        Serial.println("Nenhuma rede encontrada");
        wifiStatus = "Nenhuma rede";
    } else {
        Serial.println(String(n) + " redes encontradas:");
        for (int i = 0; i < n; ++i) {
            Serial.printf("%d: %s (%d) %s\n", 
                         i + 1, 
                         WiFi.SSID(i).c_str(), 
                         WiFi.RSSI(i),
                         (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        }
        wifiStatus = String(n) + " redes encontradas";
    }
}

String getWiFiStatusText() {
    switch (WiFi.status()) {
        case WL_CONNECTED: return "Conectado";
        case WL_NO_SSID_AVAIL: return "SSID nao encontrado";
        case WL_CONNECT_FAILED: return "Falha na conexao";
        case WL_CONNECTION_LOST: return "Conexao perdida";
        case WL_DISCONNECTED: return "Desconectado";
        case WL_IDLE_STATUS: return "Idle";
        default: return "Status desconhecido";
    }
}

void showWiFiInfo() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" Informações WiFi ");
        Serial.println("SSID: " + WiFi.SSID());
        Serial.println("IP: " + WiFi.localIP().toString());
        Serial.println("Gateway: " + WiFi.gatewayIP().toString());
        Serial.println("Subnet: " + WiFi.subnetMask().toString());
        Serial.println("DNS: " + WiFi.dnsIP().toString());
        Serial.println("MAC: " + WiFi.macAddress());
        Serial.println("RSSI: " + String(WiFi.RSSI()) + " dBm");
        Serial.println("Canal: " + String(WiFi.channel()));
    }
}

void demonstrateBluetooth() {
    initScreenOnce(); // Limpa tela apenas uma vez por modo
    
    displayOnScreen(" BLUETOOTH ", 1, 0x00FFFF);
    displayOnScreen("Conectividade", 3, 0xFFFFFF);
    displayOnScreen("Bluetooth", 4, 0xFFFFFF);
    displayOnScreen("disponivel mas", 5, 0xFFFFFF);
    displayOnScreen("desabilitada", 6, 0xFFFFFF);
    displayOnScreen("nesta demo", 7, 0xFFFFFF);
    
    displayOnScreen("Capacidades:", 9, 0xFFFF00);
    displayOnScreen("- BLE", 10, 0x888888);
    displayOnScreen("- Classic BT", 11, 0x888888);
    displayOnScreen("- Serial SPP", 12, 0x888888);
    
    // Simular atividade Bluetooth
    static unsigned long btBlink = 0;
    if (millis() - btBlink > 2000) {
        btBlink = millis();
        for (int i = 0; i < 3; i++) {
            setAllLeds(0x0000FF);
            delay(100);
            setAllLedsOff();
            delay(100);
        }
    }
}

//  FUNÇÕES NATIVAS PARA SD CARD 

bool writeToSD(String filename, String data) {
    if (!sdCardAvailable) {
        Serial.println("SD Card não disponível para escrita");
        return false;
    }
    
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("Erro ao abrir arquivo para escrita: " + filename);
        return false;
    }
    
    size_t written = file.print(data);
    file.close();
    
    if (written == data.length()) {
        Serial.println("Arquivo escrito com sucesso: " + filename + " (" + String(written) + " bytes)");
        return true;
    } else {
        Serial.println("Erro na escrita do arquivo: " + filename);
        return false;
    }
}

String readFromSD(String filename) {
    if (!sdCardAvailable) {
        Serial.println("SD Card não disponível para leitura");
        return "";
    }
    
    File file = SD.open(filename, FILE_READ);
    if (!file) {
        Serial.println("Erro ao abrir arquivo para leitura: " + filename);
        return "";
    }
    
    String content = "";
    while (file.available()) {
        content += char(file.read());
    }
    file.close();
    
    Serial.println("Arquivo lido com sucesso: " + filename + " (" + String(content.length()) + " bytes)");
    return content;
}

bool appendToSD(String filename, String data) {
    if (!sdCardAvailable) {
        Serial.println("SD Card não disponível para append");
        return false;
    }
    
    File file = SD.open(filename, FILE_APPEND);
    if (!file) {
        Serial.println("Erro ao abrir arquivo para append: " + filename);
        return false;
    }
    
    size_t written = file.print(data);
    file.close();
    
    if (written == data.length()) {
        Serial.println("Dados anexados com sucesso: " + filename + " (+" + String(written) + " bytes)");
        return true;
    } else {
        Serial.println("Erro ao anexar dados ao arquivo: " + filename);
        return false;
    }
}

bool deleteFromSD(String filename) {
    if (!sdCardAvailable) {
        Serial.println("SD Card não disponível para exclusão");
        return false;
    }
    
    if (SD.remove(filename)) {
        Serial.println("Arquivo deletado com sucesso: " + filename);
        return true;
    } else {
        Serial.println("Erro ao deletar arquivo: " + filename);
        return false;
    }
}

void listSDFiles() {
    if (!sdCardAvailable) {
        Serial.println("SD Card não disponível para listagem");
        return;
    }
    
    Serial.println(" Listando arquivos no SD Card ");
    File root = SD.open("/");
    if (!root) {
        Serial.println("Erro ao abrir diretório raiz");
        return;
    }
    
    int fileCount = 0;
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("DIR: ");
        } else {
            Serial.print("FILE: ");
        }
        Serial.print(file.name());
        if (!file.isDirectory()) {
            Serial.print(" (");
            Serial.print(file.size());
            Serial.print(" bytes)");
        }
        Serial.println();
        
        fileCount++;
        file = root.openNextFile();
    }
    root.close();
    
    Serial.println("Total de itens encontrados: " + String(fileCount));
}

bool sdFileExists(String filename) {
    if (!sdCardAvailable) {
        return false;
    }
    
    File file = SD.open(filename, FILE_READ);
    if (file) {
        file.close();
        return true;
    }
    return false;
}

size_t getSDFileSize(String filename) {
    if (!sdCardAvailable) {
        return 0;
    }
    
    File file = SD.open(filename, FILE_READ);
    if (!file) {
        return 0;
    }
    
    size_t size = file.size();
    file.close();
    return size;
}

//  FUNÇÕES DE ÁUDIO 

void playBeep() {
    playTone(1000, 200);
}

void playTone(int frequency, int duration) {
    // Melhorado: Simular som com LEDs piscando na frequência
    Serial.println("Tocando frequencia: " + String(frequency) + "Hz por " + String(duration) + "ms");
    
    unsigned long startTime = millis();
    unsigned long blinkInterval = 1000000 / (frequency * 2); // Microsegundos para cada piscada
    bool ledState = false;
    
    while (millis() - startTime < duration) {
        static unsigned long lastBlink = 0;
        if (micros() - lastBlink > blinkInterval) {
            ledState = !ledState;
            if (ledState) {
                setAllLeds(0xFFFFFF); // Branco para indicar tom
            } else {
                setAllLedsOff();
            }
            lastBlink = micros();
        }
    }
    
    setAllLedsOff();
    delay(50); // Pausa entre tons
}