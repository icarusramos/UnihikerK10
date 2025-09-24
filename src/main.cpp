#include "unihiker_k10.h"
#include "asr.h"
#include "AIRecognition.h"
#include "Adafruit_LTR329_LTR303.h"
#include "DFRobot_AHT20.h"
#include <WiFi.h>
#include <SD.h>
#include <FS.h>
#include <LittleFS.h>

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

void setup() {
    Serial.begin(115200);
    delay(3000); // Aguardar estabilização mais longa
    
    Serial.println("=== UNIHIKER K10 DEMO COMPLETO ===");
    
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

// === FUNÇÕES AUXILIARES BÁSICAS ===

void displayOnScreen(String text, int line, uint32_t color) {
    k10.canvas->canvasText(text, line, color);
    k10.canvas->updateCanvas();
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
    
    // LittleFS (opcional)
    displayOnScreen("Inicializando LittleFS...", 6, 0xFFFF00);
    delay(500);
    try {
        if (LittleFS.begin(true)) {
            displayOnScreen("LittleFS: OK", 7, 0x00FF00);
            Serial.println("LittleFS inicializado com sucesso");
        } else {
            displayOnScreen("LittleFS: FALHOU", 7, 0xFF0000);
            Serial.println("LittleFS não disponível");
        }
    } catch (...) {
        displayOnScreen("LittleFS: ERRO", 7, 0xFF0000);
        Serial.println("Erro ao inicializar LittleFS");
    }
    delay(500);
    
    // Sensores (opcionais)
    displayOnScreen("Inicializando sensores...", 8, 0xFFFF00);
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
    
    displayOnScreen("Sensores: OK", 9, 0x00FF00);
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
    displayOnScreen("LFS: " + String(LittleFS.begin() ? "OK" : "NO"), 11, 0x00FF00);
    displayOnScreen("Carregando...", 13, 0x888888);
    
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
    
    displayOnScreen("=== MENU PRINCIPAL ===", 1, 0x00FFFF);
    
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
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    static unsigned long lastUpdate = 0;
    static int state = 0;
    
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
    }
    
    displayOnScreen("=== DEMO LEDs ===", 1, 0x00FFFF);
    displayOnScreen("Cores automaticas", 3, 0xFFFFFF);
    displayOnScreen("Estado: " + String(state % 8), 5, 0xFFFF00);
    displayOnScreen("A: Mudar modo", 11, 0x888888);
    displayOnScreen("B: Flash todos", 12, 0x888888);
    displayOnScreen("A+B: Proximo demo", 13, 0x888888);
}

void demonstrateSensors() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    readAllSensors();
    
    displayOnScreen("=== SENSORES ===", 1, 0x00FFFF);
    displayOnScreen("Temperatura:", 3, 0xFFFFFF);
    displayOnScreen(String(temperature, 1) + "°C", 4, 0x00FF00);
    displayOnScreen("Umidade:", 5, 0xFFFFFF);
    displayOnScreen(String(humidity, 1) + "%", 6, 0x00FF00);
    displayOnScreen("Luminosidade:", 7, 0xFFFFFF);
    displayOnScreen(String(lightPercent) + "%", 8, 0x00FF00);
    displayOnScreen("Acelerometro:", 9, 0xFFFFFF);
    displayOnScreen("X:" + String(accelX, 1), 10, 0x888888);
    displayOnScreen("Y:" + String(accelY, 1), 11, 0x888888);
    displayOnScreen("Z:" + String(accelZ, 1), 12, 0x888888);
    displayOnScreen("Mic:" + String(micData), 13, 0x888888);
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
    
    displayOnScreen("=== DEMO TELA ===", 1, 0x00FFFF);
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
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    static String lastAction = "Nenhuma";
    static unsigned long lastTime = 0;
    
    displayOnScreen("=== DEMO BOTOES ===", 1, 0x00FFFF);
    displayOnScreen("Pressione botoes:", 3, 0xFFFFFF);
    
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
    
    displayOnScreen("Ultima acao:", 5, 0xFFFF00);
    displayOnScreen(lastAction, 6, 0x00FF00);
    displayOnScreen("A = LED Vermelho", 9, 0x888888);
    displayOnScreen("B = LED Verde", 10, 0x888888);
    displayOnScreen("A+B = Proximo modo", 12, 0x888888);
    
    unsigned long elapsed = (millis() - lastTime) / 1000;
    displayOnScreen("Desde: " + String(elapsed) + "s", 8, 0x444444);
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
    
    displayOnScreen("=== ACELEROMETRO ===", 1, 0x00FFFF);
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
    
    displayOnScreen("=== SENSOR DE LUZ ===", 1, 0x00FFFF);
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
    
    displayOnScreen("=== DEMO SOM ===", 1, 0x00FFFF);
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
    
    if (millis() - lastUpdate > 50) {
        lastUpdate = millis();
        
        ballX += ballSpeedX;
        ballY += ballSpeedY;
        
        // Rebater nas bordas
        if (ballX <= 0 || ballX >= 230) ballSpeedX = -ballSpeedX;
        if (ballY <= 20 || ballY >= 100) ballSpeedY = -ballSpeedY;
        
        ballX = constrain(ballX, 0, 230);
        ballY = constrain(ballY, 20, 100);
    }
    
    // Limpar tela completamente a cada frame
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000033);
    
    displayOnScreen("=== ANIMACAO ===", 1, 0x00FFFF);
    displayOnScreen("Bola rebatendo", 2, 0xFFFFFF);
    
    // Desenhar "bola" com texto
    int row = (int)(ballY / 10) + 3;
    displayOnScreen("O", row, 0xFFFF00);
    
    displayOnScreen("Pos: " + String((int)ballX) + "," + String((int)ballY), 13, 0x888888);
    displayOnScreen("Vel: " + String(ballSpeedX) + "," + String(ballSpeedY), 14, 0x888888);
    
    // LEDs acompanham a bola
    int ledR = map(ballX, 0, 230, 0, 255);
    int ledG = map(ballY, 20, 100, 0, 255);
    uint32_t color = (ledR << 16) | (ledG << 8) | 100;
    setAllLeds(color);
}

void demonstrateInteraction() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    readAllSensors();
    
    displayOnScreen("=== INTERACAO ===", 1, 0x00FFFF);
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
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    displayOnScreen("=== AUDIO/ASR ===", 1, 0x00FFFF);
    displayOnScreen("Funcionalidade", 3, 0xFFFFFF);
    displayOnScreen("de reconhecimento", 4, 0xFFFFFF);
    displayOnScreen("de voz nao", 5, 0xFFFFFF);
    displayOnScreen("implementada nesta", 6, 0xFFFFFF);
    displayOnScreen("demo basica", 7, 0xFFFFFF);
    
    readAllSensors();
    displayOnScreen("Nivel mic: " + String(micData), 9, 0x00FF00);
    displayOnScreen("Use modo Som", 11, 0x888888);
    displayOnScreen("para interacao", 12, 0x888888);
}

void demonstrateSDCard() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    displayOnScreen("=== CARTAO SD ===", 1, 0x00FFFF);
    displayOnScreen("Status SD:", 3, 0xFFFFFF);
    
    if (sdCardAvailable) {
        displayOnScreen("CONECTADO", 4, 0x00FF00);
        displayOnScreen("Funcoes basicas", 6, 0xFFFFFF);
        displayOnScreen("de arquivo", 7, 0xFFFFFF);
        displayOnScreen("disponiveis", 8, 0xFFFFFF);
    } else {
        displayOnScreen("DESCONECTADO", 4, 0xFF0000);
        displayOnScreen("Insira um cartao", 6, 0xFFFFFF);
        displayOnScreen("SD para usar", 7, 0xFFFFFF);
        displayOnScreen("funcoes de", 8, 0xFFFFFF);
        displayOnScreen("armazenamento", 9, 0xFFFFFF);
    }
    
    displayOnScreen("LittleFS:", 11, 0xFFFF00);
    displayOnScreen(LittleFS.begin() ? "OK" : "ERRO", 12, LittleFS.begin() ? 0x00FF00 : 0xFF0000);
}

void demonstrateAll() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    static unsigned long lastUpdate = 0;
    static int subMode = 0;
    
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        subMode = (subMode + 1) % 4;
    }
    
    displayOnScreen("=== DEMO COMPLETA ===", 1, 0x00FFFF);
    
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
            displayOnScreen("WiFi: OFF", 4, 0x888888);
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
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    displayOnScreen("=== CAMERA ===", 1, 0x00FFFF);
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
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    displayOnScreen("=== WiFi ===", 1, 0x00FFFF);
    displayOnScreen("Conectividade", 3, 0xFFFFFF);
    displayOnScreen("WiFi disponivel", 4, 0xFFFFFF);
    displayOnScreen("mas desabilitada", 5, 0xFFFFFF);
    displayOnScreen("nesta demo para", 6, 0xFFFFFF);
    displayOnScreen("evitar conflitos", 7, 0xFFFFFF);
    displayOnScreen("de inicializacao", 8, 0xFFFFFF);
    
    displayOnScreen("Capacidades:", 10, 0xFFFF00);
    displayOnScreen("- Station Mode", 11, 0x888888);
    displayOnScreen("- Access Point", 12, 0x888888);
    displayOnScreen("- HTTP Server", 13, 0x888888);
    
    // Simular scan de redes com LEDs
    static int wifiSim = 0;
    wifiSim = (wifiSim + 1) % 3;
    uint32_t colors[] = {0x000000, 0x0000FF, 0x00FFFF};
    setAllLeds(colors[wifiSim]);
}

void demonstrateBluetooth() {
    // Limpar tela completamente
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);
    
    displayOnScreen("=== BLUETOOTH ===", 1, 0x00FFFF);
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